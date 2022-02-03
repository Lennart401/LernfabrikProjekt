#ifndef _UNIT_WIFI_H
#define _UNIT_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "MbedCircularBuffer.h"
#include "Row.h"

class UnitWiFi {

public:
    enum WiFiMode {
        SEND_TO_DATASERVER,
        REPORT_TO_BROKER,
        IDLE
    };

    UnitWiFi(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer);
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

    // void receiveMQTTMessage(char *topic, uint8_t *payload, uint32_t length) {}

    volatile WiFiMode currentMode;

    int status = WL_IDLE_STATUS;
    PubSubClient *mqttClient = nullptr;
    uint8_t lastPublishedPrediction = 0;

    Row readRow;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;
    uint16_t packetCounter = 0;

    volatile bool running = true;
    volatile bool flushBuffer = false;

};

#endif // _UNIT_WIFI_H
