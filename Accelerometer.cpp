// header file
#include "Accelerometer.h"

// required dependencies
#include <Arduino.h>
#include <stdint.h>
#include <I2Cdev.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

#define acc_range 2 // 2g
#define acc_scale (0x4000 / acc_range)

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

    void ConfigureOffsets() {
        mpu.setXGyroOffset(154);
        mpu.setYGyroOffset(74);
        mpu.setZGyroOffset(-75);
        mpu.setXAccelOffset(1330);
        mpu.setYAccelOffset(-1565);
        mpu.setZAccelOffset(1588);
    }
} // namespace

void Initialize() {
    // Serial.println("Accelerometer -- Setup");

    Wire.begin();
    Wire.setClock(400000);

    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    ConfigureOffsets();

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

void Loop() {
    // Serial.println("Accelerometer -- Loop");

    if (!dmpReady) return;

    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        mpu.dmpGetAccel(&acceleration, fifoBuffer);
        mpu.dmpGetLinearAccel(&realAcceleration, &acceleration, &gravity);
        mpu.dmpGetLinearAccelInWorld(&realAccelerationInWorld, &realAcceleration, &q);

        Serial.print("acc\t");
        Serial.print((float) realAccelerationInWorld.x * 9.81 / acc_scale);
        Serial.print("\t");
        Serial.print((float) realAccelerationInWorld.y * 9.81 / acc_scale);
        Serial.print("\t");
        Serial.print((float) realAccelerationInWorld.z * 9.81 / acc_scale);

        Serial.print("\typr\t");
        Serial.print(ypr[0] * 180/M_PI);
        Serial.print("\t");
        Serial.print(ypr[1] * 180/M_PI);
        Serial.print("\t");
        Serial.print(ypr[2] * 180/M_PI);

        temperature = mpu.getTemperature() / 340.0 + 36.53;
        Serial.print("\ttemp\t");
        Serial.print(temperature);
    }
}

void ReadValues(float *temp) {
    if (dmpReady && mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        mpu.dmpGetAccel(&acceleration, fifoBuffer);
        mpu.dmpGetLinearAccel(&realAcceleration, &acceleration, &gravity);
        mpu.dmpGetLinearAccelInWorld(&realAccelerationInWorld, &realAcceleration, &q);
        temperature = mpu.getTemperature() / 340.0 + 36.53;

        
    }
}

bool RunningOK() {
    return dmpReady;
}

} // namespace accelerometer