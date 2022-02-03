#include "IPAddressScreen.h"

#include <RPC_internal.h>
#include "SettingsScreen.h"
#include "ScreenHeading.h"

// ---------------------------------------------------------
// logic variables
static ADDRESS_MODE addressMode = ADDRESS_MODE::DATA_SERVER;

// ---------------------------------------------------------
// heading
static lv_obj_t *labelHeading;
static lv_style_t labelHeadingStyle;

// ---------------------------------------------------------
// screen stuff
static lv_obj_t *ipAddressScreen;

static lv_indev_t *encoderIndev;
static lv_group_t *mainGroup;

// ---------------------------------------------------------
// main elements
static lv_obj_t *flexBoxIPAddress;
static lv_style_t flexBoxStyle;

static lv_style_t spinboxStyle;

static lv_obj_t *spinboxIP1;
static lv_obj_t *spinboxIP2;
static lv_obj_t *spinboxIP3;
static lv_obj_t *spinboxIP4;
static lv_obj_t *spinboxPort;

static lv_obj_t *btnCancel;
static lv_obj_t *btnSave;

// ---------------------------------------------------------
// helper functions
static lv_obj_t* createIPSpinBox() {
    lv_obj_t *spinbox = lv_spinbox_create(flexBoxIPAddress);
    lv_spinbox_set_digit_format(spinbox, 3, 0);
    lv_spinbox_set_range(spinbox, 0, 255);
    lv_spinbox_set_rollover(spinbox, true);
    lv_spinbox_set_value(spinbox, 0);
    lv_obj_add_style(spinbox, &spinboxStyle, 0);
    lv_obj_set_width(spinbox, LV_SIZE_CONTENT);
    lv_group_add_obj(mainGroup, spinbox);
    return spinbox;
}

// ---------------------------------------------------------
// callbacks
static void handleButtonClick(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnCancel) {
            settings_screen_load();
        }
        else if (obj == btnSave) {
            // ip Address
            uint32_t ipAddress = 0;
            ipAddress |=  lv_spinbox_get_value(spinboxIP1) & 0xFF;
            ipAddress |= (lv_spinbox_get_value(spinboxIP2) & 0xFF) <<  8;
            ipAddress |= (lv_spinbox_get_value(spinboxIP3) & 0xFF) << 16;
            ipAddress |= (lv_spinbox_get_value(spinboxIP4) & 0xFF) << 24;

            // port
            uint16_t port = lv_spinbox_get_value(spinboxPort) & 0xFFFF;

            if (addressMode == ADDRESS_MODE::DATA_SERVER) {
                char buffer1[43];
                sprintf(buffer1, "SET settings/data-server-address %u", ipAddress);
                RPC1.println(buffer1);

                char buffer2[35];
                sprintf(buffer2, "SET settings/data-server-port %u", port);
                RPC1.println(buffer2);

                RPC1.println("DO settings/save");
            }
            else if (addressMode == ADDRESS_MODE::BROKER) {
                char buffer1[38];
                sprintf(buffer1, "SET settings/broker-address %u", ipAddress);
                RPC1.println(buffer1);

                char buffer2[30];
                sprintf(buffer2, "SET settings/broker-port %u", port);
                RPC1.println(buffer2);

                RPC1.println("DO settings/save");
            }

            settings_screen_load();
        }
    }
}

