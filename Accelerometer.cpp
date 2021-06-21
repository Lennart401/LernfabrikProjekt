// header file
#include "Accelerometer.h"

Accelerometer::Accelerometer(uint8_t gRange)
    : accScale((0x4000 / gRange) * 9.81) {
}

void Accelerometer::initialize() {
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

void Accelerometer::readValues(float *temp) {
    if (dmpReady && mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        mpu.dmpGetAccel(&acceleration, fifoBuffer);
        mpu.dmpGetLinearAccel(&realAcceleration, &acceleration, &gravity);
        mpu.dmpGetLinearAccelInWorld(&realAccelerationInWorld, &realAcceleration, &q);
        temperature = mpu.getTemperature() / 340.0 + 36.53;

        temp[0] = (float) realAccelerationInWorld.x / accScale;
        temp[2] = (float) realAccelerationInWorld.z / accScale;
        temp[1] = (float) realAccelerationInWorld.y / accScale;
        temp[3] = ypr[0] * 180/M_PI;
        temp[4] = ypr[1] * 180/M_PI;
        temp[5] = ypr[2] * 180/M_PI;
        temp[6] = temperature;
    }
}

bool Accelerometer::runningOK() {
    return dmpReady;
}

void Accelerometer::configureOffsets() {
    mpu.setXGyroOffset(154);
    mpu.setYGyroOffset(74);
    mpu.setZGyroOffset(-75);
    mpu.setXAccelOffset(1330);
    mpu.setYAccelOffset(-1565);
    mpu.setZAccelOffset(1588);
}
