// header file
#include "Ultrasonic.h"

// using a macro to avoid function call overhead
// #define WAIT_FOR_PIN_STATE(state) \
//     while (digitalRead(pin) != (state)) { \
//         if (micros() - timestamp > timeout) { \
//             return 0; \
//         } \
//     }

// static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout) {
//     unsigned long timestamp = micros();
//     WAIT_FOR_PIN_STATE(!state);
//     WAIT_FOR_PIN_STATE(state);
//     timestamp = micros();
//     WAIT_FOR_PIN_STATE(!state);
//     return micros() - timestamp;
// }

Ultrasonic::Ultrasonic(int triggerPin_, int echoPin_)
    : triggerPin(triggerPin_)
    , echoPin(echoPin_) 
    , lastDuration(0)
    , autoTriggerCounter(0)
    , echoInterrupt(digitalPinToPinName(echoPin_)) {
}

void Ultrasonic::initialize() {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);

    echoInterrupt.rise(mbed::callback(this, &Ultrasonic::onRise));
    echoInterrupt.fall(mbed::callback(this, &Ultrasonic::onFall));
}

void Ultrasonic::trigger() {
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
}

void Ultrasonic::getLastValue(float* value) {
    *value = 0.017 * lastDuration;

    autoTriggerCounter++;
    if (autoTriggerCounter > 50) {
        autoTriggerCounter = 0;
        Serial.println("!!! --------------------- Last Distance: " + String(*value));
        trigger();
    }
}

void Ultrasonic::onRise() {
    begin = micros();
}

void Ultrasonic::onFall() {
    lastDuration = micros() - begin;
}
