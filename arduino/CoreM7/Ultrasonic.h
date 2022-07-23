#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <Arduino.h>
#include <mbed.h>

class Ultrasonic {

public:
    Ultrasonic(int triggerPin, int echoPin);

    void initialize();
    void trigger();
    void getLastValue(float *value);

private:
    void onRise();
    void onFall();

    long begin;
    long lastDuration;
    int autoTriggerCounter;
    
    const int triggerPin;
    const int echoPin;

    mbed::InterruptIn echoInterrupt;

};

//static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout = 1000000L);

#endif // ULTRASONIC_H_
