#ifndef SCREEN_HEADING_H_
#define SCREEN_HEADING_H_

#include <lvgl.h>

#define SCREEN_HEADING_BUTTON_X 0
#define SCREEN_HEADING_BUTTON_Y 42
#define SCREEN_HEADING_BUTTON_W 29
#define SCREEN_HEADING_BUTTON_H 29

#define SCREEN_HEADING_LABEL_X 29
#define SCREEN_HEADING_LABEL_Y 40

void screen_heading_init();
lv_style_t screen_heading_get();

#endif // SCREEN_HEADING_H_