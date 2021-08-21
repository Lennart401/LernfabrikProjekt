#include "OnboardingScreen.h"

LV_IMG_DECLARE(leuphana_logo);

static lv_obj_t *onboardingScreen;

static lv_obj_t *imgLeuphanaLogo;
static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

void onboarding_screen_create(lv_indev_t *encoderIndev) {
    // init the screen
    onboardingScreen = lv_obj_create(NULL);

    // init heading
    imgLeuphanaLogo = lv_img_create(onboardingScreen);
    lv_img_set_src(imgLeuphanaLogo, &leuphana_logo);
    lv_obj_align(imgLeuphanaLogo, LV_ALIGN_TOP_LEFT, 50, 50);

    lv_style_init(&labelHeadingStyle);
    lv_style_set_text_font(&labelHeadingStyle, &lv_font_montserrat_26);

    labelHeading = lv_label_create(onboardingScreen);
    lv_label_set_text(labelHeading, "Lernfabrik");
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);
    lv_obj_align(labelHeading, LV_ALIGN_TOP_LEFT, 90, 53);

    // init buttons

}

void onboarding_screen_load() {
    lv_scr_load(onboardingScreen);
}
