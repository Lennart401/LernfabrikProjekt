#include <Arduino.h>

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <Arduino.h>
#include <stdint.h>
#include <I2Cdev.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

class Accelerometer {

public:
    Accelerometer(uint8_t gRange);

    void initialize();
    void readValues(float *temp);

    bool runningOK();
    void getError(char *buffer) {};

private:
    void configureOffsets();

    MPU6050 mpu;
    float accScale;

    bool dmpReady = false;
    uint8_t devStatus;
    uint16_t fifoCount;
    uint8_t fifoBuffer[64];

    Quaternion q;
    VectorInt16 acceleration;
    VectorInt16 realAcceleration;
    VectorInt16 realAccelerationInWorld;
    VectorFloat gravity;
    float ypr[3];

    float temperature;

};


#endif // ACCELEROMETER_H_
