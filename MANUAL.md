# Production manual for Lernfabrik KI Kisten
This manual describes the operative procedures needed for the finished box and also contains more instructions on certain topics that might be needed after the project has finished.

---

## Table of Contents
1. Operative procedures<br/>
    1.1 Prerequisites<br/>
    1.2 Settings up an environment for productive use<br/>

2. Technical instructions<br/>
    2.1 Compiling and uploading the code<br/>
    2.2 Exchanging the model<br/>


---

## 1. Operative procedures
TBD

### 1.1 Prerequisites

### 1.2 Settings up an environment for productive use


---

## 2. Technical instructions
This section contains technical instruction that anyone who wants to modify contents on the box without full code knowledge might need.

### 2.1 Compiling and uploading the code
Before you start:

1. If you are on linux or macOS, ensure that you have the Arduino CLI installed (`brew install arduino-cli`)
2. Make sure you have the required libraries installed and did the code adjustments described in the README
3. Make sure you have the "Arduino Mbed OS Portenta Boards" installed (goto `Arduino IDE -> Tools -> "Board..." -> Boards Manager` and Search for "portenta")

#### Compiling and uploading on Windows

1. Open either the CoreM7 or CoreM4 project using the Arduino IDE.
2. Plug the board into the PC. Use a native USB Port, NOT a hub. Using a hub can cause unexpected/wrong behaviour.
3. Select the board and chip corresponding to the project you opened, e.g. the M7 core for the CoreM7-project and vice versa:<br/>
    `Arduino IDE -> Tools -> Board... -> Arduino Mbed OS Portenta Boards -> Arduino Portenta H7 (M7 Core)`
4. Select the correct port:<br/>
    `Arduino IDE -> Tools -> Port`, select COM5 or whatever the Arduino is at.
5. Click the upload button `(➔)`

#### Compiling and uploading on linux/macOS (only tested on linux though)

For linux/macOS, there are some limitations with dfu-util and therefore the easiest way is to use the `arduino-util` script from the `scripts`-folder. It works as follows:

`arduino-util <core> <action> <port>`

- Core: either `m7` or `M4`, case-insensitive
- Action: either `compile` or `upload`, case-insensitive
- Port: the port of the Arduino, e. g. `/dev/ttyACM0`

To compile the code, run `arduino-util <core> compile`.<br/>
To upload (but not compile) the code, run `sudo arduino-util <core> upload <port>`<br/>
The latter command needs to be run with _sudo_ to access the device port and, mainly, so that dfu-util works correctly.

:warning: If you having installed the Arduino CLI in its default location of `/home/linuxbrew/.linuxbrew/bin/arduino-cli`, you will need to update the `arduino-util`-script.

### 2.2 Exchanging the model
Things to pay attention to:

1. Does the new model have the same classes as the old one? If not, you need to update the ProductionScreen on CoreM4
2. The variable name of the model changes.

#### Step by step
1. Export the new model as a c array using `xxd -i model_file.tflite > model.h`
2. Add the following lines to `model.h`

```c
// custom code
#define INPUT_TENSOR_SIZE <number of input tensors, e.g. 6>
#define OUTPUT_TENSOR_SIZE <number of output tensors, e.g. 8>
```

3. Replace the existing `model.h` file with the new `model.h` file
4. In `UnitDataProcessing.cpp`, change this line at the start of the `runDataProcessing` method to take the name of you new model, which is the variable name of the C array:

```c++
model = tflite::GetModel(NAME_OF_YOUR_MODEL);
```

5. If necessary, adjust the classes LUT on the ProductionPage of the UI.
