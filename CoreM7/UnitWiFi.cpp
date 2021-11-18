#include "UnitWiFi.h"

#include <mbed.h>
#include <rtos.h>
#include <SPI.h>
#include <RPC_internal.h>
#include "InternalComm.h"
#include "BoxSettings.h"

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

// this file contains the wifi credentials for now, this will change
// as the project progresses. the idea is to save the credentials in the
// QSPI flash and then get them from there before connecting. they could
// been entered/changed through the touch screen user interface
#include "arduino_secrets.h"

#define BYTES_PER_SENSOR 6
#define MIN_ROWS_PER_PACKET 2000
#define MYLED LEDB

#define TYPE_UINT8  0x00
#define TYPE_UINT16 0x01
#define TYPE_UINT32 0x02
#define TYPE_UINT64 0x03
#define TYPE_FLOAT  0x04
#define TYPE_DOUBLE 0x05

UnitWiFi::UnitWiFi(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer)
    : crcBuffer(buffer)
    , currentMode(WiFiMode::SEND_TO_DATASERVER) {
    Serial.println("SSID: " + String(SECRET_SSID));
    Serial.println("Pass: " + String(SECRET_PASS));
    Serial.println("Mode: " + String(currentMode));
}

UnitWiFi::~UnitWiFi() {
    delete mqttClient;
}

void UnitWiFi::runWiFi(WiFiClient &client) {
    // Setup
    Serial.println("Setting up wifi, connecting...");

    // MQTT setup
    mqttClient = new PubSubClient(client);
    mqttClient->setServer(IPAddress(BoxSettings.getBrokerAddress()), BoxSettings.getBrokerPort());
    // mqttClient->setCallback(receiveMQTTMessage);

    // setup blue led for output and turn it off
    pinMode(MYLED, OUTPUT);
    digitalWrite(MYLED, HIGH);

    // connect to wifi
    connectWiFi();

    Serial.println("Entering WiFi loop...");
    // Loop
    while (running) {
        switch (currentMode) {

        case SEND_TO_DATASERVER:
            loopSendToDataServer(client);
            //Serial.println("--SENDING TO DATASERVER......--");
            //rtos::ThisThread::sleep_for(200);
            break;

        case REPORT_TO_BROKER:
            loopReportToBroker(client);
            //Serial.println("--REPORTING TO BROKER........--");
            //rtos::ThisThread::sleep_for(200);
            break;

        case IDLE:
            rtos::ThisThread::sleep_for(200);
            break;

        }
    }
}

void UnitWiFi::stopWiFi() {
    running = false;
}

void UnitWiFi::flush() {
    flushBuffer = true;
}

void UnitWiFi::setMode(WiFiMode mode) {
    if (currentMode == REPORT_TO_BROKER && mode != REPORT_TO_BROKER) {
        Serial.println("Disconnecting MQTT...");
        mqttClient->disconnect();
    }

    currentMode = mode;
}

void UnitWiFi::connectWiFi() {
    char ssid[] = SECRET_SSID;
    char pass[] = SECRET_PASS;

    static bool firstConnect = true;
    if (firstConnect) {
        firstConnect = false;
        Serial.println("Making first WiFi connection, waiting 1.3 seconds for UI to initialize");
        rtos::ThisThread::sleep_for(1300);
    }

    Serial.println("Connecting to network " + String(SECRET_SSID) + "...");
    RPC1.println("POST wifi/status connecting");
    digitalWrite(MYLED, LOW);

    while (status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        //delay(1000); // 5000
    }
    digitalWrite(MYLED, HIGH);
    RPC1.println("POST wifi/status OK");

    printWifiStatus();
}

void UnitWiFi::loopSendToDataServer(WiFiClient &client) {
    if (crcBuffer->size() > MIN_ROWS_PER_PACKET || flushBuffer) {
        if (crcBuffer->size() <= MIN_ROWS_PER_PACKET) flushBuffer = false;

        digitalWrite(LEDG, LOW);

        Serial.println("\nStarting connection to data server...");
        if (client.connect(IPAddress(BoxSettings.getDataServerAddress()), BoxSettings.getDataServerPort())) {
            Serial.println("Sending data to data server");
            sendBuffer(client);

            // could also read from the server here, maybe implement that later
        } else {
            Serial.println("could not connect!");
            displayConnectError();
        }

        client.stop();
        digitalWrite(LEDG, HIGH);
    } else {
        // if (mBoxSettings->hasSampleRecordingFinished()) {
        if (InternalComm.sampleRecordFlush) {
            // mBoxSettings->resetSampleRecordingFinished();
            InternalComm.sampleRecordFlush = false;
            flush();
        } else {
            rtos::ThisThread::sleep_for(200);
        }
    }
}

void UnitWiFi::loopReportToBroker(WiFiClient &client) {
    uint8_t deviceID = BoxSettings.getDeviceID();
    char deviceName[18];
    int deviceNameLength = sprintf(deviceName, "lernfabrik/box_%d", deviceID);

    while (!mqttClient->connected()) {
        Serial.println("Connecting MQTT...");
        mqttClient->setServer(IPAddress(BoxSettings.getBrokerAddress()), BoxSettings.getBrokerPort());
        mqttClient->connect(deviceName);
    }

    uint8_t settingsLastPrediction = InternalComm.lastPrediction; // mBoxSettings->getLastPrediction();

    if (settingsLastPrediction != lastPublishedPrediction) {
        lastPublishedPrediction = settingsLastPrediction;

        //String buffer = String(lastPublishedPrediction);
        char buffer[5];
        int arg = lastPublishedPrediction;
        int n = sprintf(buffer, "%d", arg);

        const uint8_t *messageBuffer = reinterpret_cast<const uint8_t*>(buffer);

        char topic[deviceNameLength + 9];
        sprintf(topic, "%.*s/movement", deviceNameLength, deviceName);

        Serial.println("Publishing over MQTT: " + String(lastPublishedPrediction));
        mqttClient->publish(topic, messageBuffer, n, true);
    } else {
        mqttClient->loop();
        rtos::ThisThread::sleep_for(200);
    }
}

