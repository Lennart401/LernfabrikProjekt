#ifndef IP_ADDRESS_SCREEN_H_
#define IP_ADDRESS_SCREEN_H_

#include <Arduino.h>
#include <lvgl.h>

extern "C" {

enum ADDRESS_MODE {
    DATA_SERVER = 0,
    BROKER = 1
};

void ip_address_screen_create(lv_indev_t *encoderIndev);
void ip_address_screen_load();

void ip_address_screen_receive_message(String command, String subject, String payload);
void ip_address_screen_set_mode(ADDRESS_MODE mode);

}

#endif // IP_ADDRESS_SCREEN_H_