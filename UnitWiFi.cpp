#include <mbed.h>
#include <rtos.h>
#include "UnitWiFi.h"

// this file contains the wifi credentials for now, this will change
// as the project progresses. the idea is to save the credentials in the
// QSPI flash and then get them from there before connecting. they could
// been entered/changed through the touch screen user interface
#include "arduino_secrets.h"

#define MIN_ROWS_PER_PACKET 30
#define MYLED LEDB

UnitWiFi::UnitWiFi(mbed::MbedCircularBuffer<Row, BUF_ROWS>* buffer) {
    this->crcBuffer = buffer;
    host = new IPAddress(192, 168, 178, 67);
}

UnitWiFi::~UnitWiFi() {
    delete host;
}

void UnitWiFi::connectWiFi() {
    char ssid[] = SECRET_SSID;
    char pass[] = SECRET_PASS;

    while (status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        
        for (int i = 0; i < 10; i++) {
            if (status == WL_CONNECTED) break;
            digitalWrite(MYLED, LOW);
            rtos::ThisThread::sleep_for(500);
            digitalWrite(MYLED, HIGH);
            rtos::ThisThread::sleep_for(1000);
        }
    }
}

void UnitWiFi::sendBuffer(WiFiClient &client) {
    client.println("GET /send HTTP/1.1");
    while (crcBuffer->pop(readRow));

    digitalWrite(MYLED, LOW);
    rtos::ThisThread::sleep_for(20);
    digitalWrite(MYLED, HIGH);
}

void UnitWiFi::displayConnectError() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(MYLED, LOW);
        rtos::ThisThread::sleep_for(30);
        digitalWrite(MYLED, HIGH);
        rtos::ThisThread::sleep_for(70);
    }
}

void UnitWiFi::runWiFi() {
    // Setup
    pinMode(MYLED, OUTPUT);
    connectWiFi();

    // Loop
    while (running) {
        if (crcBuffer->size() > MIN_ROWS_PER_PACKET) {
            // pop all rows from the buffer and send them
            WiFiClient client;

            if (!client.connect(*host, port)) {
                displayConnectError();
            }

            if (client.connected()) {
                sendBuffer(client);
            }

            client.stop();
        } else {
            rtos::ThisThread::sleep_for(250);
        }
    }
}

void UnitWiFi::stopWiFi() {
    running = false;
}