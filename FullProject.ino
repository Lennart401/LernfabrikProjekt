#include <Arduino.h>
#include <RPC_internal.h>

#include "CoreM7.h"
#include "CoreM4.h"

void setup() {
    RPC1.begin();

    #ifdef CORE_CM7
        core_m7::setup();
    #endif

    #ifdef CORE_CM4
        core_m4::Setup();
    #endif
}

void loop() {
    #ifdef CORE_CM7
        core_m7::loop();
    #endif

    #ifdef CORE_CM4
        core_m4::Loop();
    #endif
}
