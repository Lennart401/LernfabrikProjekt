#ifndef UNIT_DATA_PROCESSING_H_
#define UNIT_DATA_PROCESSING_H_

#include <Arduino.h>
#include <TensorFlowLite.h>
#include "Row.h"
#include "MbedCircularBuffer.h"
#include "BoxSettings.h"

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/c/common.h"

class UnitDataProcessing {

public:
    enum DPMode {
        RUNNING,
        IDLE
    };

    UnitDataProcessing(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer, BoxSettings *boxSettings);
    ~UnitDataProcessing();
    void runDataProcessing();
    void stopDataProcessing();

    void setMode(DPMode mode);

private:
    void computeFeatures(float *sample, Row *rows);
    uint8_t predict(float *sample);

    uint8_t lastPrediction = 0;
    const tflite::Model *model = nullptr;
    tflite::ErrorReporter *errorReporter = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    tflite::AllOpsResolver *resolver = nullptr;
    TfLiteTensor *inputTensor = nullptr;
    TfLiteTensor *outputTensor = nullptr;

    const int tensorArenaSize = 2 * 1024;
    uint8_t *tensorArena;

    uint32_t nrows;
    mbed::MbedCircularBuffer<Row, BUF_ROWS> *crcBuffer;
    BoxSettings *settings;

    volatile bool running = true;
    volatile DPMode currentMode;

};

#endif // UNIT_DATA_PROCESSING_H_