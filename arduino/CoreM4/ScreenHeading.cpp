#include "ScreenHeading.h"

static lv_style_t btnBackStyle;

void screen_heading_init() {
    lv_style_init(&btnBackStyle);
    lv_style_set_bg_opa(&btnBackStyle, LV_OPA_TRANSP);
    lv_style_set_border_opa(&btnBackStyle, LV_OPA_TRANSP);
    lv_style_set_shadow_opa(&btnBackStyle, LV_OPA_TRANSP);
}

lv_style_t screen_heading_get() {
    return btnBackStyle;
}