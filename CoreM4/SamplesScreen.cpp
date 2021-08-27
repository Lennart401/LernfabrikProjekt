#include "SamplesScreen.h"

#include <RPC_internal.h>
#include "ScreenHeading.h"
#include "OnboardingScreen.h"

#define BTN_WIDTH 120
#define BTN_HEIGHT 35

#define BTN_START_RECORDING_DEFAULT_TEXT LV_SYMBOL_PLAY

// ---------------------------------------------------------
// screen stuff
static lv_obj_t *samplesScreen;

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
static lv_obj_t *btnCalibrate;
static lv_obj_t *btnCalibrateLabel;

static lv_obj_t *btnStartRecording;
static lv_obj_t *btnStartRecordingLabel;
static lv_style_t btnStartRecordingWaitStyle;
static lv_style_t btnStartRecordingActiveStyle;
static bool ignoreStartRecordingButtonClicks = false;

static lv_obj_t *rollerSelectSample;

// ---------------------------------------------------------
// callbacks
static void runCountdownTimer(lv_timer_t *timer) {
    static uint8_t state = 3;

    if (state > 0) {
        lv_label_set_text(btnStartRecordingLabel, String(state).c_str());
        state--;
    } else {
        lv_label_set_text(btnStartRecordingLabel, LV_SYMBOL_LOOP);
        lv_obj_remove_style(btnStartRecording, &btnStartRecordingWaitStyle, 0);
        lv_obj_add_style(btnStartRecording, &btnStartRecordingActiveStyle, 0);
        RPC1.println("DO samples/record/start");
    }
}

static void handleButtonClick(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnBack) {
            onboarding_screen_load();
        } else if (obj == btnCalibrate) {
            RPC1.println("DO sensors/calibrate");
        } else if (obj == btnStartRecording && !ignoreStartRecordingButtonClicks) {
            ignoreStartRecordingButtonClicks = true;
            lv_obj_add_style(btnStartRecording, &btnStartRecordingWaitStyle, 0);
            lv_obj_clear_flag(btnStartRecording, LV_OBJ_FLAG_CLICKABLE);
            lv_timer_t *timer = lv_timer_create(runCountdownTimer, 1000, NULL);
            lv_timer_set_repeat_count(timer, 4);
            lv_timer_ready(timer);
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (obj == rollerSelectSample) {
            // the movement-key is specified in protocol v2. since the roller is just displaying all movement types 
            // expect for the first one (which is "none") in the protocol v2 specified order, we can just add 1 to the 
            // index from the roller and have the correct movement type key.
            int movementKey = lv_roller_get_selected(rollerSelectSample) + 1;
            RPC1.println("SET samples/movement-type " + String(movementKey));
        }
    }
}

// ---------------------------------------------------------
// header function implementations
void samples_screen_create(lv_indev_t *_encoderIndev) {
     // init screen
    samplesScreen = lv_obj_create(NULL);

    mainGroup = lv_group_create();
    encoderIndev = _encoderIndev;

    // init back button
    static lv_style_t btnBackStyle = screen_heading_get();
    
    btnBack = lv_btn_create(samplesScreen);
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

    labelHeading = lv_label_create(samplesScreen);
    lv_label_set_text(labelHeading, "Samples");
    lv_obj_align(labelHeading, LV_ALIGN_TOP_LEFT, SCREEN_HEADING_LABEL_X, SCREEN_HEADING_LABEL_Y);
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);

    // buttons
    btnCalibrate = lv_btn_create(samplesScreen);
    lv_obj_add_event_cb(btnCalibrate, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(btnCalibrate, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_align(btnCalibrate, LV_ALIGN_TOP_LEFT, SCREEN_CONTENT_START_X, SCREEN_CONTENT_START_Y);
    lv_group_add_obj(mainGroup, btnCalibrate);

    btnCalibrateLabel = lv_label_create(btnCalibrate);
    lv_label_set_text(btnCalibrateLabel, "Calibrate");
    lv_obj_center(btnCalibrateLabel);

    lv_style_init(&btnStartRecordingWaitStyle);
    lv_style_set_bg_color(&btnStartRecordingWaitStyle, lv_palette_main(LV_PALETTE_RED));

    lv_style_init(&btnStartRecordingActiveStyle);
    lv_style_set_bg_color(&btnStartRecordingActiveStyle, lv_palette_main(LV_PALETTE_GREEN));

    btnStartRecording = lv_btn_create(samplesScreen);
    lv_obj_add_event_cb(btnStartRecording, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(btnStartRecording, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_align(btnStartRecording, LV_ALIGN_TOP_LEFT, SCREEN_CONTENT_START_X, SCREEN_CONTENT_START_Y + 45);
    lv_group_add_obj(mainGroup, btnStartRecording);

    btnStartRecordingLabel = lv_label_create(btnStartRecording);
    lv_label_set_text(btnStartRecordingLabel, BTN_START_RECORDING_DEFAULT_TEXT);
    lv_obj_center(btnStartRecordingLabel);

    // samples roller
    rollerSelectSample = lv_roller_create(samplesScreen);
    // the options are set according the protocol v2 rev 1 standard (leaving out the first option)
    lv_roller_set_options(rollerSelectSample, 
            "On ramp\n"
            "Ready for pickup\n"
            "On moving wagon\n"
            "No movement", LV_ROLLER_MODE_NORMAL);
    lv_obj_add_event_cb(rollerSelectSample, handleButtonClick, LV_EVENT_ALL, NULL);
    lv_obj_align(rollerSelectSample, LV_ALIGN_TOP_RIGHT, -SCREEN_CONTENT_START_X, SCREEN_HEADING_LABEL_Y);
    lv_obj_set_size(rollerSelectSample, 145, 190);
    lv_group_add_obj(mainGroup, rollerSelectSample);

    // lastly, add the back button to the group
    lv_group_add_obj(mainGroup, btnBack);
}

void samples_screen_load() {
    lv_scr_load(samplesScreen);
    lv_indev_set_group(encoderIndev, mainGroup);

    RPC1.println("GET samples/movement-type");
}

void samples_screen_receive_message(String command, String subject, String payload) {
    if (lv_scr_act() != samplesScreen) return;

    if (command == "POST") {
        if (subject == "samples/record/state") {
            if (payload == "DONE") {
                ignoreStartRecordingButtonClicks = false;
                lv_obj_add_flag(btnStartRecording, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_remove_style(btnStartRecording, &btnStartRecordingActiveStyle, 0);
                lv_label_set_text(btnStartRecordingLabel, BTN_START_RECORDING_DEFAULT_TEXT);
            }
        } else if (subject == "samples/movement-type") {
            lv_roller_set_selected(rollerSelectSample, payload.toInt() - 1, LV_ANIM_OFF);
        }
    }
}
