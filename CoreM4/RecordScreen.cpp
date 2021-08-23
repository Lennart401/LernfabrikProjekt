#include "RecordScreen.h"

#include <RPC_internal.h>
#include "ScreenHeading.h"
#include "OnboardingScreen.h"

#define BTN_WIDTH 120
#define BTN_HEIGHT 35

// ---------------------------------------------------------
// screen stuff
static lv_obj_t *recordScreen;

static lv_indev_t *encoderIndev;
static lv_group_t *mainGroup;

// ---------------------------------------------------------
// heading
static lv_obj_t *btnBack;
static lv_obj_t *btnBackLabel;

static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

// ---------------------------------------------------------
// main elements
static lv_obj_t *btnStartStop;
static lv_obj_t *btnStartStopLabel;

static lv_obj_t *btnCalibrate;
static lv_obj_t *btnCalibrateLabel;

static lv_obj_t *labelBufferSize;
static lv_obj_t *arcBufferSize;

static void handleButtonClick(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnCalibrate) {
            RPC1.println("DO sensors/calibrate");
        } else if (obj == btnBack) {
            onboarding_screen_load();
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (obj == btnStartStop) {
            if ((lv_obj_get_state(btnStartStop) & LV_STATE_CHECKED) != 0) {
                lv_label_set_text(btnStartStopLabel, LV_SYMBOL_PAUSE);
                RPC1.println("SET mode/running 1");
            } else {
                lv_label_set_text(btnStartStopLabel, LV_SYMBOL_PLAY);
                RPC1.println("SET mode/running 0");
            }
        }
    }
}

void record_screen_create(lv_indev_t *_encoderIndev) {
    // init screen
    recordScreen = lv_obj_create(NULL);

    mainGroup = lv_group_create();
    encoderIndev = _encoderIndev;

    // init back button
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
    btnStartStop = lv_btn_create(recordScreen);
    lv_obj_add_event_cb(btnStartStop, handleButtonClick, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_flag(btnStartStop, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(btnStartStop, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_align(btnStartStop, LV_ALIGN_TOP_RIGHT, -10, 105);
    lv_group_add_obj(mainGroup, btnStartStop);

    btnStartStopLabel = lv_label_create(btnStartStop);
    lv_label_set_text(btnStartStopLabel, LV_SYMBOL_PLAY);
    lv_obj_center(btnStartStopLabel);

    btnCalibrate = lv_btn_create(recordScreen);
    lv_obj_add_event_cb(btnCalibrate, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(btnCalibrate, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_align(btnCalibrate, LV_ALIGN_TOP_RIGHT, -10, 155);
    lv_group_add_obj(mainGroup, btnCalibrate);

    btnCalibrateLabel = lv_label_create(btnCalibrate);
    lv_label_set_text(btnCalibrateLabel, "Calibrate");
    lv_obj_center(btnCalibrateLabel);

    // buffer arc
    labelBufferSize = lv_label_create(recordScreen);
    lv_obj_align(labelBufferSize, LV_ALIGN_TOP_LEFT, 10, 105);
    lv_label_set_text(labelBufferSize, "Buffer fill:");

    arcBufferSize = lv_arc_create(recordScreen);
    lv_obj_align(arcBufferSize, LV_ALIGN_BOTTOM_LEFT, 10, -25);
    lv_obj_set_size(arcBufferSize, 80, 80);
    lv_arc_set_rotation(arcBufferSize, 270);
    lv_arc_set_bg_angles(arcBufferSize, 0, 360);
    lv_obj_remove_style(arcBufferSize, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arcBufferSize, LV_OBJ_FLAG_CLICKABLE);

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
            lv_arc_set_value(arcBufferSize, payload.toInt());
        } else if (subject == "buffer/levels") {
            long levels = payload.toInt();
            long startValue = levels / 1000;
            long endValue = levels % 1000;
            lv_arc_set_angles(arcBufferSize, static_cast<uint16_t>(startValue * 3.6), static_cast<uint16_t>(endValue * 3.6));
        }
    }
}