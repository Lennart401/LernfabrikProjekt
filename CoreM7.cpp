#ifdef CORE_CM7

#include <Arduino.h>
#include <SDRAM.h>
#include <mbed.h>
#include <rtos.h>
#include "Row.h"
#include "MbedCircularBuffer.h"
#include "UnitSensors.h"

namespace core_m7 {

// we will use a circular buffer here
// feed the input side from the RPC input and read values from the output of the buffer
// to be preprocessed put into the (first) model as input (tensor)

namespace {
    Row* buffer_space;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;

    rtos::Thread unitSensorsThread(osPriorityRealtime);
}

void setup() {
    SDRAM.begin();
    buffer_space = (Row*) SDRAM.malloc(sizeof(Row) * BUF_ROWS);
    crcBuffer = new mbed::MbedCircularBuffer<Row, BUF_ROWS>(*buffer_space);

    unitSensorsThread.start(mbed::callback(units::runSensors, crcBuffer));
}

void loop() {
    Serial.println("crcBuffer size: " + String(crcBuffer->size()));
    rtos::ThisThread::sleep_for((uint32_t) 500);
}

} // namespace core_m7

#endif // CORE_CM7