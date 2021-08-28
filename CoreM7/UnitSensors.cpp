#include "UnitSensors.h"

#include <mbed.h>
#include <rtos.h>
#include "Accelerometer.h"
#include "Ultrasonic.h"

//#define SENSORS_HZ 10 // would be 200 or 400 or whatever
#define UNITS_PER_SECOND 1000 // 1000000 for µs
#define FETCH_TIME(_hz) (UNITS_PER_SECOND/_hz)

UnitSensors::UnitSensors(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer, BoxSettings *boxSettings, uint32_t hz)
    : crcBuffer(buffer)
    , mHz(hz)
    , mBoxSettings(boxSettings)
    , mUltrasonic(4, 5)
    , currentMode(SensorsMode::IDLE) {
}

void UnitSensors::runSensors() {
    accelerometer::initialize();
    //mUltrasonic.initialize();

    while (running) {
        switch(currentMode) {

        case RECORDING:
            // TODO replace with std::chrono::time_point
            currentTime = rtos::Kernel::get_ms_count();
            nextFetch = currentTime + FETCH_TIME(mHz);

            accelerometer::readValues(temp);
            //mUltrasonic.readValue(&temp[7]);

            insertRow.timestamp = currentTime;
            insertRow.acc_x = temp[0];
            insertRow.acc_y = temp[1];
            insertRow.acc_z = temp[2];
            insertRow.realacc_x = temp[3];
            insertRow.realacc_y = temp[4];
            insertRow.realacc_z = temp[5];
            //insertRow.gyro_x = temp[6];
            //insertRow.gyro_y = temp[7];
            //insertRow.gyro_z = temp[8];
            //insertRow.temperature = temp[6];
            //insertRow.distance = temp[7];
            crcBuffer->push(insertRow);

            rtos::ThisThread::sleep_until(nextFetch);
            break;

        case IDLE:
            rtos::ThisThread::sleep_for(200);
            break;

        }

        
    }
}

void UnitSensors::stopSensors() {
    running = false;
}

void UnitSensors::calibrate() {
    accelerometer::calibrate();
}

void UnitSensors::setMode(SensorsMode mode) {
    if (currentMode == IDLE && mode == RECORDING) {
        setFrequencyLUTKey(mBoxSettings->getFrequencyLUTKey());
    }

    currentMode = mode;
}

void UnitSensors::setFrequencyLUTKey(uint8_t frequencyKey) {
    uint8_t baseKey = frequencyKey & 0x3; // 0x3 == 0011
    uint8_t multiplier = (frequencyKey & 0xC) >> 2; // 0xC == 1100
    int frequencyMapped = 0;

    // the lower two bits (base) define what the frequency is
    if (baseKey == 0x0) frequencyMapped = 1;
    else if (baseKey == 0x1) frequencyMapped = 2;
    else if (baseKey == 0x2) frequencyMapped = 4;
    else if (baseKey == 0x3) frequencyMapped = 5;

    // the upper two bits (multiplier) define what the mapped frequency has to be mutliplied with to get the true frequency
    // if (multiplier == 0x0) frequencyMapped *= 1;
    if (multiplier == 0x1) frequencyMapped *= 10;
    else if (multiplier == 0x2) frequencyMapped *= 100;
    else if (multiplier == 0x3) frequencyMapped *= 1000;

    mHz = frequencyMapped;
    Serial.println("Settings frequency to " + String(mHz));
}
