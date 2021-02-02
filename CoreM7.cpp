#ifdef CORE_CM7

#include <Arduino.h>
#include <SDRAM.h>
#include <mbed.h>
#include <rtos.h>
#include "Row.h"
#include "MbedCircularBuffer.h"
#include "UnitSensors.h"
#include "UnitWiFi.h"

// wifi client workaround -- it will only work when created as a global variable
WiFiClient globClient;

namespace core_m7 {

// we will use a circular buffer here
// feed the input side from the RPC input and read values from the output of the buffer
// to be preprocessed put into the (first) model as input (tensor)

namespace {
    Row* buffer_space;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;

    rtos::Thread unitSensorsThread(osPriorityRealtime);
    rtos::Thread unitWiFiThread(osPriorityNormal);

    void runUnitSensors() {
        UnitSensors sensors(crcBuffer);
        sensors.runSensors();
    }

    void runUnitWiFi() {
        UnitWiFi wifi(crcBuffer);
        wifi.runWiFi(globClient);
    }
}

void setup() {
    SDRAM.begin();
    buffer_space = (Row*) SDRAM.malloc(sizeof(Row) * BUF_ROWS);
    crcBuffer = new mbed::MbedCircularBuffer<Row, BUF_ROWS>(*buffer_space);

    unitSensorsThread.start(runUnitSensors); // mbed::callback(runUnitSensors, crcBuffer)
    unitWiFiThread.start(runUnitWiFi);
}

void loop() {
    Serial.println("crcBuffer size: " + String(crcBuffer->size()));
    rtos::ThisThread::sleep_for((uint32_t) 1000);
}

} // namespace core_m7

#endif // CORE_CM7
