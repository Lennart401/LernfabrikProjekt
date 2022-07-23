#include "InternalComm.h"

InternalCommClass::InternalCommClass() 
    : useMovementTypes(false)
    , sampleRecordFlush(false) 
    , lastPrediction(0)
    , lastDistance(0.0) {
}

InternalCommClass InternalComm;