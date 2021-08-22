#include "OnboardingScreen.h"

#define BTN_WIDTH 120
#define BTN_HEIGHT 35

LV_IMG_DECLARE(leuphana_logo);

// ---------------------------------------------------------
// screen & general objects
static lv_obj_t *onboardingScreen;
static lv_style_t transparentObjectStyle;

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

// ---------------------------------------------------------
void onboarding_screen_create(lv_indev_t *encoderIndev) {
    // init the screen
    onboardingScreen = lv_obj_create(NULL);

    mainGroup = lv_group_create();
    lv_indev_set_group(encoderIndev, mainGroup);

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
    btnRecordPageLabel = lv_label_create(btnRecordPage);
    lv_label_set_text(btnRecordPageLabel, LV_SYMBOL_STOP " Record");
    lv_obj_center(btnRecordPageLabel);

    btnProductionPage = lv_btn_create(flexBoxButtons);
    lv_group_add_obj(mainGroup, btnProductionPage);
    lv_obj_set_size(btnProductionPage, BTN_WIDTH, BTN_HEIGHT);
    btnProductionPageLabel = lv_label_create(btnProductionPage);
    lv_label_set_text(btnProductionPageLabel, LV_SYMBOL_POWER " Production");
    lv_obj_center(btnProductionPageLabel);

    btnSamplesPage = lv_btn_create(flexBoxButtons);
    lv_group_add_obj(mainGroup, btnSamplesPage);
    lv_obj_set_size(btnSamplesPage, BTN_WIDTH, BTN_HEIGHT);
    btnSamplesPageLabel = lv_label_create(btnSamplesPage);
    lv_label_set_text(btnSamplesPageLabel, LV_SYMBOL_UPLOAD " Samples");
    lv_obj_center(btnSamplesPageLabel);

    btnSettingsPage = lv_btn_create(flexBoxButtons);
    lv_group_add_obj(mainGroup, btnSettingsPage);
    lv_obj_set_size(btnSettingsPage, BTN_WIDTH, BTN_HEIGHT);
    btnSettingsPageLabel = lv_label_create(btnSettingsPage);
    lv_label_set_text(btnSettingsPageLabel, LV_SYMBOL_SETTINGS " Settings");
    lv_obj_center(btnSettingsPageLabel);
}

void onboarding_screen_load() {
    lv_scr_load(onboardingScreen);
}
