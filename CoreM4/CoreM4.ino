#include <Arduino.h>
#include <RPC_internal.h>

#include <Adafruit_ILI9341.h>
#include <lvgl.h>

#include <InterruptIn.h>
#include <RotaryEncoder.h>
#include <Mutex.h>
#include <rtos.h>

#include "OnboardingScreen.h"
#include "RecordScreen.h"

// ---------------------------------------------------------
// defines
#define TFT_CS_PIN 7
#define TFT_DC_PIN 6

#define MONITOR_HOR_RES 320
#define MONITOR_VER_RES 240

#define COLOR_BUFFER_SIZE MONITOR_HOR_RES * 20

#define ENCODER_PIN_A 3
#define ENCODER_PIN_B 2
#define ENCODER_BUTTON_PIN A6

// ---------------------------------------------------------
rtos::Mutex lvglMutex;

// ---------------------------------------------------------
// rotary encoder stuff
static RotaryEncoder encoder(ENCODER_PIN_A, ENCODER_PIN_B, RotaryEncoder::LatchMode::FOUR3);
//static mbed::InterruptIn encoderInterruptA(digitalPinToPinName(ENCODER_PIN_A), PullUp); // (digitalPinToPinName(PIN_A), PullUp);
//static mbed::InterruptIn encoderInterruptB(digitalPinToPinName(ENCODER_PIN_B), PullUp); // (digitalPinToPinName(PIN_B), PullUp);
static int encoderLastPosition = 0;
static rtos::Thread encoderCheckThread(osPriorityRealtime);

static void runEncoderCheckThread() {
    encoder.tick();
    rtos::ThisThread::sleep_for((uint32_t) 1);
}

static void checkPosition() {
    encoder.tick();
}

// ---------------------------------------------------------
// display variables
static Adafruit_ILI9341 tft(TFT_CS_PIN, TFT_DC_PIN);

static lv_disp_draw_buf_t disp_buf;

static lv_color_t buf_1[COLOR_BUFFER_SIZE];
static lv_color_t buf_2[COLOR_BUFFER_SIZE];

// ---------------------------------------------------------
// Top layer UI variables
static lv_obj_t *labelWiFi;
static lv_style_t labelWiFiStyle;
static lv_obj_t *labelWiFiStatus;
static lv_style_t labelWiFiStatusStyle;

// ---------------------------------------------------------
// other lvgl variables
static lv_indev_t *encoderIndev;

// ---------------------------------------------------------
// lvgl callbacks
static void my_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels(&color_p->full, w * h, true, true);
    tft.endWrite();

    lv_disp_flush_ready(disp_drv);
}

static void encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    encoder.tick();
    int pos = encoder.getPosition();

    int16_t diff = static_cast<int16_t>(pos) - static_cast<int16_t>(encoderLastPosition);
    data->enc_diff = diff;
    encoderLastPosition = pos;

    bool isButtonStateLow = (digitalRead(ENCODER_BUTTON_PIN) == LOW);
    if (isButtonStateLow) {
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    //return false;
}

// ---------------------------------------------------------
// extra threads
static rtos::Thread m4RPCReceiverThread(osPriorityNormal);

