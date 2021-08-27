# Lernfabrik Webserver


## Communication Protocol
The data server uses a custom data format to save as much space as possible and keep the size of the data as small as possible while still being very simple and easy to parse. The protocol builds on the Hypertext Transfer Protocol (HTTP) and sets a standard for the contents of the request. 

The content consists of a header describing how to parse the payload, and a payload itself. The current version of the protocol is 2.

## Protocol Version 2
Protocol Version 2 delivers much more metadata than its predecessor protocol version 1, with both a device and packet id as well as information about the recording frequency and the movement type (e.g. for a data-sample of one of the types).

### Header structure
| Version | Device-ID | Packet ID | Frequency | Movement-Type | Number of Sensors | Sensors                           |
| ------- | --------- | --------- | --------- | ------------- | ----------------- | --------------------------------- |
| 1 byte  | 1 byte    | 2 bytes   | 4 bits    | 4 bits        | 1 byte            | 1 byte for type, 5 bytes for name |

The header does not specify a length of the content since this can be deducted from the `Content-Length`-Attribute set in the HTTP header.

#### Frequency
The frequency is encoded as a key to a lookup table. This table is as follows:

| Key | Frequency | Key | Frequency |
| --- | --------- | --- | --------- |
| 0x0 |      1 Hz | 0x8 |    100 Hz |
| 0x1 |      2 Hz | 0x9 |    200 Hz |
| 0x2 |      4 Hz | 0xA |    400 Hz |
| 0x3 |      5 Hz | 0xB |    500 Hz |
| 0x4 |     10 Hz | 0xC |   1000 Hz |
| 0x5 |     20 Hz | 0xD |   2000 Hz |
| 0x6 |     40 Hz | 0xE |   4000 Hz |
| 0x7 |     50 Hz | 0xF |   5000 Hz |

#### Movement Type
The movement-type is encoded as a key to a lookup table. This table is as follows:

| Key | Movement             | Key | Movement           |
| --- | -------------------- | --- | ------------------ |
| 0x0 | None (just data)     | 0x8 |                    |
| 0x1 | Sit on ramp          | 0x9 |                    |
| 0x2 | Sit ready for pickup | 0xA |                    |
| 0x3 | Sit on moving wagon  | 0xB |                    |
| 0x4 | No movement          | 0xC |                    |
| 0x5 |                      | 0xD |                    |
| 0x6 |                      | 0xE |                    |
| 0x7 |                      | 0xF |                    |

The empty field (0x5 to 0xF) are left intentionally blank as they have no defintion yet and may be filled by later revisions of the protocol.

#### Sensors
There are 6 bytes per sensor, so this field of the header is `Number of Sensors` * 6 bytes long. The first byte of each sensor specifies its type (see protocol version 1) and the last 5 bytes specify the name as an ANSI string.

## Protocol Version 1
The first protocol can send a batch of data with the device id being the only included metadata.

### Header structure

| Version | Device-ID | Number of Sensors | Sensors                           |
| ------- | --------- | ----------------- | --------------------------------- |
| 1 byte  | 1 byte    | 2 bytes           | 1 byte for type, 4 bytes for name |

The header does not specify a length of the content since this can be deducted from the `Content-Length`-Attribute set in the HTTP header.

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
