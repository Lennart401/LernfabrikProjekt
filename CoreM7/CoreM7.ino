#include <Arduino.h>
#include <SDRAM.h>
#include <mbed.h>
#include <rtos.h>
#include <RPC_internal.h>
#include "Row.h"
#include "MbedCircularBuffer.h"
#include "UnitSensors.h"
#include "UnitWiFi.h"

// wifi client workaround -- it will only work when created as a global variable
WiFiClient globClient;

// we will use a circular buffer here
// feed the input side from the RPC input and read values from the output of the buffer
// to be preprocessed put into the (first) model as input (tensor)

static Row *buffer_space;
static mbed::MbedCircularBuffer<Row, BUF_ROWS> *crcBuffer;

static UnitWiFi *unitWiFi = nullptr;
static UnitSensors *unitSensors = nullptr;

static rtos::Thread unitSensorsThread(osPriorityRealtime, OS_STACK_SIZE, nullptr, "sensors-thread");
static rtos::Thread unitWiFiThread(osPriorityNormal, OS_STACK_SIZE, nullptr, "wifi-thread");
static rtos::Thread m7RPCReceiverThread(osPriorityNormal, OS_STACK_SIZE, nullptr, "rpc-recv-thread");

static void runUnitSensors() {
    //rtos::ThisThread::sleep_for((uint32_t) 15000);
    //Serial.println("Starting sensors...");
    //UnitSensors sensors(crcBuffer);
    //sensors.runSensors();
    unitSensors = new UnitSensors(crcBuffer);
    unitSensors->runSensors();
    delete unitSensors;
}

static void runUnitWiFi() {
    //UnitWiFi wifi(crcBuffer);
    //wifi.runWiFi(globClient);
    unitWiFi = new UnitWiFi(crcBuffer);
    unitWiFi->runWiFi(globClient);
    delete unitWiFi;
}

static void runM7RPCReceiver() {
    String bufferString;

    while (true) {
        while (RPC1.available()) {
            char currentChar = RPC1.read();
            if (currentChar != '\n') bufferString += currentChar;
            else {
                Serial.println("RPC message: '" + bufferString + "'"); 

                int firstDelimiter = bufferString.indexOf(' ');
                String command = bufferString.substring(0, firstDelimiter);
                String subject;
                String payload;

                if (command == "GET" || command == "DO") {
                    subject = bufferString.substring(firstDelimiter+1, bufferString.length() - 1);
                } else if (command == "SET") {
                    int secondDelimiter = bufferString.indexOf(' ', firstDelimiter + 1);
                    subject = bufferString.substring(firstDelimiter+1, secondDelimiter);
                    payload = bufferString.substring(secondDelimiter+1, bufferString.length() - 1);
                }

                Serial.println("\nDecoded command: ");
                Serial.println("Command: '" + command + "'");
                Serial.println("Subject: '" + subject + "'");
                if (payload.length() > 0) Serial.println("Payload: '" + payload + "'\n");
                else Serial.println("No payload\n");

                if (command == "SET") {
                    if (subject == "mode/running") {
                        if (payload == "1") {
                            Serial.println("Enabling data record and send mode");
                            //if (unitWiFi) unitWiFi->setMode(UnitWiFi::WiFiMode::SEND_TO_DATASERVER);
                            if (unitSensors) unitSensors->setMode(UnitSensors::SensorsMode::RECORDING);
                        } else if (payload == "0") {
                            Serial.println("Disabling data record and send mode");
                            //if (unitWifi) unitWiFi->setMode(UnitWiFi::WiFiMode::IDLE);
                            if (unitWiFi) unitWiFi->flush();                            
                            if (unitSensors) unitSensors->setMode(UnitSensors::SensorsMode::IDLE);
                        }
                    }
                }

                if (command == "DO") {
                    if (subject == "sensors/calibrate") {
                        if (unitSensors) unitSensors->calibrate();
                    }
                }

                bufferString = "";
            }
        }
        rtos::ThisThread::sleep_for((uint32_t) 50);
    }
}

void setup() {
    // Open RPC connection and boot M4 core
    RPC1.begin();
    bootM4();

    // Open Serial connection
    Serial.begin(115200);
    //while (!Serial);

    SDRAM.begin();
    buffer_space = (Row*) SDRAM.malloc(sizeof(Row) * BUF_ROWS);
    crcBuffer = new mbed::MbedCircularBuffer<Row, BUF_ROWS>(*buffer_space);

    unitWiFiThread.start(runUnitWiFi);
    m7RPCReceiverThread.start(runM7RPCReceiver);
    unitSensorsThread.start(runUnitSensors);
}

void loop() {
    Serial.println("crcBuffer size: " + String(crcBuffer->size()));
    rtos::ThisThread::sleep_for((uint32_t) 1000);
}

