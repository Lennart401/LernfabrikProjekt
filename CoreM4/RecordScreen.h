#ifndef RECORD_SCREEN_H_
#define RECORD_SCREEN_H_

#include <Arduino.h>
#include <lvgl.h>

extern "C" {

void record_screen_create();
void record_screen_load(lv_indev_t *encoderIndev);

void record_screen_receive_message(String command, String subject, String payload);

}

#endif // RECORD_SCREEN_H_
