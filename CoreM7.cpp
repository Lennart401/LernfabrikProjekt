#include <Arduino.h>
#include <RPC_internal.h>
#include "Circbuffer.h"

namespace core_m7 {

// we will use a circular buffer here
// feed the input side from the RPC input and read values from the output of the buffer
// to be preprocessed put into the (first) model as input (tensor)

namespace {
    Row workingRow;
    Row insertRow;

    void appendMeasurementM7(float ax, float ay, float az, float gx, float gy, float gz, float temp, float dist) {
        //Serial.print(timestamp);
        //Serial.print(",");
        // Serial.print(ax, 5);
        // Serial.print(",");
        // Serial.print(ay, 5);
        // Serial.print(",");
        // Serial.print(az, 5);
        // Serial.print(",");
        // Serial.print(gx, 5);
        // Serial.print(",");
        // Serial.print(gy, 5);
        // Serial.print(",");
        // Serial.print(gz, 5);
        // Serial.print(",");
        // Serial.print(temp, 5);
        // Serial.print(",");
        // Serial.println(dist, 5);

        //insertRow.timestamp = (uint64_t) timestamp;
        insertRow.acc_x = (float) ax;
        insertRow.acc_y = (float) ay;
        insertRow.acc_z = (float) az;
        insertRow.gyro_x = (float) gx;
        insertRow.gyro_y = (float) gy;
        insertRow.gyro_z = (float) gz;
        insertRow.temperature = (float) temp;
        insertRow.distance = (float) dist;
        circbuffer::WriteRow(&insertRow);

        //digitalWrite(LEDG, LOW);
        //delay(100);
        //digitalWrite(LEDG, HIGH);
    }
}

void Setup() {
    // this needs an extra if guard otherwise M4 will not compile
    #ifdef CORE_CM7
    bootM4();
    RPC1.bind("appendMeasurement", appendMeasurementM7);
    #endif

    pinMode(LEDB, OUTPUT);
    pinMode(LEDG, OUTPUT);
}

void Loop() {
    if (!circbuffer::IsBufferEmpty()) {
        workingRow = *circbuffer::ReadRow();

        //Serial.print(workingRow.timestamp);
        //Serial.print(",");
        Serial.print(workingRow.acc_x, 5);
        Serial.print(",");
        Serial.print(workingRow.acc_y, 5);
        Serial.print(",");
        Serial.print(workingRow.acc_z, 5);
        Serial.print(",");
        Serial.print(workingRow.gyro_x, 5);
        Serial.print(",");
        Serial.print(workingRow.gyro_y, 5);
        Serial.print(",");
        Serial.print(workingRow.gyro_z, 5);
        Serial.print(",");
        Serial.print(workingRow.temperature, 5);
        Serial.print(",");
        Serial.println(workingRow.distance, 5);
    }
    
    digitalWrite(LEDB, LOW);
    delay(100);
    digitalWrite(LEDB, HIGH);
    delay(400);
}

} // namespace core_m7