// ---------------------------------------------------------
void ip_address_screen_create(lv_indev_t *_encoderIndev) {
    // init the screen
    ipAddressScreen = lv_obj_create(NULL);

    encoderIndev = _encoderIndev;
    mainGroup = lv_group_create();

    // heading
    lv_style_init(&labelHeadingStyle);
    lv_style_set_text_font(&labelHeadingStyle, &lv_font_montserrat_26);

    labelHeading = lv_label_create(ipAddressScreen);
    lv_label_set_text(labelHeading, "Edit IP/Port");
    lv_obj_align(labelHeading, LV_ALIGN_TOP_MID, 0, SCREEN_HEADING_LABEL_Y);
    lv_obj_add_style(labelHeading, &labelHeadingStyle, 0);

    // init flexbox
    lv_style_init(&flexBoxStyle);
    lv_style_set_bg_opa(&flexBoxStyle, LV_OPA_TRANSP);
    lv_style_set_border_opa(&flexBoxStyle, LV_OPA_TRANSP);
    lv_style_set_pad_column(&flexBoxStyle, 5);

    flexBoxIPAddress = lv_obj_create(ipAddressScreen);
    lv_obj_set_size(flexBoxIPAddress, 320, LV_SIZE_CONTENT);
    lv_obj_align(flexBoxIPAddress, LV_ALIGN_TOP_MID, 0, SCREEN_CONTENT_START_Y - 15);
    lv_obj_add_style(flexBoxIPAddress, &flexBoxStyle, 0);
    lv_obj_set_flex_flow(flexBoxIPAddress, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(flexBoxIPAddress, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_style_init(&spinboxStyle);
    lv_style_set_pad_hor(&spinboxStyle, 6);

    // ip address spinboxes
    spinboxIP1 = createIPSpinBox();
    lv_obj_t *label1 = lv_label_create(flexBoxIPAddress);
    lv_label_set_text(label1, ".");

    spinboxIP2 = createIPSpinBox();
    lv_obj_t *label2 = lv_label_create(flexBoxIPAddress);
    lv_label_set_text(label2, ".");

    spinboxIP3 = createIPSpinBox();
    lv_obj_t *label3 = lv_label_create(flexBoxIPAddress);
    lv_label_set_text(label3, ".");

    spinboxIP4 = createIPSpinBox();

    // port spinbox
    lv_obj_t *label4 = lv_label_create(flexBoxIPAddress);
    lv_label_set_text(label4, ":");

    spinboxPort = lv_spinbox_create(flexBoxIPAddress);
    lv_spinbox_set_digit_format(spinboxPort, 5, 0);
    lv_spinbox_set_range(spinboxPort, 0, 65535);
    lv_spinbox_set_rollover(spinboxPort, true);
    lv_spinbox_set_value(spinboxPort, 0);
    lv_obj_add_style(spinboxPort, &spinboxStyle, 0);
    lv_obj_set_width(spinboxPort, LV_SIZE_CONTENT);
    lv_group_add_obj(mainGroup, spinboxPort);

    // buttons
    btnCancel = lv_btn_create(ipAddressScreen);
    lv_obj_align(btnCancel, LV_ALIGN_TOP_MID, -50, SCREEN_CONTENT_START_Y + 58);
    lv_obj_set_width(btnCancel, 90);
    lv_obj_add_event_cb(btnCancel, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(mainGroup, btnCancel);
    lv_obj_t *btnCancelLabel = lv_label_create(btnCancel);
    lv_obj_center(btnCancelLabel);
    lv_label_set_text(btnCancelLabel, "Cancel");

    btnSave = lv_btn_create(ipAddressScreen);
    lv_obj_align(btnSave, LV_ALIGN_TOP_MID, 50, SCREEN_CONTENT_START_Y + 58);
    lv_obj_set_width(btnSave, 90);
    lv_obj_add_event_cb(btnSave, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(mainGroup, btnSave);
    lv_obj_t *btnSaveLabel = lv_label_create(btnSave);
    lv_obj_center(btnSaveLabel);
    lv_label_set_text(btnSaveLabel, "Save");
}

void ip_address_screen_load() {
    lv_scr_load(ipAddressScreen);
    lv_indev_set_group(encoderIndev, mainGroup);

    if (addressMode == ADDRESS_MODE::DATA_SERVER) {
        RPC1.println("GET settings/data-server-address");
        RPC1.println("GET settings/data-server-port");
    }
    else if (addressMode == ADDRESS_MODE::BROKER) {
        RPC1.println("GET settings/broker-address");
        RPC1.println("GET settings/broker-port");
    }
}

void ip_address_screen_receive_message(String command, String subject, String payload) {
    if (lv_scr_act() != ipAddressScreen) return;

    if (command == "POST") {
        if (subject == "settings/data-server-address" || subject == "settings/broker-address") {
            uint32_t ipAddress = payload.toInt() & 0xFFFFFFFF;

            lv_spinbox_set_value(spinboxIP1,  ipAddress        & 0xFF);
            lv_spinbox_set_value(spinboxIP2, (ipAddress >>  8) & 0xFF);
            lv_spinbox_set_value(spinboxIP3, (ipAddress >> 16) & 0xFF);
            lv_spinbox_set_value(spinboxIP4,  ipAddress >> 24        );
        }
        else if (subject == "settings/data-server-port" || subject == "settings/broker-port") {
            lv_spinbox_set_value(spinboxPort, payload.toInt() & 0xFFFF);
        }
    }
}

void ip_address_screen_set_mode(ADDRESS_MODE mode) {
    addressMode = mode;
}
