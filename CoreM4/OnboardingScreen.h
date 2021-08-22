#ifndef ONBOARDING_SCREEN_H_
#define ONBOARDING_SCREEN_H_

#include <lvgl.h>

extern "C" {

void onboarding_screen_create(lv_indev_t *encoderIndev);
void onboarding_screen_load();

}

#endif // ONBOARDING_SCREEN_H_