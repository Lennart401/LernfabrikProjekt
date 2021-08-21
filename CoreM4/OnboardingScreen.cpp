#include "OnboardingScreen.h"

LV_IMG_DECLARE(leuphana_logo);

static lv_obj_t *onboardingScreen;
static lv_style_t transparentObjectStyle;

static lv_obj_t *flexBoxHeading;
static lv_obj_t *imgLeuphanaLogo;
static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

void onboarding_screen_create(lv_indev_t *encoderIndev) {
    // init the screen
    onboardingScreen = lv_obj_create(NULL);

    // init heading
    lv_style_init(&transparentObjectStyle);
    lv_style_set_bg_opa(&transparentObjectStyle, LV_OPA_TRANSP);
    lv_style_set_border_opa(&transparentObjectStyle, LV_OPA_TRANSP);

    flexBoxHeading = lv_obj_create(onboardingScreen);
    lv_obj_set_size(flexBoxHeading, 320, 100);
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

    //lv_obj_remove_style_all(flexBoxHeading);

    // init buttons

}

void onboarding_screen_load() {
    lv_scr_load(onboardingScreen);
}
