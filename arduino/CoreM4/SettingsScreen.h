#ifndef SETTINGS_SCREEN_H_
#define SETTINGS_SCREEN_H_

#include <Arduino.h>
#include <lvgl.h>

extern "C" {

void settings_screen_create(lv_indev_t *encoderIndev);
void settings_screen_load();

void settings_screen_receive_message(String command, String subject, String payload);

}

#endif // SETTINGS_SCREEN_H_