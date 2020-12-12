#include <Arduino.h>

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

namespace accelerometer {

void Initialize();
void ReadValues(float *temp);

bool RunningOK();
void GetError(char *buffer);

} // namespace accelerometer


#endif // ACCELEROMETER_H_