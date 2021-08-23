#ifndef SAMPLES_SCREEN_H_
#define SAMPLES_SCREEN_H_

#include <Arduino.h>
#include <lvgl.h>

extern "C" {

void samples_screen_create(lv_indev_t *encoderIndev);
void samples_screen_load();

void samples_screen_receive_message(String command, String subject, String payload);

}

#endif // SAMPLES_SCREEN_H_