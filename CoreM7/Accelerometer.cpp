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

// internal stuff
static MPU6050 mpu;

static bool dmpReady = false;
static uint8_t devStatus;
static uint16_t fifoCount;
static uint8_t fifoBuffer[64];

static Quaternion q;
static VectorInt16 acceleration;
static VectorInt16 realAcceleration;
static VectorInt16 realAccelerationInWorld;
static VectorFloat gravity;
static VectorInt16 gyro;
static float ypr[3];

static float temperature;

static void configureOffsets() {
	mpu.setXGyroOffset(154);
	mpu.setYGyroOffset(74);
	mpu.setZGyroOffset(-75);
	mpu.setXAccelOffset(1330);
	mpu.setYAccelOffset(-1565);
	mpu.setZAccelOffset(1588);
}

// implementations from header
void accelerometer::initialize() {
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
    Serial.println(mpu.getFullScaleGyroRange());
}

void accelerometer::readValues(float *temp) {
    if (dmpReady && mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        //mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        mpu.dmpGetAccel(&acceleration, fifoBuffer);
        mpu.dmpGetLinearAccel(&realAcceleration, &acceleration, &gravity);
        mpu.dmpGetLinearAccelInWorld(&realAccelerationInWorld, &realAcceleration, &q);
        mpu.dmpGetGyro(&gyro);
        temperature = mpu.getTemperature() / 340.0 + 36.53;

        temp[0] = (float) realAcceleration.x / acc_scale;
        temp[1] = (float) realAcceleration.y / acc_scale;
        temp[2] = (float) realAcceleration.z / acc_scale;
        temp[3] = (float) realAccelerationInWorld.x / acc_scale;
        temp[4] = (float) realAccelerationInWorld.y / acc_scale;
        temp[5] = (float) realAccelerationInWorld.z / acc_scale;
        temp[6] = (float) gyro.x;
        temp[7] = (float) gyro.y;
        temp[8] = (float) gyro.z;

        //temp[6] = ypr[0] * 180/M_PI;
        //temp[7] = ypr[1] * 180/M_PI;
        //temp[8] = ypr[2] * 180/M_PI;
    }
}

void accelerometer::calibrate() {
    mpu.CalibrateAccel();
    mpu.CalibrateGyro();
    mpu.PrintActiveOffsets();
}

bool accelerometer::runningOK() {
    return dmpReady;
}
