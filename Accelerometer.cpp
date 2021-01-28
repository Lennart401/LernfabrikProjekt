// header file
#include "Accelerometer.h"

// required dependencies
#include <Arduino.h>
#include <stdint.h>
#include <I2Cdev.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

#define acc_range 2 // 2g
#define acc_scale (0x4000 / acc_range) * 9.81

namespace accelerometer {

namespace {
    MPU6050 mpu;

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

    void configureOffsets() {
        mpu.setXGyroOffset(154);
        mpu.setYGyroOffset(74);
        mpu.setZGyroOffset(-75);
        mpu.setXAccelOffset(1330);
        mpu.setYAccelOffset(-1565);
        mpu.setZAccelOffset(1588);
    }
} // namespace

void initialize() {
    // Serial.println("Accelerometer -- Setup");

    Wire.begin();
    Wire.setClock(400000);

    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    configureOffsets();

    if (devStatus == 0) {
        mpu.PrintActiveOffsets();
        mpu.setDMPEnabled(true);
        dmpReady = true;
    } else {
        Serial.print("Accelerometer -- MPU Init Error: ");
        Serial.println(devStatus);
        // TODO implement error
    }

    Serial.println(mpu.getFullScaleAccelRange());
}

void readValues(float *temp) {
    if (dmpReady && mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        mpu.dmpGetAccel(&acceleration, fifoBuffer);
        mpu.dmpGetLinearAccel(&realAcceleration, &acceleration, &gravity);
        mpu.dmpGetLinearAccelInWorld(&realAccelerationInWorld, &realAcceleration, &q);
        temperature = mpu.getTemperature() / 340.0 + 36.53;

        temp[0] = (float) realAccelerationInWorld.x / acc_scale;
        temp[1] = (float) realAccelerationInWorld.y / acc_scale;
        temp[2] = (float) realAccelerationInWorld.z / acc_scale;
        temp[3] = ypr[0] * 180/M_PI;
        temp[4] = ypr[1] * 180/M_PI;
        temp[5] = ypr[2] * 180/M_PI;
        temp[6] = temperature;
    }
}

bool runningOK() {
    return dmpReady;
}

} // namespace accelerometer