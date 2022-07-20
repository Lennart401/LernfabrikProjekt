#include "UnitDataProcessing.h"

#include <math.h>
#include <mbed.h>
#include <rtos.h>
#include <RPC_internal.h>
#include <algorithm>
#include <cstring>
//#include <time.h>
#include "model.h"
#include "InternalComm.h"

UnitDataProcessing::UnitDataProcessing(mbed::MbedCircularBuffer<Row, BUF_ROWS> *buffer)
    : crcBuffer(buffer)
    , currentMode(DPMode::IDLE)
    , tensorArena(new uint8_t[tensorArenaSize])
    , nrows(128)
    , nbytes(nrows * sizeof(float))
    , fftlen(nrows / 2) {
}

UnitDataProcessing::~UnitDataProcessing() {
    delete tensorArena;
    delete errorReporter;
    delete resolver;
    delete interpreter;
    
    pffft_destroy_setup(fftSetup);
    pffft_aligned_free(x_NoDC);
    pffft_aligned_free(y_NoDC);
    pffft_aligned_free(z_NoDC);
    pffft_aligned_free(abs_NoDC);
    pffft_aligned_free(x_fft);
    pffft_aligned_free(y_fft);
    pffft_aligned_free(z_fft);
    pffft_aligned_free(abs_fft);
    pffft_aligned_free(tmp_buffer);
}

