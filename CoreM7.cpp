#ifdef CORE_CM7

#include <Arduino.h>
#include <RPC_internal.h>
#include <SDRAM.h>
//#include "Circbuffer.h"
#include "Row.h"
#include "MbedCircularBuffer.h"

namespace core_m7 {

#define BUF_ROWS 100

// we will use a circular buffer here
// feed the input side from the RPC input and read values from the output of the buffer
// to be preprocessed put into the (first) model as input (tensor)

namespace {
    Row workingRow;

    Row* buffer_space;
    mbed::MbedCircularBuffer<Row, BUF_ROWS>* crcBuffer;

    uint64_t counter = 0;

    void appendMeasurementM7(uint64_t timestamp, float ax, float ay, float az, float gx, float gy, float gz, float temp, float dist) {
        Row insertRow;
        counter++;
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

        insertRow.timestamp = (uint64_t) timestamp;
        insertRow.acc_x = (float) ax;
        insertRow.acc_y = (float) ay;
        insertRow.acc_z = (float) az;
        insertRow.gyro_x = (float) gx;
        insertRow.gyro_y = (float) gy;
        insertRow.gyro_z = (float) gz;
        insertRow.temperature = (float) temp;
        insertRow.distance = (float) dist;
        crcBuffer->push(insertRow);
        
        //circbuffer::WriteRow(&insertRow);

        //digitalWrite(LEDG, LOW);
        //delay(100);
        //digitalWrite(LEDG, HIGH);
    }
}

void Setup() {
    SDRAM.begin();
    buffer_space = (Row*) SDRAM.malloc(sizeof(Row) * BUF_ROWS);
    crcBuffer = new mbed::MbedCircularBuffer<Row, BUF_ROWS>(*buffer_space);

    bootM4();
    RPC1.bind("appendMeasurement", appendMeasurementM7);

    pinMode(LEDB, OUTPUT);
    pinMode(LEDG, OUTPUT);
}

void Loop() {
    // if (!circbuffer::IsBufferEmpty()) {
    //     workingRow = *circbuffer::ReadRow();

    //     //Serial.print(workingRow.timestamp);
    //     //Serial.print(",");
    //     Serial.print(workingRow.acc_x, 5);
    //     Serial.print(",");
    //     Serial.print(workingRow.acc_y, 5);
    //     Serial.print(",");
    //     Serial.print(workingRow.acc_z, 5);
    //     Serial.print(",");
    //     Serial.print(workingRow.gyro_x, 5);
    //     Serial.print(",");
    //     Serial.print(workingRow.gyro_y, 5);
    //     Serial.print(",");
    //     Serial.print(workingRow.gyro_z, 5);
    //     Serial.print(",");
    //     Serial.print(workingRow.temperature, 5);
    //     Serial.print(",");
    //     Serial.println(workingRow.distance, 5);
    // }

    Serial.print("buffer size: ");
    Serial.print(crcBuffer->size());
    Serial.print(" counter: ");
    Serial.print(counter);
    Serial.print(" size of variables: ");
    Serial.print(counter * (sizeof(float) * 8 + sizeof(uint64_t)));
    Serial.print(" size of Rows ");
    Serial.print(counter * sizeof(Row));
    Serial.print(" total size: ");
    Serial.println(counter * (sizeof(float) * 8 + sizeof(uint64_t) + sizeof(Row)));

    // while (RPC1.available()) {
    //     Serial.write(RPC1.read());
    // }
    
    digitalWrite(LEDB, LOW);
    delay(50);
    digitalWrite(LEDB, HIGH);
    delay(200);
}

} // namespace core_m7

#endif // CORE_CM7