// header file
#include "Accelerometer.h"

// required dependencies
#include <Arduino.h>
#include <stdint.h>
#include <I2Cdev.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

#define acc_range 8 // unit: g
#define acc_scale (0x4000 / acc_range) * 9.81

#define gyro_range 500 // unit: deg / sec
#define gyro_scale (0x4000 / gyro_range)

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
	mpu.setXGyroOffset(163);
	mpu.setYGyroOffset(80);
	mpu.setZGyroOffset(-92);

	mpu.setXAccelOffset(1300);
	mpu.setYAccelOffset(-1587);
	mpu.setZAccelOffset(1606);
    //mpu.setXAccelOffset(-2192);
	//mpu.setYAccelOffset(2385);
	//mpu.setZAccelOffset(6474);
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

    switch (acc_range) {
    case  2: mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);  break;
    case  4: mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);  break;
    case  8: mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);  break;
    case 16: mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16); break;
    }

    switch (gyro_range) {
    case  250: mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);  break;
    case  500: mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_500);  break;
    case 1000: mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000); break;
    case 2000: mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000); break;
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

        temp[0] = (float) acceleration.x / acc_scale;
        temp[1] = (float) acceleration.y / acc_scale;
        temp[2] = (float) acceleration.z / acc_scale;
        // temp[0] = (float) realAcceleration.x / acc_scale;
        // temp[1] = (float) realAcceleration.y / acc_scale;
        // temp[2] = (float) realAcceleration.z / acc_scale;
        // temp[3] = (float) realAccelerationInWorld.x / acc_scale;
        // temp[4] = (float) realAccelerationInWorld.y / acc_scale;
        // temp[5] = (float) realAccelerationInWorld.z / acc_scale;
        temp[6] = (float) gyro.x / gyro_range;
        temp[7] = (float) gyro.y / gyro_range;
        temp[8] = (float) gyro.z / gyro_range;

        temp[10] = q.w;
        temp[11] = q.x;
        temp[12] = q.y;
        temp[13] = q.z;

        //temp[6] = ypr[0] * 180/M_PI;
        //temp[7] = ypr[1] * 180/M_PI;
        //temp[8] = ypr[2] * 180/M_PI;
    }
}

void accelerometer::calibrate() {
    mpu.PrintActiveOffsets();

    mpu.setDMPEnabled(false);
    mpu.CalibrateAccel(10);
    //mpu.CalibrateGyro();
    mpu.PrintActiveOffsets();
    mpu.setDMPEnabled(true);

    Serial.println(mpu.getFullScaleAccelRange());
    Serial.println(mpu.getFullScaleGyroRange());
    //Serial.println(mpu.dmpGetSampleFrequency());
    //Serial.println(mpu.dmpGetFIFORate());
}

bool accelerometer::runningOK() {
    return dmpReady;
}
