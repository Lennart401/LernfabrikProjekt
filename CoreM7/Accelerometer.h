#include <Arduino.h>

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

namespace accelerometer {

void initialize();
void readValues(float *temp);

void calibrate();

bool runningOK();
void getError(char *buffer);

} // namespace accelerometer


#endif // ACCELEROMETER_H_