#ifndef _UNIT_SENSORS_H
#define _UNIT_SENSORS_H

#include <Arduino.h>
#include "Row.h"
#include "MbedCircularBuffer.h"
#include "Accelerometer.h"
#include "Ultrasonic.h"

class UnitSensors {

public:
    enum SensorsMode {
        RECORDING,
        IDLE
    };

    UnitSensors(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer, uint32_t hz = 10);
    void runSensors();
    void stopSensors();
    void calibrate();

    void setMode(SensorsMode mode);

private:
    float temp[9];
    uint64_t currentTime;
    uint64_t nextFetch;

    volatile bool running = true;
    volatile SensorsMode currentMode;

    Row insertRow;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;
    uint32_t mHz;

    Ultrasonic mUltrasonic;

};

#endif // _UNIT_SENSORS_H
