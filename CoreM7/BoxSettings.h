#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <Arduino.h>
#include <QSPIFBlockDevice.h>
#include <MBRBlockDevice.h>
#include <FlashIAPBlockDevice.h>
#include <TDBStore.h>

// whether to use the internal flash or the external flash
// the internal flash has to share with the sketch, the external flash has to share with the WiFi firmware file 
// system, the ota file system and the programmed wifi firmware
//
// USE_INTERNAL_FLASH 0 --> place the TDBStore on partition 3 between 15 and 15.5 MB on the QSPI flash
// USE_INTERNAL_FLASH 1 --> place the TDBStore on the free space on the internal flash
//
// using USE_INTERNAL_FLASH 0 will currently crash the portenta as the blockDevice cannot init correctly
#define USE_INTERNAL_FLASH 1 

class BoxSettingsClass {

public:
    BoxSettingsClass();
    ~BoxSettingsClass();

    // -----------------------------------------------------
    // cached internal settings
    uint32_t getSampleLength() const { return _sampleLength; }
    uint8_t getFrequencyLUTKey() const { return _frequencyKey; }
    uint8_t getMovementTypeLUTKey() const { return _movementTypeKey; }

    uint8_t getModeDependendMovementTypeLUTKey();
    void processRPCCommand(String command, String subject, String payload);

    void setSampleLength(uint32_t ms) { _sampleLength = ms; hasChanges = true; }
    void setFrequencyLUTKey(uint8_t frequencyKey) { _frequencyKey = frequencyKey; hasChanges = true; }
    void setMovementTypeLUTKey(uint8_t typeKey) { _movementTypeKey = typeKey; };

    // -----------------------------------------------------
    // internal flash stuff
    void saveChanges();
    void setupFlashStorage();

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
    // internal flash stuff
    struct SavedBoxSettings {
        uint32_t savedSampleLength;
        uint8_t savedFrequencyKey;
    };

    bool hasChanges;
    bool storeOK;
    const char *tdbStoreKey;

#if (USE_INTERNAL_FLASH == 0)
    QSPIFBlockDevice *rootDevice;
    mbed::MBRBlockDevice *blockDevice;
#else
    FlashIAPBlockDevice *blockDevice;
#endif
    mbed::TDBStore *tdbStore;

    int getSavedBoxSettings(SavedBoxSettings *whereToLoad);
    int setSavedBoxSettings(SavedBoxSettings *whatToSave);
};

extern BoxSettingsClass BoxSettings;

#endif // SETTINGS_H_
