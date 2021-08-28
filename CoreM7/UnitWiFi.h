#ifndef _UNIT_WIFI_H
#define _UNIT_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include "MbedCircularBuffer.h"
#include "Row.h"
#include "BoxSettings.h"

class UnitWiFi {

public:
    enum WiFiMode {
        SEND_TO_DATASERVER,
        REPORT_TO_BROKER,
        IDLE
    };

    UnitWiFi(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer, BoxSettings *boxSettings);
    ~UnitWiFi();
    void runWiFi(WiFiClient &client);
    void stopWiFi();

    void flush();

    void setMode(WiFiMode mode);

private:
    void connectWiFi();

    void loopSendToDataServer(WiFiClient &client);
    void loopReportToBroker(WiFiClient &client);

    void sendBuffer(WiFiClient &client);
    void displayConnectError();
    void printWifiStatus();

    volatile WiFiMode currentMode;

    IPAddress *dataServerHost;
    uint16_t dataServerPort;
    int status = WL_IDLE_STATUS;

    Row readRow;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;
    uint16_t packetCounter = 0;
    BoxSettings *mBoxSettings;

    volatile bool running = true;
    volatile bool flushBuffer = false;

};

#endif // _UNIT_WIFI_H
