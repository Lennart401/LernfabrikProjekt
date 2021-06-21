#include <Arduino.h>

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <Arduino.h>

class Ultrasonic {

public:
    Ultrasonic(int triggerPin, int echoPin);

    void initialize();
    void readValues(float *value);

private:
    void sendPulse();

    long mDuration;
    const int mTriggerPin;
    const int mEchoPin;

};

static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout = 1000000L);

#endif // ULTRASONIC_H_
