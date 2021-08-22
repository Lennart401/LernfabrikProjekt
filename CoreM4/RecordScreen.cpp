#include "RecordScreen.h"

#include <RPC_internal.h>

static lv_obj_t *recordScreen;

static lv_group_t *mainGroup;

static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

static lv_obj_t *btnStart;
static lv_obj_t *btnStartLabel;

static lv_obj_t *btnStop;
static lv_obj_t *btnStopLabel;

static lv_obj_t *btnCalibrate;
static lv_obj_t *btnCalibrateLabel;

static lv_obj_t *labelBufferSize;
static lv_obj_t *barBufferSize;

static void handleButtonClick(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnStart) {
            RPC1.println("SET mode/running 1");
        } else if (obj == btnStop) {
            RPC1.println("SET mode/running 0");
        } else if (obj == btnCalibrate) {
            RPC1.println("DO sensors/calibrate");
        }
    }
}

void record_screen_create() {
    // init screen
    recordScreen = lv_obj_create(NULL);

    // init heading

    // init other ui elements
    // buttons 
    mainGroup = lv_group_create();

    lv_style_init(&labelHeadingStyle);
    lv_style_set_text_font(&labelHeadingStyle, &lv_font_montserrat_26);

    labelHeading = lv_label_create(recordScreen);
    lv_label_set_text(labelHeading, "Record");
    lv_obj_align(labelHeading, LV_ALIGN_TOP_LEFT, 10, 30);
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);
    
    btnStart = lv_btn_create(recordScreen);
    lv_obj_add_event_cb(btnStart, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btnStart, LV_ALIGN_RIGHT_MID, -10, -55);
    lv_group_add_obj(mainGroup, btnStart);

    btnStartLabel = lv_label_create(btnStart);    
    lv_label_set_text(btnStartLabel, "Start");

    btnStop = lv_btn_create(recordScreen);
    lv_obj_add_event_cb(btnStop, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btnStop, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_group_add_obj(mainGroup, btnStop);

    btnStopLabel = lv_label_create(btnStop);
    lv_label_set_text(btnStopLabel, "Stop");

    btnCalibrate = lv_btn_create(recordScreen);
    lv_obj_add_event_cb(btnCalibrate, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btnCalibrate, LV_ALIGN_RIGHT_MID, -10, 55);
    lv_group_add_obj(mainGroup, btnCalibrate);

    btnCalibrateLabel = lv_label_create(btnCalibrate);
    lv_label_set_text(btnCalibrateLabel, "Calibrate");

    // buffer bar
    labelBufferSize = lv_label_create(recordScreen);
    lv_obj_align(labelBufferSize, LV_ALIGN_BOTTOM_LEFT, 10, -45);
    lv_label_set_text(labelBufferSize, "Buffer fill:");

    barBufferSize = lv_bar_create(recordScreen);
    lv_obj_set_size(barBufferSize, 150, 30);
    lv_obj_align(barBufferSize, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    //lv_bar_set_anim_time(barBufferSize, 1000);
    lv_bar_set_value(barBufferSize, 0, LV_ANIM_ON);
}

void record_screen_load(lv_indev_t *encoderIndev) {
    lv_scr_load(recordScreen);
    lv_indev_set_group(encoderIndev, mainGroup);
}

void record_screen_receive_message(String command, String subject, String payload) {
    if (command == "POST") {
        if (subject == "buffer/fill") {
            lv_bar_set_value(barBufferSize, payload.toInt(), LV_ANIM_ON);
        }
    }
}