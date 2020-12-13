#include <Arduino.h>
#include <RPC_internal.h>
#include "Accelerometer.h"
#include "Ultrasonic.h"

#define HZ 1 // would be 200 or 400 or whatever
#define FETCH_TIME (1000/HZ)

namespace core_m4 {

namespace {
    float temp[8];
    unsigned long nextFetch;
}

void Setup() {
    accelerometer::Initialize();
    ultrasonic::Initialize();
}

void Loop() {
    // wait for next fetch
    while (micros() <= nextFetch);
    nextFetch = micros() + FETCH_TIME;

    // read values from accelerometer into temp[0-6] and from 
    // ultrasonic into temp[7], so we have 8 floats of information
    accelerometer::ReadValues(temp);
    ultrasonic::ReadValue(&temp[7]);

    // print values over rpc --> this needs work
    RPC1.print(temp[0], 5);
    RPC1.print(",");
    RPC1.print(temp[1], 5);
    RPC1.print(",");
    RPC1.print(temp[2], 5);
    RPC1.print(",");
    RPC1.print(temp[3], 5);
    RPC1.print(",");
    RPC1.print(temp[4], 5);
    RPC1.print(",");
    RPC1.print(temp[5], 5);
    RPC1.print(",");
    RPC1.print(temp[6], 5);
    RPC1.print(",");
    RPC1.println(temp[7], 5);
    
    delay(1000);
}

}