#ifndef _UNIT_SENSORS_H
#define _UNIT_SENSORS_H

#include <Arduino.h>
#include "Row.h"
#include "MbedCircularBuffer.h"

#define SENSORS_HZ 5 // would be 200 or 400 or whatever
#define UNITS_PER_SECOND 1000 // 1000000 for µs
#define FETCH_TIME (UNITS_PER_SECOND/SENSORS_HZ)

class UnitSensors {

public:
    void runSensors(mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer);
    void stopSensors();

private:
    float temp[8];
    uint64_t currentTime;
    uint64_t nextFetch;
    volatile bool running = true;
    Row insertRow;
    
};

#endif // _UNIT_SENSORS_H