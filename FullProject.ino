#include <Arduino.h>

#include "Accelerometer.h"
#include "Ultrasonic.h"
#include "Circbuffer.h"

#define FETCH_TIME (1000/200)

float temp[8];
unsigned long nextFetch;

circbuffer::Row* tempRow;

bool stop = false;

void setup() {
    Serial.begin(115200);
    
    accelerometer::Initialize();
    ultrasonic::Initialize();
}

void loop() {
    if (stop) {
        Serial.println("stopped");
        delay(1000);
        return;
    }

    // wait for next fetch
    while (micros() <= nextFetch);
    nextFetch = micros() + FETCH_TIME;

    accelerometer::ReadValues(temp);
    ultrasonic::ReadValue(&temp[7]);

    tempRow = (circbuffer::Row*) temp; 
    circbuffer::WriteRow(tempRow);

    if (circbuffer::IsBufferFull()) {
        Serial.println("acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, temp, dist");

        while (circbuffer::RowAvailable()) {
            tempRow = circbuffer::ReadRow();
            Serial.print(tempRow->acc_x, 5);
            Serial.print(",");
            Serial.print(tempRow->acc_y, 5);
            Serial.print(",");
            Serial.print(tempRow->acc_z, 5);
            Serial.print(",");
            Serial.print(tempRow->gyro_x, 5);
            Serial.print(",");
            Serial.print(tempRow->gyro_y, 5);
            Serial.print(",");
            Serial.print(tempRow->gyro_z, 5);
            Serial.print(",");
            Serial.print(tempRow->temperature, 5);
            Serial.print(",");
            Serial.println(tempRow->distance, 5);
        }
        stop = true;
    }

    // delay(1000);
}