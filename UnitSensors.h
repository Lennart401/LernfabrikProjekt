#ifndef _UNIT_SENSORS_H
#define _UNIT_SENSORS_H

#include <Arduino.h>
#include "Row.h"
#include "MbedCircularBuffer.h"

class UnitSensors {

public:
    UnitSensors(mbed::MbedCircularBuffer<Row, BUF_ROWS>* buffer);
    void runSensors();
    void stopSensors();

private:
    float temp[8];
    uint64_t currentTime;
    uint64_t nextFetch;
    volatile bool running = true;
    Row insertRow;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;

};

#endif // _UNIT_SENSORS_H