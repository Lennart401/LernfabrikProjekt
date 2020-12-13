#include <Arduino.h>

#ifndef BUFFER_H_
#define BUFFER_H_

namespace circbuffer {

struct Row {
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float temperature;
    float distance;
};

bool IsBufferFull();

Row* ReadRow();
void WriteRow(Row* row);

} // namespace circbuffer

#endif