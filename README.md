# LernfabrikProjektArduino

__ATTENTION!!!__ If the code does not compile for weird reasons, see [Library Source Adjustments](#Library-Source-Adjustments) down below!

Arduino code for my "intelligent boxes" project for the Lernfabrik at Leuphana University. These boxes will host the Arduino Portenta H7, so the code is written for that platform. The job of these boxes is to demonstrate machine learning/IoT/industry 4.0 in factories. The Lernfabrik will be the host factory for these boxes.

The boxes themselfes shall be capable of detecting their current state based on sensory data. These states could for example be "in production", "in warehouse", "being moved from warehouse to production", "being moved in production", etc. The machine learning will be implemented in Python in a different Repository, which will be linked here. Since the trained models have to be executed on the Arduino, Tensorflow will be used for training and deploying and Tensorflow Lite for production on the Arduino.

## Setup
The Arduino is connected to 2 sensors:
- MPU6050 accel/gyro sensor
- HC-SR04 ultrasonic proximity sensor

Furthermore, a Touch Screen Shield 2.0 will be connected to the Arduino as a User Interface/Information display.

All these components will be mounted on a stackable box for better handling and realistic-ish use in a "factory".

### Dual core processing
With FullProject.ino being the main sketch file, it includes both CoreM7.h and CoreM4.h, which both implement Setup() and Loop() and will respectively called by the main sketch. Basically this means that all M7-related code is placed in CoreM7.cpp and all M4-related code is placed in CoreM4.cpp

The M4's job is to record data from the sensors at precisely the correct frequency and send them over to the M7 code. Code on the M4 core is time-critical.

The M7 is responsible for buffering the sensor data, preprocessing it and applying the ML-model on them. It also connects to a web server to broadcast its current state. The touch screen display is also managed by the M7 core. Code on the M7 core is not time-critical.

## Libraries
At this point there aren't many libraries required, but more will be added once the project matures.
- the required Arduino mbed libraries
- [I2C Device Library](https://github.com/jrowberg/i2cdevlib) from jrowberg

## Library Source Adjustments
Since the Arduino mbed API is still in its infancy, some library source code edits are required to compile this code.

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

