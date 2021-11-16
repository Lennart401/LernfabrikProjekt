# LernfabrikArduino

| :warning: If the code does not compile for weird reasons, see [Library Source Adjustments](#Library-Source-Adjustments) down below! |
| -- |

This is the Arduino code for my "intelligent boxes" project for the Lernfabrik at Leuphana University. These boxes will host the Arduino Portenta H7, so the code is written for that platform. The job of these boxes is to demonstrate machine learning/IoT/industry 4.0 in factories. The Lernfabrik will be the host factory for these boxes.

The boxes themselfes shall be capable of detecting their current state based on sensory data. These states could for example be "in production", "in warehouse", "being moved from warehouse to production", "being moved in production", etc. The machine learning will be implemented in Python in a different Repository, which will be linked here. Since the trained models have to be executed on the Arduino, Tensorflow will be used for training and deploying and Tensorflow Lite for production on the Arduino.

## Setup
The Arduino is connected to 2 sensors:
- MPU6050 accel/gyro sensor
- HC-SR04 ultrasonic proximity sensor

Furthermore, a TFT/LCD ILI9341 panel with a 4-wire resistive touchscreen is connected to the Arduino as a user interface and information display.

All these components will be mounted on a stackable box for better handling and realistic-ish use in a "factory". The arduino will be powered using a 2300 mAh rechargable Li-Ion battery connected to the arduinos own battery charging port.

## Pinout
-- WIP --

## Dual core processing
Since both cores on the portenta H7 need to be programmed seperately, each core-specific code is in their respective subprojects `CoreM7` and `CoreM4` with their .ino-files. The cores communicate soly via a custom text-based protocol layered on top of the RPC message-based communication mechanism (see [custom RPC protocol](#Internal-RPC-communication-protocol)).

### Core M4
The M4's job is to control the user interface using [LVGL](https://lvgl.io/), display information received from the M7 core and dispatch input events (e. g. mode change) to the M7 core.

### Core M7
The M7 is responsible for multiple different tasks and is therefore split into multiple units:

- Reading sensor data und writing it into a ring buffer. See [UnitSensors](/CoreM7/UnitSensors.h)
- Send the recorded data to a the [LernfabrikDataserver](https://github.com/Lennart401/LernfabrikDataserver) over WiFi. See [UnitWiFi](/CoreM7/UnitWiFi.h)
- Running a machine learning modell using TensorFlow Lite
- Communicate with the M4 core using a [custom RPC protocol](#Internal-RPC-communication-protocol)

Depeding on its mode of operation, the M7 will...
- ...either record sensor data and send the data in batches to a webserver (See [LernfabrikDataserver](https://github.com/Lennart401/LernfabrikDataserver))
- ...or record sensor data and feed it into the Machine Learning Model Unit and then only send its currect state/position to the webserver

## Libraries
- Arduino mbed-enabled boards (see Arduino Board Manager)
- [I2C Device Library](https://github.com/jrowberg/i2cdevlib) from jrowberg
- [LVGL](https://lvgl.io) &ndash; see Arduino Library Manager
- [Adafruit ILI9341 TFT/LCD driver](https://github.com/adafruit/Adafruit_ILI9341) &ndash; see Arduino Library Manager
    - this requires the [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)

### Arduino WiFi library
In order for the WiFi library to work as intended, the wifi firmware might need to be updated. This is done using the example sketch "PortentaWiFiFirmwareUpdater". With some version incompatibilities, this update might fail (the updater cannot find any files). To solve this, see my reply in the Arduino Portenta forum [here](https://forum.arduino.cc/index.php?topic=712615.msg4862415#msg4862415).

## Library Source Adjustments
:warning: This section has not been checked for being up-to-date with the newest Arduino SDK. While the code compiles at the moment, some of these adjustment might not be required anymore.

With the Arduino mbed libraries still being developed, some library source code edits are required to compile this code. There might be more adjustments needed depending on libray versions.

### MPU6050 Library
At the very end of `MPU6050.cpp` there is a macro `printfloatx` defined, which uses the `char *dtostrf` function, that, at the point of writing, is not yet included in the Arduino mbed library. To fix the issue, the following code has to be pasted above the macro:

```c++
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
    char fmt[20];
    sprintf(fmt, "%%%d.%df", width, prec);
    sprintf(sout, fmt, val);
    return sout;
}
```

Futhermore, the definition of `BUFFER_LENGTH` from `I2Cdev.cpp` does not make it to `MPU6050.cpp` for some reason and the file fails to compile. To fix it, simple add the guarded definition of BUFFER_LENGTH to the file just above the method where it is needed, `int8_t MPU6050::GetCurrentFIFOPacket(uint8_t*, uint8_t)`:

```c++
#ifndef BUFFER_LENGTH
#define BUFFER_LENGTH 32
#endif
```

## Protocols and definitions

### Internal RPC communication protocol
This protocol is a text based protocol layered on top of the RPC message-based communication mechanism. Each transmitted line is handled as a single message (i.e. each message terminates with a line feed (`\n`) character). The protocol has been inspired by HTTP and MQTT.

Each message consists of the three parts `<command> <subject> [(optional) payload]` with the exception of the `PRINT`-command, which does not contain a subject.

There are five types of commands in total, with only some of them making use of all three message parts.

| Command                    | Usage                                                                                     |
| -------------------------- | ----------------------------------------------------------------------------------------- |
| `GET <subject>`            | Request information. Sent from M4 -> M7, M7 will respond with `POST` on the same subject. |
| `POST <subject> <payload>` | Send information (e.g. to be displayed). Sent from M7 -> M4, no response.                 |
| `SET <subject> <payload>`  | Change mode/settings/etc. Sent from M4 -> M7, no response.                                |
| `DO <subject>`             | Run an action without any payload. Sent from M4 -> M7, no response.                       |
| `PRINT <payload>`          | Print the payload to the Serial output. Sent from M4 -> M7, no response.                  |

The subject string shall be formatted like a MQTT subject string, e.g. `wifi/status`.

### WiFi Dataserver communication protocol
To minimize the data that required to be sent over wifi, I have developed a simple and lightweigt protocol. It's details can be found in the README of my [LernfabrikDataserver](https://github.com/Lennart401/LernfabrikDataserver/#communication-protocol)-Repository.

## Using the Code/a Box in a production enviroment
Operative procedures and other information on productivly using the box is described in the user manual. See [MANUAL.md](https://github.com/Lennart401/LernfabrikArduino/MANUAL.md).
