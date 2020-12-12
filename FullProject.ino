#include <Arduino.h>

#include "Accelerometer.h"
#include "Ultrasonic.h"
#include "Circbuffer.h"

void setup() {
    Serial.begin(115200);
    
    accelerometer::Initialize();
    ultrasonic::Initialize();
}

void loop() {
    //accelerometer::Loop();
    ultrasonic::Loop();
    Serial.println();

    delay(500);
}