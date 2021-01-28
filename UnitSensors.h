#ifndef _UNIT_SENSORS_H
#define _UNIT_SENSORS_H

#include <Arduino.h>
#include <mbed.h>
#include <rtos.h>
#include "Accelerometer.h"
#include "Ultrasonic.h"
#include "Row.h"
#include "MbedCircularBuffer.h"

namespace units {

#define SENSORS_HZ 5 // would be 200 or 400 or whatever
#define UNITS_PER_SECOND 1000 // 1000000 for µs
#define FETCH_TIME (UNITS_PER_SECOND/SENSORS_HZ)

namespace {
    float temp[8];
    uint64_t currentTime;
    uint64_t nextFetch;
    volatile bool running = true;
    Row insertRow;
}

void runSensors(mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer) {
    accelerometer::initialize();
    ultrasonic::initialize();

    while (running) {
        // TODO replace with std::chrono::time_point
        currentTime = rtos::Kernel::get_ms_count();
        nextFetch = currentTime + FETCH_TIME;

        accelerometer::readValues(temp);
        ultrasonic::readValue(&temp[7]);

        insertRow.timestamp = currentTime;
        insertRow.acc_x = temp[0];
        insertRow.acc_y = temp[1];
        insertRow.acc_z = temp[2];
        insertRow.gyro_x = temp[3];
        insertRow.gyro_y = temp[4];
        insertRow.gyro_z = temp[5];
        insertRow.temperature = temp[6];
        insertRow.distance = temp[7];
        crcBuffer->push(insertRow);

        rtos::ThisThread::sleep_until(nextFetch);
    }
}

void stopSensors() {
    running = false;
}

}

#endif // _UNIT_SENSORS_H