#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <Arduino.h>

class BoxSettingsClass {

public:
    BoxSettingsClass();

    uint32_t getSampleLength() const { return _sampleLength; }
    uint8_t getFrequencyLUTKey() const { return _frequencyKey; }
    uint8_t getMovementTypeLUTKey() const { return _movementTypeKey; }

    uint8_t getModeDependendMovementTypeLUTKey();
    void processRPCCommand(String command, String subject, String payload);

    void setSampleLength(uint32_t ms) { _sampleLength = ms; }
    void setFrequencyLUTKey(uint8_t frequencyKey) { _frequencyKey = frequencyKey; }
    void setMovementTypeLUTKey(uint8_t typeKey) { _movementTypeKey = typeKey; };

private:
    // -----------------------------------------------------
    // (to be) persistent settings
    // just for now, they should really only be stored on the flash
    uint32_t _sampleLength;
    uint8_t _frequencyKey;
    
    // -----------------------------------------------------
    // (to be) maybe persistent settings
    uint8_t _movementTypeKey;
};

extern BoxSettingsClass BoxSettings;

#endif // SETTINGS_H_
