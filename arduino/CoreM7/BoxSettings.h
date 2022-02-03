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
    // cached internal stuff
    uint32_t getSampleLength()      const { return _sampleLength; }
    uint8_t getFrequencyLUTKey()    const { return _frequencyKey; }
    uint8_t getDeviceID()           const { return _deviceID; }
    uint32_t getDataServerAddress() const { return _dataServerAddress; }
    uint16_t getDataServerPort()    const { return _dataServerPort; }
    uint32_t getBrokerAddress()     const { return _brokerAddress; }
    uint16_t getBrokerPort()        const { return _brokerPort; }
    char* getWiFiSSID()                   { return _wifiSSID; }
    char* getWiFiPassphrase()             { return _wifiPassphrase; }
    uint8_t getMovementTypeLUTKey() const { return _movementTypeKey; }

    uint8_t getModeDependendMovementTypeLUTKey();
    void processRPCCommand(String command, String subject, String payload);

    void setSampleLength(uint32_t ms)               { hasChanges = hasChanges || _sampleLength != ms;                      _sampleLength = ms; }
    void setFrequencyLUTKey(uint8_t frequencyKey)   { hasChanges = hasChanges || _frequencyKey != frequencyKey;            _frequencyKey = frequencyKey; }
    void setDeviceID(uint8_t deviceID)              { hasChanges = hasChanges || _deviceID != deviceID;                    _deviceID = deviceID; }
    void setDataServerAddress(uint32_t address)     { hasChanges = hasChanges || _dataServerAddress != address;            _dataServerAddress = address; }
    void setDataServerPort(uint16_t port)           { hasChanges = hasChanges || _dataServerPort != port;                  _dataServerPort = port; }
    void setBrokerAddress(uint32_t address)         { hasChanges = hasChanges || _brokerAddress != address;                _brokerAddress = address; }
    void setBrokerPort(uint16_t port)               { hasChanges = hasChanges || _brokerPort != port;                      _brokerPort = port; }
    void setWiFiSSID(char *ssid)                    { hasChanges = hasChanges || strcmp(_wifiSSID, ssid) != 0;             strcpy(_wifiSSID, ssid); }
    void setWiFiPassphrase(char *passphrase)        { hasChanges = hasChanges || strcmp(_wifiPassphrase, passphrase) != 0; strcpy(_wifiPassphrase, passphrase); }
    void setMovementTypeLUTKey(uint8_t typeKey)     {                                                                      _movementTypeKey = typeKey; };

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
    uint8_t _deviceID;

    uint32_t _dataServerAddress;
    uint16_t _dataServerPort;

    uint32_t _brokerAddress;
    uint16_t _brokerPort;

    char _wifiSSID[32];
    char _wifiPassphrase[63];
    
    // -----------------------------------------------------
    // (to be) maybe persistent settings
    uint8_t _movementTypeKey;

    // -----------------------------------------------------
    // internal flash stuff
    struct SavedBoxSettings {
        uint32_t savedSampleLength;
        uint8_t savedFrequencyKey;
        uint8_t savedDeviceID;
        uint32_t savedDataServerAddress;
        uint16_t savedDataServerPort;
        uint32_t savedBrokerAddress;
        uint16_t savedBrokerPort;
        char savedWifiSSID[32];
        char savedWifiPassphrase[63];
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
