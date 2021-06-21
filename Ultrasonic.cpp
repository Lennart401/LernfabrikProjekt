// header file
#include "Ultrasonic.h"

// using a macro to avoid function call overhead
#define WAIT_FOR_PIN_STATE(state) \
    while (digitalRead(pin) != (state)) { \
        if (micros() - timestamp > timeout) { \
            return 0; \
        } \
    }

static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout) {
    unsigned long timestamp = micros();
    WAIT_FOR_PIN_STATE(!state);
    WAIT_FOR_PIN_STATE(state);
    timestamp = micros();
    WAIT_FOR_PIN_STATE(!state);
    return micros() - timestamp;
}

Ultrasonic::Ultrasonic(int triggerPin, int echoPin)
    : mTriggerPin(triggerPin)
    , mEchoPin(echoPin) {
}

void Ultrasonic::initialize() {
    pinMode(mTriggerPin, OUTPUT);
    pinMode(mEchoPin, INPUT);
}

void Ultrasonic::readValue(float* value) {
    sendPulse();
    duration = newPulseIn(mEchoPin, HIGH);
    *value = 0.017 * duration;
}

void Ultrasonic::sendPulse() {
    digitalWrite(mTriggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(mTriggerPin, LOW);
}
