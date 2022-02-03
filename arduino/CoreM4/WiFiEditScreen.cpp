#include "WiFiEditScreen.h"

#include <RPC_internal.h>
#include "SettingsScreen.h"

// ---------------------------------------------------------
// screen stuff
static lv_obj_t *wifiEditScreen;

static lv_indev_t *encoderIndev;
static lv_group_t *mainGroup;

// ---------------------------------------------------------
// main elements
static lv_obj_t *taSSID;
static lv_obj_t *taPassphrase;

static lv_obj_t *btnCancel;
static lv_obj_t *btnSave;
static lv_obj_t *restartLabel;

static lv_obj_t *keyboard;
static lv_obj_t *lastObjectBeforeKeyboard;

// ---------------------------------------------------------
// callbacks
static void handleTAEvent(lv_event_t * e) {
    if (lv_scr_act() != wifiEditScreen) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        lastObjectBeforeKeyboard = ta;
        lv_keyboard_set_textarea(keyboard, ta);
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(keyboard);
        lv_group_set_editing(mainGroup, true);
    }

    if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        if (lastObjectBeforeKeyboard != NULL) {
            lv_group_focus_obj(lastObjectBeforeKeyboard);
        } else {
            lv_group_focus_obj(taSSID);
        }
    }
}

static void handleButtonClick(lv_event_t *event) {
    if (lv_scr_act() != wifiEditScreen) return;

    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);

    if (code == LV_EVENT_CLICKED) {
        if (obj == btnCancel) {
            settings_screen_load();
        }
        else if (obj == btnSave) {
            char buffer1[55];
            sprintf(buffer1, "SET settings/wifi-ssid %s", lv_textarea_get_text(taSSID));
            RPC1.println(buffer1);

            char buffer2[93];
            sprintf(buffer2, "SET settings/wifi-passphrase %s", lv_textarea_get_text(taPassphrase));
            RPC1.println(buffer2);

            RPC1.println("DO settings/save");

            lv_obj_clear_flag(restartLabel, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// ---------------------------------------------------------
void wifi_edit_screen_create(lv_indev_t *_encoderIndev) {
    // init the screen
    wifiEditScreen = lv_obj_create(NULL);

    encoderIndev = _encoderIndev;
    mainGroup = lv_group_create();

    // textarea SSID 
    taSSID = lv_textarea_create(wifiEditScreen);
    lv_textarea_set_one_line(taSSID, true);
    lv_textarea_set_max_length(taSSID, 32);
    lv_obj_set_width(taSSID, LV_HOR_RES / 2 - 12);
    lv_obj_align(taSSID, LV_ALIGN_TOP_LEFT, 8, 38);
    lv_obj_add_event_cb(taSSID, handleTAEvent, LV_EVENT_ALL, NULL);
    lv_group_add_obj(mainGroup, taSSID);

    lv_obj_t *taSSIDLabel = lv_label_create(wifiEditScreen);
    lv_label_set_text(taSSIDLabel, "SSID:");
    lv_obj_align_to(taSSIDLabel, taSSID, LV_ALIGN_OUT_TOP_LEFT, 11, 0);

    // textarea passphrase
    taPassphrase = lv_textarea_create(wifiEditScreen);
    lv_textarea_set_one_line(taPassphrase, true);
    lv_textarea_set_max_length(taPassphrase, 63);
    lv_obj_set_width(taPassphrase, LV_HOR_RES / 2 - 12);
    lv_obj_align(taPassphrase, LV_ALIGN_TOP_RIGHT, -8, 38);
    lv_obj_add_event_cb(taPassphrase, handleTAEvent, LV_EVENT_ALL, NULL);
    lv_group_add_obj(mainGroup, taPassphrase);

    lv_obj_t *taPassphraseLabel = lv_label_create(wifiEditScreen);
    lv_label_set_text(taPassphraseLabel, "Passphrase:");
    lv_obj_align_to(taPassphraseLabel, taPassphrase, LV_ALIGN_OUT_TOP_LEFT, 11, 0);

    // buttons
    btnSave = lv_btn_create(wifiEditScreen);
    lv_obj_add_event_cb(btnSave, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btnSaveLabel = lv_label_create(btnSave);
    lv_label_set_text(btnSaveLabel, "Save");
    lv_obj_align_to(btnSave, taPassphrase, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 10);

    btnCancel = lv_btn_create(wifiEditScreen);
    lv_obj_add_event_cb(btnCancel, handleButtonClick, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btnCancelLabel = lv_label_create(btnCancel);
    lv_label_set_text(btnCancelLabel, "Cancel");
    lv_obj_align_to(btnCancel, btnSave, LV_ALIGN_OUT_LEFT_TOP, -10, 0);

    lv_group_add_obj(mainGroup, btnCancel);
    lv_group_add_obj(mainGroup, btnSave);

    // restart label
    restartLabel = lv_label_create(wifiEditScreen);
    lv_label_set_text(restartLabel, "Restart to make changes take effect");
    lv_obj_align_to(restartLabel, taSSID, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 11);
    lv_obj_set_width(restartLabel, LV_HOR_RES / 2 - 12);
    lv_obj_add_flag(restartLabel, LV_OBJ_FLAG_HIDDEN);

    // keyboard
    keyboard = lv_keyboard_create(wifiEditScreen);
    lv_obj_set_size(keyboard, LV_HOR_RES, LV_VER_RES / 2 - 10);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_group_add_obj(mainGroup, keyboard);
}

void wifi_edit_screen_load() {
    lv_scr_load(wifiEditScreen);
    lv_indev_set_group(encoderIndev, mainGroup);

    RPC1.println("GET settings/wifi-ssid");
    RPC1.println("GET settings/wifi-passphrase");
}

void wifi_edit_screen_receive_message(String command, String subject, String payload) {
    if (lv_scr_act() != wifiEditScreen) return;

    if (command == "POST") {
        if (subject == "settings/wifi-ssid") {
            lv_textarea_set_text(taSSID, payload.c_str());
        }
        else if (subject == "settings/wifi-passphrase") {
            lv_textarea_set_text(taPassphrase, payload.c_str());
        }
    }
}