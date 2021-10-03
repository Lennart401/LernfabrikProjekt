#include "UnitDataProcessing.h"

#include <math.h>
#include <mbed.h>
#include <rtos.h>
#include <RPC_internal.h>
#include "model.h"

UnitDataProcessing::UnitDataProcessing(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer, BoxSettings *boxSettings)
    : settings(boxSettings)
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
    //static tflite::MicroErrorReporter microErrorReporter;
    //errorReporter = &microErrorReporter;
    errorReporter = new tflite::MicroErrorReporter();

    model = tflite::GetModel(model_protocol_v2_1_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(errorReporter, "Model provided is schema version %d not equal to supported version %d.", 
                             model->version(), TFLITE_SCHEMA_VERSION);
        // dispatch error to M4
        return;
    }

    //static tflite::AllOpsResolver resolver;
    //static tflite::MicroInterpreter static_interpreter(model, resolver, tensorArena, tensorArenaSize, errorReporter);
    //interpreter = &static_interpreter;
    resolver = new tflite::AllOpsResolver();
    interpreter = new tflite::MicroInterpreter(model, *resolver, tensorArena, tensorArenaSize, errorReporter);

    TfLiteStatus allocateStatus = interpreter->AllocateTensors();
    if (allocateStatus != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(errorReporter, "AllocateTensors() failed!");
        // dispatch error to M4
        return;
    }

    inputTensor = interpreter->input(0);
    outputTensor = interpreter->output(0);

    //Row data[nrows];

    // loop
    while (running) {
        switch (currentMode)
        {
        case RUNNING:
            if (crcBuffer->size() >= nrows) {
                //Serial.println("1: collected enough rows");
                Row *rawData = (Row*) malloc(sizeof(Row) * nrows);
                //Row rawData[nrows];
                crcBuffer->pop(rawData, nrows);
                //Serial.println("2: popped rows into buffer");

                float *sample = (float*) malloc(sizeof(float) * 6);
                //Serial.println("Creating sample...");
                computeFeatures(sample, rawData);
                //Serial.println("3: computed features");
                lastPrediction = predict(sample);
                //Serial.println("4: made prediction");
                
                //Serial.println("Prediction: " + String(lastPrediction));
                //Serial.println(crcBuffer->size());
                RPC1.println("POST data-processing/last-prediction " + String(lastPrediction));
                Serial.println("POST data-processing/last-prediction " + String(lastPrediction));
                rtos::ThisThread::sleep_for(100);

                free(rawData);
                free(sample);
            } else {
                rtos::ThisThread::sleep_for(100);
            }
            break;
        
        case IDLE:
            rtos::ThisThread::sleep_for(200);
            break;
        }
    }
}

void UnitDataProcessing::stopDataProcessing() {
    running = false;
}

void UnitDataProcessing::setMode(DPMode newMode) {
    currentMode = newMode;
}

void UnitDataProcessing::computeFeatures(float *sample, Row *rows) {
    Serial.println("Starting to compute features...");

    float x_mean = 0.0f, y_mean = 0.0f, z_mean = 0.0f;
    for (int i = 0; i < nrows; ++i) {
        //Serial.print(i + " ");
        x_mean += rows[i].acc_x;
        y_mean += rows[i].acc_y;
        z_mean += rows[i].acc_z;
    }
    x_mean /= nrows;
    y_mean /= nrows;
    z_mean /= nrows;
    Serial.println("Computed mean: " + String(x_mean) + " " + String(y_mean) + " " + String(z_mean));

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
    // inputTensor->data.f = sample;
    memcpy(inputTensor->data.f, sample, sizeof(float) * 6);
    TfLiteStatus invokeStatus = interpreter->Invoke();

    if (outputTensor->data.f[0] > 0.5) return 1;
    else if (outputTensor->data.f[1] > 0.5) return 2;
    else if (outputTensor->data.f[2] > 0.5) return 3;
    else if (outputTensor->data.f[3] > 0.5) return 4;
    else return 0;
}
