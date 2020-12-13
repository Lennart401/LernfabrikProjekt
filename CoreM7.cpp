#include <Arduino.h>
#include <RPC_internal.h>
#include "Circbuffer.h"

namespace core_m7 {

// we will use a circular buffer here
// feed the input side from the RPC input and read values from the output of the buffer
// to be preprocessed put into the (first) model as input (tensor)

void Setup() {
    // this needs an extra if guard otherwise M4 will not compile
    #ifdef CORE_CM7
    bootM4();
    #endif
}

void Loop() {
    while (RPC1.available()) {
        Serial.write(RPC1.read());
    }

    //Serial.println("CoreM7");
    //delay(500);
}

} // namespace core_m7

