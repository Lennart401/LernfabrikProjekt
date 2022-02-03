#include "ProductionScreen.h"

#include <RPC_internal.h>
#include "ScreenHeading.h"
#include "OnboardingScreen.h"

#define BTN_WIDTH 120
#define BTN_HEIGHT 35

// ---------------------------------------------------------
// screen stuff
static lv_obj_t *productionScreen;

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

static lv_obj_t *labelPredictionDescriptor;
static lv_obj_t *labelActualPrediction;

// ---------------------------------------------------------
// callbacks
static void handleButtonClick(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnBack) {
            onboarding_screen_load();
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (obj == btnStartStop) {
            if ((lv_obj_get_state(btnStartStop) & LV_STATE_CHECKED) != 0) {
                lv_label_set_text(btnStartStopLabel, LV_SYMBOL_PAUSE);
                RPC1.println("SET mode/predicting 1");
            } else {
                lv_label_set_text(btnStartStopLabel, LV_SYMBOL_PLAY);
                RPC1.println("SET mode/predicting 0");
            }
        }
    }
}

// ---------------------------------------------------------
// header function implementations
void production_screen_create(lv_indev_t *_encoderIndev) {
    // init screen
    productionScreen = lv_obj_create(NULL);

    mainGroup = lv_group_create();
    encoderIndev = _encoderIndev;

    // init back button
    static lv_style_t btnBackStyle = screen_heading_get();
    
    btnBack = lv_btn_create(productionScreen);
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

    labelHeading = lv_label_create(productionScreen);
    lv_label_set_text(labelHeading, "Production");
    lv_obj_align(labelHeading, LV_ALIGN_TOP_LEFT, SCREEN_HEADING_LABEL_X, SCREEN_HEADING_LABEL_Y);
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);
    
    // init other ui elements
    // buttons 
    btnStartStop = lv_btn_create(productionScreen);
    lv_obj_add_event_cb(btnStartStop, handleButtonClick, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_flag(btnStartStop, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(btnStartStop, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_align(btnStartStop, LV_ALIGN_TOP_LEFT, SCREEN_CONTENT_START_X, SCREEN_CONTENT_START_Y);
    lv_group_add_obj(mainGroup, btnStartStop);

    btnStartStopLabel = lv_label_create(btnStartStop);
    lv_label_set_text(btnStartStopLabel, LV_SYMBOL_PLAY);
    lv_obj_center(btnStartStopLabel);

    // prediction labels
    labelPredictionDescriptor = lv_label_create(productionScreen);
    lv_label_set_text(labelPredictionDescriptor, "Prediction: ");
    lv_obj_align(labelPredictionDescriptor, LV_ALIGN_TOP_LEFT, 148, SCREEN_CONTENT_START_Y);

    labelActualPrediction = lv_label_create(labelPredictionDescriptor);
    lv_label_set_text(labelActualPrediction, "---");
    lv_obj_align(labelActualPrediction, LV_ALIGN_TOP_LEFT, 0, 30);

    // lastly, add the back button to the group
    lv_group_add_obj(mainGroup, btnBack);
}

void production_screen_load() {
    lv_scr_load(productionScreen);
    lv_indev_set_group(encoderIndev, mainGroup);
}

void production_screen_receive_message(String command, String subject, String payload) {
    if (lv_scr_act() != productionScreen) return;

    // static char *onRamp = "On ramp";
    // static char *readyForPickup = "Ready for pickup";
    // static char *onMovingWagon = "On moving wagon";
    // static char *noMovement = "No movement";
    static char *LUT[9] = {
        "(No detection)",
        "On ramp",
        "Ready for pickup",
        "On moving wagon",
        "No movement",
        "General movement",
        "Throw items into box",
        "Put the box down",
        "Pick the box up"
    };

    if (command == "POST") {
        if (subject == "data-processing/last-prediction") {
            int payloadInt = static_cast<int>(payload.toInt());
            char buffer[32];
            sprintf(buffer, "%d => %s", payloadInt, LUT[payloadInt]);
            //lv_arc_set_value(arcBufferSize, payload.toInt());
            lv_label_set_text(labelActualPrediction, buffer);
        }
    }
}
