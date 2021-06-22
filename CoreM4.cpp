#ifdef CORE_CM4

#include "CoreM4.h"

#include <Arduino.h>
#include <RPC_internal.h>

namespace core_m4 {

void setup() {

}

void loop() {
    RPC1.println("SET running 1");
    delay(1000);
}

}

#endif // CORE_CM4