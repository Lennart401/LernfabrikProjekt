#ifndef ONBOARDING_SCREEN_H_
#define ONBOARDING_SCREEN_H_

#include <lvgl.h>

extern "C" {

void onboarding_screen_create();
void onboarding_screen_load(lv_indev_t *encoderIndev);

}

#endif // ONBOARDING_SCREEN_H_