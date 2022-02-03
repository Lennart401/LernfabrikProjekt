#include "InternalComm.h"

InternalCommClass::InternalCommClass() 
    : useMovementTypes(false)
    , sampleRecordFlush(false) 
    , lastPrediction(0) {
}

InternalCommClass InternalComm;