#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <Arduino.h>

class BoxSettings {

public:
    BoxSettings();

    uint32_t getSampleLength() const { return _sampleLength; }
    uint8_t getFrequencyLUTKey() const { return _frequencyKey; }
    uint8_t getMovementTypeLUTKey() const { return _movementTypeKey; }
    bool getUseMovementTypes() const { return _useMovementTypes; }
    bool hasSampleRecordingFinished() const { return _sampleRecordingFinished; }

    uint8_t getModeDependendMovementTypeLUTKey();
    void processRPCCommand(String command, String subject, String payload);

    void setSampleLength(uint32_t ms) { _sampleLength = ms; }
    void setFrequencyLUTKey(uint8_t frequencyKey) { _frequencyKey = frequencyKey; }
    void setMovementTypeLUTKey(uint8_t typeKey) { _movementTypeKey = typeKey; };
    void setUseMovementTypes(bool useMovementTypes) { _useMovementTypes = useMovementTypes; }
    void setSampleRecordingFinished() { _sampleRecordingFinished = true; }
    void resetSampleRecordingFinished() { _sampleRecordingFinished = false; }

private:
    // -----------------------------------------------------
    // (to be) persistent settings
    // just for now, they should really only be stored on the flash
    uint32_t _sampleLength;
    uint8_t _frequencyKey;
    
    // -----------------------------------------------------
    // (to be) maybe persistent settings
    uint8_t _movementTypeKey;

    // -----------------------------------------------------
    // non-persistent settings for internal communication
    bool _useMovementTypes;
    // a value to communicate between UnitSensors and UnitWiFi
    // UnitSensors will set this value to true and UnitWiFi will reset it to false once it has processed it
    bool _sampleRecordingFinished;

};

#endif // SETTINGS_H_
