#ifndef PRODUCTION_SCREEN_H_
#define PRODUCTION_SCREEN_H_

#include <Arduino.h>
#include <lvgl.h>

extern "C" {

void production_screen_create(lv_indev_t *encoderIndev);
void production_screen_load();

void production_screen_receive_message(String command, String subject, String payload);

}

#endif // RECORD_SCREEN_H_
