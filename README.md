# Lernfabrik Webserver


## Communication Protocol
This server/the whole system uses a custom protocol to save as much data as possible and just send the raw data. The message contains a header and a payload. The current version of the protocol is 1.

### Header structure

| Version | Device-ID | Number of Sensors | Sensors                           |
| ------- | --------- | ----------------- | --------------------------------- |
| 1 byte  | 1 byte    | 2 bytes           | 1 byte for type, 4 bytes for name |

The "type"-byte for every sensor describes a certain type of variable:

| Byte | Variable Type |
| ---- | ------------- |
| \x00 | uint8_t       |
| \x01 | uint16_t      |
| \x02 | uint32_t      |
| \x03 | uint64_t      |
| \x04 | float (32)    |
| \x05 | double (64)   |

An example:

    \x01\x01\x00\x02     --- Version 1, Device ID 1, 2 Sensors
    \x03\x54\x49\x4d\x45 --- Type uint64_t, Name TIME
    \x04\x41\x43\x43\x58 --- Type float (32), Name ACCX

### Payload structure

The payload is just the raw bytes from every row from every measurement from the C++ code from the Arduino appended together. It is sorted by the sensors listed in the header.

An example, continuing the one from above:

    \x00\x00\x00\x00\x00\x00\x00\x01 --- (uint64_t) 1
    \x3f\xcc\xcc\xcd                 --- (float32)  1.6
    \x00\x00\x00\x00\x00\x00\x00\x10 --- (uint64_t) 16
    \x3e\xa1\x47\xae                 --- (float32)  0.315
