#include "BoxSettings.h"

#include <RPC_internal.h>

BoxSettings::BoxSettings()
    : _sampleLength(1000)
    , _frequencyKey(0x08)
    , _movementTypeKey(0x02)
    , _useMovementTypes(false) {
} 

uint8_t BoxSettings::getModeDependendMovementTypeLUTKey() {
    if (_useMovementTypes) {
        return _movementTypeKey;
    } else {
        return 0x0;
    }
}

void BoxSettings::processRPCCommand(String command, String subject, String payload) {
    // GET responses
    if (command == "GET") {
        if (subject == "settings/sample-length") {
            char buffer[38];
            sprintf(buffer, "POST settings/sample-length %u", _sampleLength);
            RPC1.println(buffer);
        } else if (subject == "settings/frequency") {
            char buffer[26];
            sprintf(buffer, "POST settings/frequency %2u", _frequencyKey);
            RPC1.println(buffer);
        } else if (subject == "samples/movement-type") {
            char buffer[29];
            sprintf(buffer, "POST samples/movement-type %2u", _movementTypeKey);
            RPC1.println(buffer);
        }
    }

    // SET processing
    else if (command == "SET") {
        if (subject == "settings/sample-length") {
            _sampleLength = payload.toInt();
        } else if (subject == "settings/frequency") {
            _frequencyKey = static_cast<uint8_t>(payload.toInt() & 0xFF);
        } else if (subject == "samples/movement-type") {
            _movementTypeKey = static_cast<uint8_t>(payload.toInt() & 0xFF);
        }
    }
}
