#ifndef WIFI_EDIT_SCREEN_H_
#define WIFI_EDIT_SCREEN_H_

#include <Arduino.h>
#include <lvgl.h>

extern "C" {

void wifi_edit_screen_create(lv_indev_t *encoderIndev);
void wifi_edit_screen_load();

void wifi_edit_screen_receive_message(String command, String subject, String payload);

}

#endif // WIFI_EDIT_SCREEN_H_