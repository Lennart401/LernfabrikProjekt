#include <Arduino.h>

#ifndef ROW_H_
#define ROW_H_

struct Row {
    uint64_t timestamp;
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float temperature;
    float distance;
};

#endif // ROW_H_