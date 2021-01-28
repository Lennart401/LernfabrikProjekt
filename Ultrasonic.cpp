// header file
#include "Ultrasonic.h"

// required dependencies
#include <Arduino.h>

#define TRIGGER_PIN 5
#define ECHO_PIN 4

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

namespace ultrasonic {

namespace {
    void sendPulse() {
        digitalWrite(TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGGER_PIN, LOW);
    }

    long duration;
    //double distance;
}

void initialize() {
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void readValue(float* value) {
    sendPulse();
    duration = newPulseIn(ECHO_PIN, HIGH);
    *value = 0.017 * duration;
}

}