#include <Arduino.h>

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

namespace ultrasonic {

void Initialize();
void ReadValue(float* value);

} // namespace ultrasonic

static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout = 1000000L);

#endif // ULTRASONIC_H_