static void runM4RPCReceiver() {
    String bufferString;

    while (true) {
        while (RPC1.available()) {
            char currentChar = RPC1.read();
            if (currentChar != '\n') bufferString += currentChar;
            else {
                //Serial.println("RPC message: '" + bufferString + "'"); 

                int firstDelimiter = bufferString.indexOf(' ');
                String command = bufferString.substring(0, firstDelimiter);
                String subject;
                String payload;

                if (command == "DO") {
                    subject = bufferString.substring(firstDelimiter+1, bufferString.length() - 1);
                } else if (command == "POST") {
                    int secondDelimiter = bufferString.indexOf(' ', firstDelimiter + 1);
                    subject = bufferString.substring(firstDelimiter+1, secondDelimiter);
                    payload = bufferString.substring(secondDelimiter+1, bufferString.length() - 1);
                }

                if (command == "POST") {
                    if (subject == "wifi/status") {

                        lvglMutex.lock();
                        if (payload == "connecting") {
                            //lv_label_set_text(labelWiFiStatus, "#ffc830 Connecting...");
                            lv_label_set_text(labelWiFiStatus, "Connecting...");
                            lv_style_set_bg_color(&labelWiFiStatusStyle, lv_palette_main(LV_PALETTE_YELLOW));
                        } else if (payload == "OK") {
                            //lv_label_set_text(labelWiFiStatus, "#16a616 OK");
                            lv_label_set_text(labelWiFiStatus, "OK");
                            lv_style_set_bg_color(&labelWiFiStatusStyle, lv_palette_main(LV_PALETTE_GREEN));
                        } else if (payload == "error") {
                            //lv_label_set_text(labelWiFiStatus, "#ff0000 Error");
                            lv_label_set_text(labelWiFiStatus, "Error");
                            lv_style_set_bg_color(&labelWiFiStatusStyle, lv_palette_main(LV_PALETTE_RED));
                        } else {
                            lv_label_set_text(labelWiFiStatus, payload.c_str());
                        }
                        lvglMutex.unlock();
                    }
                }

                record_screen_receive_message(command, subject, payload);

                bufferString = "";
            }
        }
        rtos::ThisThread::sleep_for((uint32_t) 50);
    }
}

static rtos::Thread lvTickThread(osPriorityRealtime);

static void runLvTickThread() {
    while (true) {
        lv_tick_inc(5);
        rtos::ThisThread::sleep_for(5);
    }
}

// ---------------------------------------------------------
// arduino functions
void setup() {
    RPC1.begin();

    // input initialization
    // encoderInterruptA.rise(&checkPosition);
    // encoderInterruptA.fall(&checkPosition);
    // encoderInterruptB.rise(&checkPosition);
    // encoderInterruptB.fall(&checkPosition);

    pinMode(ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);

    pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);

    encoderCheckThread.start(runEncoderCheckThread);

    // lvgl initialization
    lv_init();

    // tft initialization
    tft.begin();
    tft.setRotation(1);

    // display buffer
    lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, COLOR_BUFFER_SIZE);

    // display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = my_flush_cb;
    //disp_drv.gpu_fill_cb = gpu_fill;
    //disp_drv.gpu_blend_cb = gpu_blend;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = MONITOR_HOR_RES;
    disp_drv.ver_res = MONITOR_VER_RES;
    lv_disp_t *disp;
    disp = lv_disp_drv_register(&disp_drv);

    // encoder driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    encoderIndev = lv_indev_drv_register(&indev_drv);

    // top layer ui initialization
    // status labels
    lv_style_init(&labelWiFiStyle);
    lv_style_set_bg_color(&labelWiFiStyle, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_opa(&labelWiFiStyle, LV_OPA_50);

    labelWiFi = lv_label_create(lv_layer_top());
    lv_obj_align(labelWiFi, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(labelWiFi, "WiFi:");
    lv_obj_add_style(labelWiFi, &labelWiFiStyle, 0);

    lv_style_init(&labelWiFiStatusStyle);
    lv_style_set_bg_color(&labelWiFiStatusStyle, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_opa(&labelWiFiStatusStyle, LV_OPA_50);

    labelWiFiStatus = lv_label_create(lv_layer_top());
    lv_obj_align(labelWiFiStatus, LV_ALIGN_TOP_LEFT, 36, 0);
    lv_obj_add_style(labelWiFiStatus, &labelWiFiStatusStyle, 0);
    //lv_label_set_recolor(labelWiFiStatus, true);
    //lv_label_set_text(labelWiFiStatus, "#4d4d4d unknown");
    lv_label_set_text(labelWiFiStatus, "unknown");

    // RPC comm
    m4RPCReceiverThread.start(runM4RPCReceiver);

    // init screens
    onboarding_screen_create();
    record_screen_create();

    // start onboarding/record screen
    onboarding_screen_load(encoderIndev);

    lvTickThread.start(runLvTickThread);
}

void loop() {
    lvglMutex.lock();
    lv_task_handler();
    lvglMutex.unlock();
    rtos::ThisThread::sleep_for((uint32_t) 5);
}