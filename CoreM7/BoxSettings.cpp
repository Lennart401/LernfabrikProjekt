#include "BoxSettings.h"

#include <RPC_internal.h>
#include "InternalComm.h"

#if USE_INTERNAL_FLASH == 1 // defined in the header
    // Get limits of the In Application Program (IAP) flash, ie. the internal MCU flash.
    #include "FlashIAPLimits.h"
    auto iapLimits { getFlashIAPLimits() };
#endif 

// description of the portenta QSPI Flash filesystem
// -------------------------------------------------
// when updating the wifi firmware using the updater sketch, it will create two MBR partitions on the QSPI flash:
//
//      Partition 1:    FATFileSystem, the place where the wifi firmware is stored, at /wlan/4343WA1.BIN .
//                      The filesystem is one MB (1024 * 1024 bytes) large.
//
//      Partition 2:    FATFileSystem, the ota partition, used for updating the portenta over the air.
//                      The filesystem starts at 1 MB (1024 * 1024 bytes), is 14 MB (14 * 1024 * 1024 bytes) large
//                      and contains nothing by default.
//                      AFAIK, this will only ever be used when updating OTA, so you can overwrite it if you need that
//                      space for something else.
//
// if using USE_INTERNAL_FLASH 0, this program will create another partition
//
//      Partition 3:    TDBStore, used for saving the BoxSettings of this program.
//                      The partition is 8 kB (1024 * 8) large, however it could be up to 512 kB (1024 * 512 bytes) 
//                      large, which is not needed at this moment.

#define BLOCK_DEVICE_START_ADDR 15 * 1024 * 1024 // for now, use the 512 kb available between 15 MB and 15.5 MB
#define BLOCK_DEVICE_SIZE 1024 * 8 // 8 KB
#define PARTITION_ID 3 // one of the ones left (one and two are takes by wlan and ota)
#define PARTITION_TYPE 0x0B // FAT 32

BoxSettingsClass::BoxSettingsClass()
    : _sampleLength(1000)
    , _frequencyKey(0x08)
    , _deviceID(0x01)
    , _movementTypeKey(0x01)
    , _dataServerAddress(0x1664A8C0) // 192.168.100.22, stored [0] = 192 (C0), [1] = 128 (A8), [2] = 100 (64), [3] = 22 (16)
    , _dataServerPort(5000)
    , _brokerAddress(0x1664A8C0) // 192.168.100.22, stored [0] = 192 (C0), [1] = 128 (A8), [2] = 100 (64), [3] = 22 (16)
    , _brokerPort(1883)
    , hasChanges(false)
    , storeOK(false)
    , tdbStoreKey("lfbox") {
}

BoxSettingsClass::~BoxSettingsClass() {
#if (USE_INTERNAL_FLASH == 0)
    delete rootDevice;
#endif
    delete blockDevice;
    delete tdbStore;
}

// ---------------------------------------------------------
// cached internal settings
uint8_t BoxSettingsClass::getModeDependendMovementTypeLUTKey() {
    if (InternalComm.useMovementTypes) {
        return _movementTypeKey;
    } else {
        return 0x0;
    }
}

void BoxSettingsClass::processRPCCommand(String command, String subject, String payload) {
    // GET responses
    if (command == "GET") {
        if (subject == "settings/sample-length") {
            char buffer[38];
            sprintf(buffer, "POST settings/sample-length %u", _sampleLength);
            RPC1.println(buffer);
        } 
        else if (subject == "settings/frequency") {
            char buffer[26];
            sprintf(buffer, "POST settings/frequency %2u", _frequencyKey);
            RPC1.println(buffer);
        } 
        else if (subject == "settings/device-id") {
            char buffer[26];
            sprintf(buffer, "POST settings/device-id %2u", _deviceID);
            RPC1.println(buffer);
        }
        else if (subject == "settings/data-server-address") {
            char buffer[44];
            sprintf(buffer, "POST settings/data-server-address %u", _dataServerAddress);
            RPC1.println(buffer);
        }
        else if (subject == "settings/data-server-port") {
            char buffer[36];
            sprintf(buffer, "POST settings/data-server-port %u", _dataServerPort);
            RPC1.println(buffer);
        }
        else if (subject == "settings/broker-address") {
            char buffer[39];
            sprintf(buffer, "POST settings/broker-address %u", _brokerAddress);
            RPC1.println(buffer);
        }
        else if (subject == "settings/broker-port") {
            char buffer[31];
            sprintf(buffer, "POST settings/broker-port %u", _brokerPort);
            RPC1.println(buffer);
        }
        else if (subject == "samples/movement-type") {
            char buffer[29];
            sprintf(buffer, "POST samples/movement-type %2u", _movementTypeKey);
            RPC1.println(buffer);
        }
    }

    // SET processing
    else if (command == "SET") {
        if (subject == "settings/sample-length") {
            setSampleLength(payload.toInt());
        } 
        else if (subject == "settings/frequency") {
            setFrequencyLUTKey(static_cast<uint8_t>(payload.toInt() & 0xFF));
        } 
        else if (subject == "settings/device-id") {
            setDeviceID(static_cast<uint8_t>(payload.toInt() & 0xFF));
        }
        else if (subject == "settings/data-server-address") {
            setDataServerAddress(static_cast<uint32_t>(payload.toInt() & 0xFFFFFFFF));
        }
        else if (subject == "settings/data-server-port") {
            setDataServerPort(static_cast<uint16_t>(payload.toInt() & 0xFFFF));
        }
        else if (subject == "settings/broker-address") {
            setBrokerAddress(static_cast<uint32_t>(payload.toInt() & 0xFFFFFFFF));
        }
        else if (subject == "settings/broker-port") {
            setBrokerPort(static_cast<uint16_t>(payload.toInt() & 0xFFFF));
        }
        else if (subject == "samples/movement-type") {
            setMovementTypeLUTKey(static_cast<uint8_t>(payload.toInt() & 0xFF));
        }
    }

    // DO processing
    else if (command == "DO") {
        if (subject == "settings/save") {
            saveChanges();
        }
    }
}

