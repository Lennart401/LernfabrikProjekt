#include "RecordScreen.h"

#include <RPC_internal.h>

static lv_obj_t *recordScreen;

static lv_group_t *mainGroup;

static lv_obj_t *btnStart;
static lv_obj_t *btnStartLabel;

static lv_obj_t *btnStop;
static lv_obj_t *btnStopLabel;

static lv_obj_t *btnCalibrate;
static lv_obj_t *btnCalibrateLabel;

static lv_obj_t *labelBufferSize;
static lv_obj_t *barBufferSize;

static void handleButtonClick(lv_obj_t *obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        if (obj == btnStart) {
            RPC1.println("SET mode/running 1");
        } else if (obj == btnStop) {
            RPC1.println("SET mode/running 0");
        } else if (obj == btnCalibrate) {
            RPC1.println("DO sensors/calibrate");
        }
    }
}

void record_screen_create(lv_indev_t *encoderIndev) {
    // init screen
    recordScreen = lv_obj_create(NULL, NULL);

    // init heading

    // init other ui elements
    // buttons 
    mainGroup = lv_group_create();
    lv_indev_set_group(encoderIndev, mainGroup);
    
    btnStart = lv_btn_create(recordScreen, NULL);
    lv_obj_set_event_cb(btnStart, handleButtonClick);
    lv_obj_align(btnStart, NULL, LV_ALIGN_IN_RIGHT_MID, -10, -55);
    lv_group_add_obj(mainGroup, btnStart);

    btnStartLabel = lv_label_create(btnStart, NULL);    
    lv_label_set_text(btnStartLabel, "Start");

    btnStop = lv_btn_create(recordScreen, NULL);
    lv_obj_set_event_cb(btnStop, handleButtonClick);
    lv_obj_align(btnStop, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 0);
    lv_group_add_obj(mainGroup, btnStop);

    btnStopLabel = lv_label_create(btnStop, NULL);
    lv_label_set_text(btnStopLabel, "Stop");

    btnCalibrate = lv_btn_create(recordScreen, NULL);
    lv_obj_set_event_cb(btnCalibrate, handleButtonClick);
    lv_obj_align(btnCalibrate, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 55);
    lv_group_add_obj(mainGroup, btnCalibrate);

    btnCalibrateLabel = lv_label_create(btnCalibrate, NULL);
    lv_label_set_text(btnCalibrateLabel, "Calibrate");

    // buffer bar
    labelBufferSize = lv_label_create(recordScreen, NULL);
    lv_obj_align(labelBufferSize, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -45);
    lv_label_set_text(labelBufferSize, "Buffer fill:");

    barBufferSize = lv_bar_create(recordScreen, NULL);
    lv_obj_set_size(barBufferSize, 150, 30);
    lv_obj_align(barBufferSize, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);
    lv_bar_set_anim_time(barBufferSize, 1000);
    lv_bar_set_value(barBufferSize, 0, LV_ANIM_ON);
}

void record_screen_load() {
    lv_scr_load(recordScreen);
}

void record_screen_receive_message(String command, String subject, String payload) {
    if (command == "POST") {
        if (subject == "buffer/fill") {
            lv_bar_set_value(barBufferSize, payload.toInt(), LV_ANIM_ON);
        }
    }
}