void UnitWiFi::sendBuffer(WiFiClient &client) {
    uint8_t numberOfSensors = 11;//7;
    int headerLength = 6 + numberOfSensors * BYTES_PER_SENSOR;

    uint8_t deviceID = BoxSettings.getDeviceID();
    uint8_t freqMoveType = (BoxSettings.getFrequencyLUTKey() & 0xF) << 4 | (BoxSettings.getModeDependendMovementTypeLUTKey() & 0xF);

    char header[headerLength] = {
        0x02, deviceID, (packetCounter & 0xFF), (packetCounter & 0xFF00) >> 8, freqMoveType, numberOfSensors, // version, device id, packet id, freq/movement-type!!!, # sensors
        TYPE_UINT64, 't', 'i', 'm', 'e', ' ', // uint64_t, "time "
        TYPE_FLOAT,  'a', 'c', 'c', 'x', ' ', // float32,  "accx "
        TYPE_FLOAT,  'a', 'c', 'c', 'y', ' ', // float32,  "accy "
        TYPE_FLOAT,  'a', 'c', 'c', 'z', ' ', // float32,  "accz "
        TYPE_FLOAT,  'g', 'y', 'r', 'o', 'x', // float32,  "gyrox"
        TYPE_FLOAT,  'g', 'y', 'r', 'o', 'y', // float32,  "gyroy"
        TYPE_FLOAT,  'g', 'y', 'r', 'o', 'z', // float32,  "gyroz"
        TYPE_FLOAT,  'q', 'u', 'a', 't', 'w', // float32,  "quatw"
        TYPE_FLOAT,  'q', 'u', 'a', 't', 'x', // float32,  "quatx"
        TYPE_FLOAT,  'q', 'u', 'a', 't', 'y', // float32,  "quaty"
        TYPE_FLOAT,  'q', 'u', 'a', 't', 'z'  // float32,  "quatz"
    };

    int numRows = MIN(crcBuffer->size(), MIN_ROWS_PER_PACKET);

    uint32_t ipAddress = BoxSettings.getDataServerAddress();

    client.println("POST /send HTTP/1.1");
    client.print("Host: "); //client.print("192.168.4.1"); 
            // optimize this?
            client.print(ipAddress & 0xFF); client.print("."); 
            client.print((ipAddress & 0xFF00) >> 8); client.print(".");
            client.print((ipAddress & 0xFF0000) >> 16); client.print(".");
            client.print(ipAddress >> 24);
            client.print(":"); client.println(BoxSettings.getDataServerPort());
    client.println("Accept: */*");
    client.println("Connection: close");
    client.println("Content-Length: " + String(headerLength + numRows * sizeof(Row)));
    client.println("Content-Type: application/octet-stream");
    client.println();

    client.write(header, headerLength);

    for (int i = 0; i < numRows; i++) {
        crcBuffer->pop(readRow);
        //Serial.println(readRow.timestamp);
        char *buf = (char*) &readRow.timestamp;    client.write(buf, sizeof(readRow.timestamp)); // static_cast<char*>(static_cast<void*>(&readRow.timestamp))
        buf =       (char*) &readRow.acc_x;        client.write(buf, sizeof(readRow.acc_x));
        buf =       (char*) &readRow.acc_y;        client.write(buf, sizeof(readRow.acc_y));
        buf =       (char*) &readRow.acc_z;        client.write(buf, sizeof(readRow.acc_z));
        //buf =       (char*) &readRow.realacc_x;    client.write(buf, sizeof(readRow.realacc_x));
        //buf =       (char*) &readRow.realacc_y;    client.write(buf, sizeof(readRow.realacc_y));
        //buf =       (char*) &readRow.realacc_z;    client.write(buf, sizeof(readRow.realacc_z));
        buf =       (char*) &readRow.gyro_x;       client.write(buf, sizeof(readRow.gyro_x));
        buf =       (char*) &readRow.gyro_y;       client.write(buf, sizeof(readRow.gyro_y));
        buf =       (char*) &readRow.gyro_z;       client.write(buf, sizeof(readRow.gyro_z));
        //buf =       (char*) &readRow.temperature;  client.write(buf, sizeof(readRow.temperature));
        //buf =       (char*) &readRow.distance;     client.write(buf, sizeof(readRow.distance));
        buf =       (char*) &readRow.quaternion_w; client.write(buf, sizeof(readRow.quaternion_w));
        buf =       (char*) &readRow.quaternion_x; client.write(buf, sizeof(readRow.quaternion_x));
        buf =       (char*) &readRow.quaternion_y; client.write(buf, sizeof(readRow.quaternion_y));
        buf =       (char*) &readRow.quaternion_z; client.write(buf, sizeof(readRow.quaternion_z));
    }
    client.println();

    // increase packet counter to assign each packet its own id
    packetCounter++;
}

void UnitWiFi::displayConnectError() {
    // Serial.println("Connect error!");
    // for (int i = 0; i < 5; i++) {
    //     digitalWrite(MYLED, LOW);
    //     rtos::ThisThread::sleep_for(30);
    //     digitalWrite(MYLED, HIGH);
    //     rtos::ThisThread::sleep_for(70);
    // }

    // TODO send message over RPC to display on screen!
    RPC1.println("POST wifi/status error");
}

void UnitWiFi::printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");

    RPC1.println("POST wifi/signalstrength " + String(rssi));
}