// ---------------------------------------------------------
// internal flash stuff
void BoxSettingsClass::saveChanges() {
    if (hasChanges && blockDevice && storeOK) {
        // write changes to flash
        SavedBoxSettings newSettings;
        newSettings.savedSampleLength = _sampleLength;
        newSettings.savedFrequencyKey = _frequencyKey;
        newSettings.savedDeviceID = _deviceID;
        newSettings.savedDataServerAddress = _dataServerAddress;
        newSettings.savedDataServerPort = _dataServerPort;
        newSettings.savedBrokerAddress = _brokerAddress;
        newSettings.savedBrokerPort = _brokerPort;

        auto result = setSavedBoxSettings(&newSettings);
        if (result == MBED_SUCCESS) {
            Serial.println("Successfully wrote settings to flash!");
        } else {
            Serial.println("Could not write settings to flash: " + String(result));
        }
        
        hasChanges = false;
    }
}

void BoxSettingsClass::setupFlashStorage() {

// the portenta current when USE_INTERNAL_FLASH = 0 crashes because the blockDevice cannot init correctly
#if (USE_INTERNAL_FLASH == 0)
    rootDevice = new QSPIFBlockDevice(PD_11, PD_12, PF_7, PD_13, PF_10, PG_6, QSPIF_POLARITY_MODE_1, 40000000);
    blockDevice = new mbed::MBRBlockDevice(rootDevice, PARTITION_ID);

    auto blockDevResult = blockDevice->init();
    Serial.println("Block device result: " + String(blockDevResult));

    if (blockDevResult != 0 || blockDevice->size() != BLOCK_DEVICE_SIZE) {
        Serial.println("Partitioning block device...");
        blockDevice->deinit();
        // Allocate a FAT 32 partition
        auto partRes = mbed::MBRBlockDevice::partition(rootDevice, PARTITION_ID, PARTITION_TYPE, BLOCK_DEVICE_START_ADDR, BLOCK_DEVICE_SIZE);
        Serial.println("|   Created partition on block device with code: " + String(partRes));
        auto initRes = blockDevice->init();
        Serial.println("|   Initialized blockDevice with code: " + String(initRes));
    }

    const auto eraseBlockSize = blockDevice->get_erase_size();
    const auto programBlockSize = blockDevice->get_program_size();

    Serial.println("Block device size: " + String((unsigned int) blockDevice->size() / 1024) + " KB");  
    Serial.println("Readable block size: " + String((unsigned int) blockDevice->get_read_size())  + " bytes");
    Serial.println("Programmable block size: " + String((unsigned int) programBlockSize) + " bytes");
    Serial.println("Erasable block size: " + String((unsigned int) eraseBlockSize / 1024) + " KB");

    blockDevice->deinit();
#else
    blockDevice = new FlashIAPBlockDevice(iapLimits.start_address, iapLimits.available_size);
#endif
    
    Serial.println("Settings up TDBStore...");
    tdbStore = new mbed::TDBStore(blockDevice);
    auto result = tdbStore->init();
    if (result == MBED_SUCCESS) {
        Serial.println("|   Successfully set up TDBStore");
        storeOK = true;
    } else {
        Serial.println("|   Error while settings up TDBStore: " + String(result));
    }

    if (storeOK) {
        SavedBoxSettings previousSettings;
        result = getSavedBoxSettings(&previousSettings);

        if (result == MBED_SUCCESS) {
            Serial.println("Successfully loaded settings from flash, updating internal cache...");
            _sampleLength = previousSettings.savedSampleLength;
            _frequencyKey = previousSettings.savedFrequencyKey;
            _deviceID = previousSettings.savedDeviceID;
            _dataServerAddress = previousSettings.savedDataServerAddress;
            _dataServerPort = previousSettings.savedDataServerPort;
            _brokerAddress = previousSettings.savedBrokerAddress;
            _brokerPort = previousSettings.savedBrokerPort;
        } else {
            Serial.println("Failed to load settings from flash: " + String(result));
        }
    }

    //blockDevice->deinit();
}

int BoxSettingsClass::getSavedBoxSettings(SavedBoxSettings *whereToLoad) {
    Serial.println("Loading SavedBoxSettings from flash...");
    mbed::TDBStore::info_t info;
    auto result = tdbStore->get_info(tdbStoreKey, &info);

    if (result == MBED_ERROR_ITEM_NOT_FOUND)
        return result;
    
    // Allocate space for the value
    uint8_t buffer[info.size] {};
    size_t actual_size;

    // Get the value
    result = tdbStore->get(tdbStoreKey, buffer, sizeof(buffer), &actual_size);
    if (result != MBED_SUCCESS)
        return result;
    
    memcpy(whereToLoad, buffer, sizeof(SavedBoxSettings));
    return result;
}

int BoxSettingsClass::setSavedBoxSettings(SavedBoxSettings *whatToSave) {
    Serial.println("Writing SavedBoxSettings to flash...");
    return tdbStore->set(tdbStoreKey, reinterpret_cast<uint8_t*>(whatToSave), sizeof(SavedBoxSettings), 0);
}

BoxSettingsClass BoxSettings;