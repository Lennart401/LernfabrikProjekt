#include "RecordScreen.h"

#include <RPC_internal.h>
#include "ScreenHeading.h"
#include "OnboardingScreen.h"

// ---------------------------------------------------------
// screen stuff
static lv_obj_t *recordScreen;

static lv_indev_t *encoderIndev;
static lv_group_t *mainGroup;

// ---------------------------------------------------------
// heading
static lv_obj_t *btnBack;
static lv_obj_t *btnBackLabel;
// static lv_style_t btnBackStyle;

static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

// ---------------------------------------------------------
// main elements
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
        } else if (obj == btnBack) {
            onboarding_screen_load();
        }
    }
}

void record_screen_create(lv_indev_t *_encoderIndev) {
    // init screen
    recordScreen = lv_obj_create(NULL);

    mainGroup = lv_group_create();
    encoderIndev = _encoderIndev;

    // init back button
    // lv_style_init(&btnBackStyle);
    // lv_style_set_bg_opa(&btnBackStyle, LV_OPA_TRANSP);
    // lv_style_set_border_opa(&btnBackStyle, LV_OPA_TRANSP);
    // lv_style_set_shadow_opa(&btnBackStyle, LV_OPA_TRANSP);

    static lv_style_t btnBackStyle = screen_heading_get();
    
    btnBack = lv_btn_create(recordScreen);
    lv_obj_add_style(btnBack, &btnBackStyle, 0);
    lv_obj_align(btnBack, LV_ALIGN_TOP_LEFT, SCREEN_HEADING_BUTTON_X, SCREEN_HEADING_BUTTON_Y);
    lv_obj_set_size(btnBack, SCREEN_HEADING_BUTTON_W, SCREEN_HEADING_BUTTON_H);
    lv_obj_add_event_cb(btnBack, handleButtonClick, LV_EVENT_CLICKED, NULL);

    btnBackLabel = lv_label_create(btnBack);
    lv_label_set_recolor(btnBackLabel, true);
    lv_label_set_text(btnBackLabel, "#121212 " LV_SYMBOL_LEFT);
    lv_obj_center(btnBackLabel);

    // init heading
    lv_style_init(&labelHeadingStyle);
    lv_style_set_text_font(&labelHeadingStyle, &lv_font_montserrat_26);

    labelHeading = lv_label_create(recordScreen);
    lv_label_set_text(labelHeading, "Record");
    lv_obj_align(labelHeading, LV_ALIGN_TOP_LEFT, SCREEN_HEADING_LABEL_X, SCREEN_HEADING_LABEL_Y);
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);
    
    // init other ui elements
    // buttons 
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

    // lastly, add the back button to the group
    lv_group_add_obj(mainGroup, btnBack);
}

void record_screen_load() {
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