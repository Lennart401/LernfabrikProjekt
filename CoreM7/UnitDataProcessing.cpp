#include "UnitDataProcessing.h"

#include <math.h>
#include <mbed.h>
#include <rtos.h>
#include <RPC_internal.h>
#include <algorithm>
#include "model.h"

UnitDataProcessing::UnitDataProcessing(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer, BoxSettings *boxSettings)
    : mBoxSettings(boxSettings)
    , crcBuffer(buffer)
    , currentMode(DPMode::IDLE)
    , tensorArena(new uint8_t[tensorArenaSize])
    , nrows(100) {
}

UnitDataProcessing::~UnitDataProcessing() {
    delete tensorArena;
    delete errorReporter;
    delete resolver;
    delete interpreter;
}

void UnitDataProcessing::runDataProcessing() {
    // init
    errorReporter = new tflite::MicroErrorReporter();

    // this is where the model gets loaded
    model = tflite::GetModel(model_protocol_v2_1_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(errorReporter, "Model provided is schema version %d not equal to supported version %d.", 
                             model->version(), TFLITE_SCHEMA_VERSION);
        // dispatch error to M4
        return;
    }

    // this is what will "run" the model
    resolver = new tflite::AllOpsResolver(); // just load all layer types for simplicity, the memory saved by this is not that important
    interpreter = new tflite::MicroInterpreter(model, *resolver, tensorArena, tensorArenaSize, errorReporter);

    // allocate memory for the tensors (internally in the tensorArena; the tensorArena just needs to be big enough)
    TfLiteStatus allocateStatus = interpreter->AllocateTensors();
    if (allocateStatus != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(errorReporter, "AllocateTensors() failed!");
        // dispatch error to M4
        return;
    }

    // quick references to the input and output tensors
    inputTensor = interpreter->input(0);
    outputTensor = interpreter->output(0);

    // this is where we will load the raw data from the buffer and the features for the sample in
    Row *rawData = new Row[nrows];
    float *sample = new float[INPUT_TENSOR_SIZE];

    // loop
    while (running) {
        switch (currentMode)
        {
        case RUNNING:
            // when there are enough rows, start processing
            if (crcBuffer->size() >= nrows) {
                // get nrows and build the features into the sample variable
                crcBuffer->pop(rawData, nrows);
                computeFeatures(sample, rawData);
                // make a prediction on the sample
                lastPrediction = predict(sample);
                
                mBoxSettings->setLastPrediction(lastPrediction);
                RPC1.println("POST data-processing/last-prediction " + String(lastPrediction));
                Serial.println("Prediction: " + String(lastPrediction));
            } else {
                rtos::ThisThread::sleep_for(100);
            }
            break;
        
        case IDLE:
            rtos::ThisThread::sleep_for(200);
            break;
        }
    }

    delete rawData;
    delete sample;
}

void UnitDataProcessing::stopDataProcessing() {
    running = false;
}

void UnitDataProcessing::setMode(DPMode newMode) {
    currentMode = newMode;
}

void UnitDataProcessing::computeFeatures(float *sample, Row *rows) {
    Serial.println("Starting to compute features...");

    // computing the mean:
    // start by adding all values together, then at the end, divide by the number of values added
    float x_mean = 0.0f, y_mean = 0.0f, z_mean = 0.0f;
    for (int i = 0; i < nrows; ++i) {
        x_mean += rows[i].acc_x;
        y_mean += rows[i].acc_y;
        z_mean += rows[i].acc_z;
    }
    x_mean /= nrows;
    y_mean /= nrows;
    z_mean /= nrows;
    Serial.println("Computed mean: " + String(x_mean) + " " + String(y_mean) + " " + String(z_mean));

    // computing the standard deviation:
    // formula =  sqrt( 1/n * sum( i=1->n, (x(i) - mean)² ) )
    // first, add the squares of all deviation from the mean, then divide by number of values and get the square root of that
    float x_std = 0.0f, y_std = 0.0f, z_std = 0.0f;
    for (int i = 0; i < nrows; ++i) {
        x_std += (rows[i].acc_x - x_mean) * (rows[i].acc_x - x_mean);
        y_std += (rows[i].acc_y - y_mean) * (rows[i].acc_y - y_mean);
        x_std += (rows[i].acc_z - z_mean) * (rows[i].acc_z - z_mean);
    }
    x_std = sqrt(x_std / nrows);
    y_std = sqrt(y_std / nrows);
    z_std = sqrt(z_std / nrows);
    Serial.println("Computed std: " + String(x_std) + " " + String(y_std) + " " + String(z_std));

    sample[0] = x_mean; sample[1] = y_mean; sample[2] = z_mean;
    sample[3] = x_std;  sample[4] = y_std;  sample[5] = z_std;
}

uint8_t UnitDataProcessing::predict(float *sample) {
    // copy sample data into the inputTensors data (float) array and start the interpreter (run the model)
    memcpy(inputTensor->data.f, sample, sizeof(float) * INPUT_TENSOR_SIZE);
    TfLiteStatus invokeStatus = interpreter->Invoke();

    // find the argmax from the data (float) of the output and return that +1 (0 = no movement, 1 = first movement type)
    float *outputArray = outputTensor->data.f;
    // std::max_element returns the maximum element from the outputArray 
    // (i. e. in the area of outputArray[0] -> outputArray[last])
    // 
    // std::distance computes the distance between the start of the array and the element, which is its position in the array
    int element = std::distance(outputArray, std::max_element(outputArray, outputArray + OUTPUT_TENSOR_SIZE));
    return element + 1;
}
