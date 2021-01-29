#ifndef _UNIT_WIFI_H
#define _UNIT_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include "MbedCircularBuffer.h"
#include "Row.h"

class UnitWiFi {

public:
    UnitWiFi(mbed::MbedCircularBuffer<Row, BUF_ROWS>* buffer);
    ~UnitWiFi();
    void runWiFi();
    void stopWiFi();

private:
    void connectWiFi();
    void sendBuffer(WiFiClient &client);
    void displayConnectError();

    IPAddress *host;
    uint16_t port = 5000;
    int status = WL_IDLE_STATUS;

    Row readRow;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;

    volatile bool running = true;

};

#endif // _UNIT_WIFI_H