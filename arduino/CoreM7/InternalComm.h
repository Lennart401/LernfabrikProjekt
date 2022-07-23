#ifndef INTERNAL_COMM_CLASS_H_
#define INTERNAL_COMM_CLASS_H_

#include <Arduino.h>

class InternalCommClass {

public:
    InternalCommClass();

    // flag whether to use the movement types or just record unbounded
    bool useMovementTypes;
    // a value to communicate between UnitSensors and UnitWiFi
    // UnitSensors will set this value to true and UnitWiFi will reset it to false once it has processed it
    bool sampleRecordFlush;
    // the lastest prediction that UnitDataProcessing has made
    uint8_t lastPrediction;

    float lastDistance;

};

extern InternalCommClass InternalComm;

#endif // INTERNAL_COMM_CLASS_H_