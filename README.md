# LernfabrikArduino

__ATTENTION!!!__ If the code does not compile for weird reasons, see [Library Source Adjustments](#Library-Source-Adjustments) down below!

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
FullProject.ino is the main arduino project file with is uploaded to the arduino. The processor-specific code for both the M7 and M4 core is in their respective ```CoreMx.h/.cpp``` files. These header files contain the same ```setup()``` and ```loop()``` methods as the main arduino sketch and are called from the main sketch file.

In summary, all M7-related core is in [```CoreM7.h```](CoreM7.h)[```/.cpp```](CoreM7.cpp) and all M4-related core is in [```CoreM7.h```](CoreM4.h)[```/.cpp```](CoreM4.cpp).

### Core M4
The M4's job is to control the user interface using [Little VGL](https://lvgl.io/), display information received from the M7 core and hand input events (e. g. mode change) to the M7 core.

The communication will work using a [custom protocol](#Internal-RPC-communication-protocol) over the RPC-communication mechanism.

### Core M7
The M7 is responsible for multiple tasks and therefore has multiple threads running.

- Reading sensor data und writing it into a ring buffer. See [UnitSensors](UnitSensors.h)
- Communicating to a server over WiFi. See [UnitWiFi](UnitWiFi.h)
- Running a machine learning modell using TensorFlow Lite
- Communicate with the M4 core using a [custom RPC protocol](#Internal-RPC-communication-protocol)

Depeding on its mode of operation, the M7 will...
- ...either record sensor data and send the data in batches to a webserver (See [LernfabrikWebserver](https://github.com/Lennart401/LernfabrikWebserver))
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
With the Arduino mbed libraries still being developed, some library source code edits are required to compile this code. There might be more adjustments needed depending on libray versions.

### MPU6050 Library
At the very end of `MPU6050.cpp` there is a macro `printfloatx` defined, which uses the `char *dtostrf` function, that, at the point of writing, is not yet included in the Arduino mbed library. To fix the issue, the following code has to be pasted above the macro:

    char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
	    char fmt[20];
	    sprintf(fmt, "%%%d.%df", width, prec);
	    sprintf(sout, fmt, val);
	    return sout;
    }

Futhermore, the definition of `BUFFER_LENGTH` from `I2Cdev.cpp` does not make it to `MPU6050.cpp` for some reason and the file fails to compile. To fix it, simple add the guarded definition of BUFFER_LENGTH to the file just above the method where it is needed, `int8_t MPU6050::GetCurrentFIFOPacket(uint8_t*, uint8_t)`:

    #ifndef BUFFER_LENGTH
    // band-aid fix for platforms without Wire-defined BUFFER_LENGTH (removed from some official implementations)
    #define BUFFER_LENGTH 32
    #endif

## Protocols and definitions

### Internal RPC communication protocol
-- WIP --

### WiFi Webserver communication protocol
To minimize the data that required to be sent over wifi, I have develop a simple and lightweigt protocol. It's details can be found in the README of my [LernfabrikWebserver](https://github.com/Lennart401/LernfabrikWebserver/#communication-protocol)-Repository.
