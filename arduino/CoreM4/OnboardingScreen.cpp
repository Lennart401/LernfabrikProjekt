#include "OnboardingScreen.h"

#include "RecordScreen.h"
#include "SamplesScreen.h"
#include "SettingsScreen.h"
#include "ProductionScreen.h"

#define BTN_WIDTH 120
#define BTN_HEIGHT 35

LV_IMG_DECLARE(leuphana_logo);

// ---------------------------------------------------------
// screen & general objects
static lv_obj_t *onboardingScreen;
static lv_style_t transparentObjectStyle;

static lv_indev_t *encoderIndev;
static lv_group_t *mainGroup;

// ---------------------------------------------------------
// heading
static lv_obj_t *flexBoxHeading;
static lv_obj_t *imgLeuphanaLogo;
static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

// ---------------------------------------------------------
// buttons
static lv_obj_t *flexBoxButtons;

static lv_obj_t *btnRecordPage;
static lv_obj_t *btnRecordPageLabel;

static lv_obj_t *btnProductionPage;
static lv_obj_t *btnProductionPageLabel;

static lv_obj_t *btnSamplesPage;
static lv_obj_t *btnSamplesPageLabel;

static lv_obj_t *btnSettingsPage;
static lv_obj_t *btnSettingsPageLabel;

static void handleButtonClick(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnRecordPage) {
            record_screen_load();
        } else if (obj == btnSamplesPage) {
            samples_screen_load();
        } else if (obj == btnSettingsPage) {
            settings_screen_load();
        } else if (obj == btnProductionPage) {
            production_screen_load();
        }
    }
}

// ---------------------------------------------------------
void onboarding_screen_create(lv_indev_t *_encoderIndev) {
    // init the screen
    onboardingScreen = lv_obj_create(NULL);

    encoderIndev = _encoderIndev;
    mainGroup = lv_group_create();

    // init heading
    lv_style_init(&transparentObjectStyle);
    lv_style_set_bg_opa(&transparentObjectStyle, LV_OPA_TRANSP);
    lv_style_set_border_opa(&transparentObjectStyle, LV_OPA_TRANSP);

    flexBoxHeading = lv_obj_create(onboardingScreen);
    lv_obj_set_size(flexBoxHeading, 320, 120);
    lv_obj_add_style(flexBoxHeading, &transparentObjectStyle, 0);
    lv_obj_set_flex_flow(flexBoxHeading, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(flexBoxHeading, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END);

    imgLeuphanaLogo = lv_img_create(flexBoxHeading);
    lv_img_set_src(imgLeuphanaLogo, &leuphana_logo);
    lv_obj_align(imgLeuphanaLogo, LV_ALIGN_TOP_LEFT, 50, 50);

    lv_style_init(&labelHeadingStyle);
    lv_style_set_text_font(&labelHeadingStyle, &lv_font_montserrat_26);

    labelHeading = lv_label_create(flexBoxHeading);
    lv_label_set_text(labelHeading, "Lernfabrik");
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);
    lv_obj_align(labelHeading, LV_ALIGN_TOP_LEFT, 90, 53);

    // init buttons
    flexBoxButtons = lv_obj_create(onboardingScreen);
    lv_obj_set_size(flexBoxButtons, 320, 120);
    lv_obj_align(flexBoxButtons, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(flexBoxButtons, &transparentObjectStyle, 0);
    lv_obj_set_flex_flow(flexBoxButtons, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(flexBoxButtons, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    btnRecordPage = lv_btn_create(flexBoxButtons);
    lv_group_add_obj(mainGroup, btnRecordPage);
    lv_obj_set_size(btnRecordPage, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_add_event_cb(btnRecordPage, handleButtonClick, LV_EVENT_CLICKED, NULL);
    btnRecordPageLabel = lv_label_create(btnRecordPage);
    lv_label_set_text(btnRecordPageLabel, LV_SYMBOL_STOP " Record");
    lv_obj_center(btnRecordPageLabel);

    btnProductionPage = lv_btn_create(flexBoxButtons);
    lv_group_add_obj(mainGroup, btnProductionPage);
    lv_obj_set_size(btnProductionPage, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_add_event_cb(btnProductionPage, handleButtonClick, LV_EVENT_CLICKED, NULL);
    btnProductionPageLabel = lv_label_create(btnProductionPage);
    lv_label_set_text(btnProductionPageLabel, LV_SYMBOL_POWER " Production");
    lv_obj_center(btnProductionPageLabel);

    btnSamplesPage = lv_btn_create(flexBoxButtons);
    lv_group_add_obj(mainGroup, btnSamplesPage);
    lv_obj_set_size(btnSamplesPage, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_add_event_cb(btnSamplesPage, handleButtonClick, LV_EVENT_CLICKED, NULL);
    btnSamplesPageLabel = lv_label_create(btnSamplesPage);
    lv_label_set_text(btnSamplesPageLabel, LV_SYMBOL_UPLOAD " Samples");
    lv_obj_center(btnSamplesPageLabel);

    btnSettingsPage = lv_btn_create(flexBoxButtons);
    lv_group_add_obj(mainGroup, btnSettingsPage);
    lv_obj_set_size(btnSettingsPage, BTN_WIDTH, BTN_HEIGHT);
    lv_obj_add_event_cb(btnSettingsPage, handleButtonClick, LV_EVENT_CLICKED, NULL);
    btnSettingsPageLabel = lv_label_create(btnSettingsPage);
    lv_label_set_text(btnSettingsPageLabel, LV_SYMBOL_SETTINGS " Settings");
    lv_obj_center(btnSettingsPageLabel);
}

void onboarding_screen_load() {
    lv_scr_load(onboardingScreen);
    lv_indev_set_group(encoderIndev, mainGroup);
}