void UnitDataProcessing::runDataProcessing() {
    // init
    errorReporter = new tflite::MicroErrorReporter();

    // this is where the model gets loaded
    model = tflite::GetModel(tflite_model);
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
                
                // mBoxSettings->setLastPrediction(lastPrediction);
                InternalComm.lastPrediction = lastPrediction;
                RPC1.println("POST data-processing/last-prediction " + String(lastPrediction));
                Serial.println("Prediction: " + String(lastPrediction));
            } else {
                rtos::ThisThread::sleep_for(200);
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
    if (fftSetup == nullptr) {
        Serial.println("fftSetup is null, creating fft structures...");

        fftSetup = pffft_new_setup(nrows, PFFFT_REAL);
        x_NoDC = (float*) pffft_aligned_malloc(nbytes);
        y_NoDC = (float*) pffft_aligned_malloc(nbytes);
        z_NoDC = (float*) pffft_aligned_malloc(nbytes);
        abs_NoDC = (float*) pffft_aligned_malloc(nbytes);
        x_fft = (float*) pffft_aligned_malloc(nbytes);
        y_fft = (float*) pffft_aligned_malloc(nbytes);
        z_fft = (float*) pffft_aligned_malloc(nbytes);
        abs_fft = (float*) pffft_aligned_malloc(nbytes);
        tmp_buffer = (float*) pffft_aligned_malloc(nbytes);
    }

    Serial.println("Starting to compute features...");
    clock_t start = clock();

    // Calc mean, min, max
    float x_mean = 0.0f, y_mean = 0.0f, z_mean = 0.0f, abs_mean = 0.0f;
    float x_min = 100.0f, y_min = 100.0f, z_min = 100.0f, abs_min = 100.0f;
    float x_max = -100.0f, y_max = -100.0f, z_max = -100.0f, abs_max = -100.0f;
    for (int i = 0; i < nrows; ++i) {
        x_mean += rows[i].acc_x;
        y_mean += rows[i].acc_y;
        z_mean += rows[i].acc_z;
        abs_mean += rows[i].acc_abs;

        x_min = std::min(x_min, rows[i].acc_x);
        y_min = std::min(y_min, rows[i].acc_y);
        z_min = std::min(z_min, rows[i].acc_z);
        abs_min = std::min(abs_min, rows[i].acc_abs);
        x_max = std::max(x_max, rows[i].acc_x);
        y_max = std::max(y_max, rows[i].acc_y);
        z_max = std::max(z_max, rows[i].acc_z);
        abs_max = std::max(abs_max, rows[i].acc_abs);
    }
    x_mean /= nrows;
    y_mean /= nrows;
    z_mean /= nrows;
    abs_mean /= nrows;

    // Calc std and no_DC signals
    float x_std = 0.0f, y_std = 0.0f, z_std = 0.0f, abs_std = 0.0f;
    for (int i = 0; i < nrows; ++i) {
        x_std += (rows[i].acc_x - x_mean) * (rows[i].acc_x - x_mean);
        y_std += (rows[i].acc_y - y_mean) * (rows[i].acc_y - y_mean);
        z_std += (rows[i].acc_z - z_mean) * (rows[i].acc_z - z_mean);
        abs_std += (rows[i].acc_abs - abs_mean) * (rows[i].acc_abs - abs_mean);
        x_NoDC[i] = rows[i].acc_x - x_mean;
        y_NoDC[i] = rows[i].acc_y - y_mean;
        z_NoDC[i] = rows[i].acc_z - z_mean;
        abs_NoDC[i] = rows[i].acc_abs - abs_mean;
    }
    x_std = sqrt(x_std / nrows);
    y_std = sqrt(y_std / nrows);
    z_std = sqrt(z_std / nrows);
    abs_std = sqrt(abs_std / nrows);

    // Perform fft on no_DC signals
    pffft_transform(fftSetup, x_NoDC, x_fft, NULL, PFFFT_FORWARD);
    pffft_transform(fftSetup, y_NoDC, y_fft, NULL, PFFFT_FORWARD);
    pffft_transform(fftSetup, z_NoDC, z_fft, NULL, PFFFT_FORWARD);
    pffft_transform(fftSetup, abs_NoDC, abs_fft, NULL, PFFFT_FORWARD);

    // ... reorder the ffts for processing
    pffft_zreorder(fftSetup, x_fft, tmp_buffer, PFFFT_FORWARD); memcpy(x_fft, tmp_buffer, nbytes);
    pffft_zreorder(fftSetup, y_fft, tmp_buffer, PFFFT_FORWARD); memcpy(y_fft, tmp_buffer, nbytes);
    pffft_zreorder(fftSetup, z_fft, tmp_buffer, PFFFT_FORWARD); memcpy(z_fft, tmp_buffer, nbytes);
    pffft_zreorder(fftSetup, abs_fft, tmp_buffer, PFFFT_FORWARD); memcpy(abs_fft, tmp_buffer, nbytes);

    // Variables for real fft and argmax of fft
    float x_rfft[fftlen], y_rfft[fftlen], z_rfft[fftlen], abs_rfft[fftlen];
    float x_rfft_sum = 0.0f, y_rfft_sum = 0.0f, z_rfft_sum = 0.0f, abs_rfft_sum = 0.0f;
    float x_rfft_max = 0.0f, y_rfft_max = 0.0f, z_rfft_max = 0.0f, abs_rfft_max = 0.0f;
    int x_rfft_argmax = 0, y_rfft_argmax = 0, z_rfft_argmax = 0, abs_rfft_argmax = 0;

    // Variables for power secpectrum density
    float x_rfft_psd[fftlen], y_rfft_psd[fftlen], z_rfft_psd[fftlen], abs_rfft_psd[fftlen];
    float x_rfft_psd_sum = 0.0f, y_rfft_psd_sum = 0.0f, z_rfft_psd_sum = 0.0f, abs_rfft_psd_sum = 0.0f;

    for (size_t i = 0; i < fftlen; ++i) {
        // calculate absolute value of fft (hypotenuse of real and imaginary parts)
        x_rfft[i] = std::hypot(x_fft[2*i], x_fft[2*i+1]);
        y_rfft[i] = std::hypot(y_fft[2*i], y_fft[2*i+1]);
        z_rfft[i] = std::hypot(z_fft[2*i], z_fft[2*i+1]);
        abs_rfft[i] = std::hypot(abs_fft[2*i], abs_fft[2*i+1]);

        // calculate sum of fft
        x_rfft_sum += x_rfft[i];
        y_rfft_sum += y_rfft[i];
        z_rfft_sum += z_rfft[i];
        abs_rfft_sum += abs_rfft[i];

        // calculate max and argmax of fft
        x_rfft_max = std::max(x_rfft_max, x_rfft[i]);
        y_rfft_max = std::max(y_rfft_max, y_rfft[i]);
        z_rfft_max = std::max(z_rfft_max, z_rfft[i]);
        abs_rfft_max = std::max(abs_rfft_max, abs_rfft[i]);

        x_rfft_argmax = (x_rfft_max < x_rfft[i]) ? i : x_rfft_argmax;
        y_rfft_argmax = (y_rfft_max < y_rfft[i]) ? i : y_rfft_argmax;
        z_rfft_argmax = (z_rfft_max < z_rfft[i]) ? i : z_rfft_argmax;
        abs_rfft_argmax = (abs_rfft_max < abs_rfft[i]) ? i : abs_rfft_argmax;

        // calculate power spectral density
        x_rfft_psd[i] = x_rfft[i] * x_rfft[i] / nrows;
        y_rfft_psd[i] = y_rfft[i] * y_rfft[i] / nrows;
        z_rfft_psd[i] = z_rfft[i] * z_rfft[i] / nrows;
        abs_rfft_psd[i] = abs_rfft[i] * abs_rfft[i] / nrows;

        // calculate the sum of the psd
        x_rfft_psd_sum += x_rfft_psd[i];
        y_rfft_psd_sum += y_rfft_psd[i];
        z_rfft_psd_sum += z_rfft_psd[i];
        abs_rfft_psd_sum += abs_rfft_psd[i];
    }

    // Peak frequencies
    float x_rfft_peak = x_rfft_argmax * 100.0 / 128.0;
    float y_rfft_peak = y_rfft_argmax * 100.0 / 128.0;
    float z_rfft_peak = z_rfft_argmax * 100.0 / 128.0;
    float abs_rfft_peak = abs_rfft_argmax * 100.0 / 128.0;

    // Entropy of fft and entropy of PSD (power spectral entropy)
    float x_rfft_entropy = 0.0f, y_rfft_entropy = 0.0f, z_rfft_entropy = 0.0f, abs_rfft_entropy = 0.0f;
    float x_rfft_pse = 0.0f, y_rfft_pse = 0.0f, z_rfft_pse = 0.0f, abs_rfft_pse = 0.0f;

    for (size_t i = 0; i < fftlen; ++i) {
        // normalize values of fft
        float ix = x_rfft[i] / x_rfft_sum;
        float iy = y_rfft[i] / y_rfft_sum;
        float iz = z_rfft[i] / z_rfft_sum;
        float ia = abs_rfft[i] / abs_rfft_sum;
        // calculate entropy of fft
        x_rfft_entropy += ix * std::log(ix);
        y_rfft_entropy += iy * std::log(iy);
        z_rfft_entropy += iz * std::log(iz);
        abs_rfft_entropy += ia * std::log(ia);

        // normalize values of psd
        float px = x_rfft_psd[i] / x_rfft_psd_sum;
        float py = y_rfft_psd[i] / y_rfft_psd_sum;
        float pz = z_rfft_psd[i] / z_rfft_psd_sum;
        float pabs = abs_rfft_psd[i] / abs_rfft_psd_sum;
        // calculate power spectral entropy
        x_rfft_pse += px * std::log(px);
        y_rfft_pse += py * std::log(py);
        z_rfft_pse += pz * std::log(pz);
        abs_rfft_pse += pabs * std::log(pabs);
    }

    // finalize fft entropy
    x_rfft_entropy = -x_rfft_entropy;
    y_rfft_entropy = -y_rfft_entropy;
    z_rfft_entropy = -z_rfft_entropy;
    abs_rfft_entropy = -abs_rfft_entropy;

    // finalize pse
    x_rfft_pse = -x_rfft_pse;
    y_rfft_pse = -y_rfft_pse;
    z_rfft_pse = -z_rfft_pse;
    abs_rfft_pse = -abs_rfft_pse;

    // Scale the features and fill the sample with them
    // mean
    sample[0] = (x_mean - input_offset[0]) / input_scale[0]; 
    sample[1] = (y_mean - input_offset[1]) / input_scale[1];
    sample[2] = (z_mean - input_offset[2]) / input_scale[2];
    sample[3] = (abs_mean - input_offset[3]) / input_scale[3];
    // min
    sample[4] = (x_min - input_offset[4]) / input_scale[4];
    sample[5] = (y_min - input_offset[5]) / input_scale[5];
    sample[6] = (z_min - input_offset[6]) / input_scale[6];
    sample[7] = (abs_min - input_offset[7]) / input_scale[7];
    // max
    sample[8] = (x_max - input_offset[8]) / input_scale[8];
    sample[9] = (y_max - input_offset[9]) / input_scale[9];
    sample[10] = (z_max - input_offset[10]) / input_scale[10];
    sample[11] = (abs_max - input_offset[11]) / input_scale[11];
    // std
    sample[12] = (x_std - input_offset[12]) / input_scale[12];
    sample[13] = (y_std - input_offset[13]) / input_scale[13];
    sample[14] = (z_std - input_offset[14]) / input_scale[14];
    sample[15] = (abs_std - input_offset[15]) / input_scale[15];
    // fft entropy
    sample[16] = (x_rfft_entropy - input_offset[16]) / input_scale[16];
    sample[17] = (y_rfft_entropy - input_offset[17]) / input_scale[17];
    sample[18] = (z_rfft_entropy - input_offset[18]) / input_scale[18];
    sample[19] = (abs_rfft_entropy - input_offset[19]) / input_scale[19];
    // peak frequency
    sample[20] = (x_rfft_peak - input_offset[20]) / input_scale[20];
    sample[21] = (y_rfft_peak - input_offset[21]) / input_scale[21];
    sample[22] = (z_rfft_peak - input_offset[22]) / input_scale[22];
    sample[23] = (abs_rfft_peak - input_offset[23]) / input_scale[23];
    // power spectral entropy
    sample[24] = (x_rfft_pse - input_offset[24]) / input_scale[24];
    sample[25] = (y_rfft_pse - input_offset[25]) / input_scale[25];
    sample[26] = (z_rfft_pse - input_offset[26]) / input_scale[26];
    sample[27] = (abs_rfft_pse - input_offset[27]) / input_scale[27];

    clock_t difference = clock() - start;
    long microsec = difference;
    Serial.println("Feature engineering took " + String(microsec) + "µs");
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
