#include "UnitWiFi.h"

#include <mbed.h>
#include <rtos.h>
#include <SPI.h>

// this file contains the wifi credentials for now, this will change
// as the project progresses. the idea is to save the credentials in the
// QSPI flash and then get them from there before connecting. they could
// been entered/changed through the touch screen user interface
#include "arduino_secrets.h"

#define MIN_ROWS_PER_PACKET 100
#define MYLED LEDB

UnitWiFi::UnitWiFi(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer)
    : crcBuffer(buffer)
    , dataServerHost(new IPAddress(192, 168, 4, 1))
    , dataServerPort(5000)
    , currentMode(IDLE) {
    Serial.println("Port: " + String(dataServerPort));
    Serial.println("SSID: " + String(SECRET_SSID));
    Serial.println("Pass: " + String(SECRET_PASS));
    Serial.println("Mode: " + String(currentMode));
}

UnitWiFi::~UnitWiFi() {
    delete dataServerHost;
}

void UnitWiFi::runWiFi(WiFiClient &client) {
    // Setup
    Serial.println("Setting up, connecting...");
    pinMode(MYLED, OUTPUT);
    digitalWrite(MYLED, HIGH);

    connectWiFi();
    //Serial.println("Dummy connected");

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
            //loopReportToBroker(client);
            Serial.println("--REPORTING TO BROKER........--");
            rtos::ThisThread::sleep_for(200);
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

void UnitWiFi::setMode(WiFiMode mode) {
    currentMode = mode;
}

void UnitWiFi::connectWiFi() {
    char ssid[] = SECRET_SSID;
    char pass[] = SECRET_PASS;

    Serial.println("Connecting to network " + String(SECRET_SSID) + "...");
    digitalWrite(MYLED, LOW);

    while (status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        //delay(1000); // 5000
    }
    digitalWrite(MYLED, HIGH);

    printWifiStatus();
}

void UnitWiFi::loopSendToDataServer(WiFiClient &client) {
    if (crcBuffer->size() > MIN_ROWS_PER_PACKET) {
        digitalWrite(LEDG, LOW);
        // pop all rows from the buffer and send them

        Serial.println("\nStarting connection to data server...");
        if (client.connect(*dataServerHost, dataServerPort)) {
            Serial.println("Sending data to data server");
            sendBuffer(client);

            // could also read from the server here, maybe implement that later
        } else {
            Serial.println("could not connect!");
            displayConnectError();
        }

        client.stop();
        digitalWrite(LEDG, HIGH);
        //rtos::ThisThread::sleep_for(1000);
    } else {
        rtos::ThisThread::sleep_for(200);
    }
}

void UnitWiFi::loopReportToBroker(WiFiClient &client) {

}

void UnitWiFi::sendBuffer(WiFiClient &client) {
    char header[] = {
        0x01, 0x02, 0x00, 0x09, // version, device id, num sensors
        0x03, 0x74, 0x69, 0x6d, 0x65, // uint64_t, "time"
        0x04, 0x61, 0x63, 0x63, 0x78, // float32, "accx"
        0x04, 0x61, 0x63, 0x63, 0x79, // float32, "accy"
        0x04, 0x61, 0x63, 0x63, 0x7a, // float32, "accz"
        0x04, 0x67, 0x79, 0x72, 0x78, // float32, "gyrx"
        0x04, 0x67, 0x79, 0x72, 0x79, // float32, "gyrx"
        0x04, 0x67, 0x79, 0x72, 0x7a, // float32, "gyrx"
        0x04, 0x74, 0x65, 0x6d, 0x70, // float32, "temp"
        0x04, 0x64, 0x69, 0x73, 0x74  // float32, "dist"
    };
    int headerLength = 4 + 9*5;

    client.println("POST /send HTTP/1.1");
    client.print("Host: "); client.print("192.168.100.22"); client.print(":"); client.println(dataServerPort);
    client.println("Accept: */*");
    client.println("Connection: close");
    client.println("Content-Length: " + String(headerLength + MIN_ROWS_PER_PACKET * sizeof(Row)));
    client.println("Content-Type: application/octet-stream");
    client.println();

    client.write(header, headerLength);

    for (int i = 0; i < MIN_ROWS_PER_PACKET; i++) {
        crcBuffer->pop(readRow);
        char *buf = (char*) &readRow.timestamp;   client.write(buf, sizeof(readRow.timestamp)); // static_cast<char*>(static_cast<void*>(&readRow.timestamp))
        buf =       (char*) &readRow.acc_x;       client.write(buf, sizeof(readRow.acc_x));
        buf =       (char*) &readRow.acc_y;       client.write(buf, sizeof(readRow.acc_y));
        buf =       (char*) &readRow.acc_z;       client.write(buf, sizeof(readRow.acc_z));
        buf =       (char*) &readRow.gyro_x;      client.write(buf, sizeof(readRow.gyro_x));
        buf =       (char*) &readRow.gyro_y;      client.write(buf, sizeof(readRow.gyro_y));
        buf =       (char*) &readRow.gyro_z;      client.write(buf, sizeof(readRow.gyro_z));
        buf =       (char*) &readRow.temperature; client.write(buf, sizeof(readRow.temperature));
        buf =       (char*) &readRow.distance;    client.write(buf, sizeof(readRow.distance));
    }
    client.println();
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
}
