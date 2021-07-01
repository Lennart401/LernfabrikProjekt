#include <Arduino.h>
#include <RPC_internal.h>
#include <lvgl.h>
#include <stdint.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <lv_examples.h>
#include <mbed.h>
#include <InterruptIn.h>
#include <RotaryEncoder.h>
#include <rtos.h>

#define TFT_CS 7
#define TFT_DC 6

#define COLOR_BUFFER_SIZE LV_HOR_RES_MAX * 20

#define PIN_A 13
#define PIN_B 14
#define ENCODER_BUTTON A6

static RotaryEncoder encoder(PIN_A, PIN_B, RotaryEncoder::LatchMode::FOUR3);
static mbed::InterruptIn encoderInterruptA(digitalPinToPinName(PIN_A)); // (digitalPinToPinName(PIN_A), PullUp);
static mbed::InterruptIn encoderInterruptB(digitalPinToPinName(PIN_B)); // (digitalPinToPinName(PIN_B), PullUp);
static int encoderLastPosition = 0;

static Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

static lv_disp_buf_t disp_buf;

static lv_color_t buf_1[COLOR_BUFFER_SIZE];
static lv_color_t buf_2[COLOR_BUFFER_SIZE];

static lv_group_t *mainGroup;

static lv_obj_t *btnStart;
static lv_obj_t *btnStartLabel;

static lv_obj_t *btnStop;
static lv_obj_t *btnStopLabel;

static void checkPosition() {
    encoder.tick();
}

static void my_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels(&color_p->full, w * h, true, true);
    tft.endWrite();

    lv_disp_flush_ready(disp_drv);
}

static bool encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    encoder.tick();
    int pos = encoder.getPosition();

    int16_t diff = static_cast<int16_t>(pos) - static_cast<int16_t>(encoderLastPosition);
    data->enc_diff = diff;
    
    if (pos != encoderLastPosition) {
        //Serial.println("Encoder Position: " + String(pos) + " with diff: " + String(diff));
        //RPC1.println("Encoder Position: " + String(pos) + " with diff: " + String(diff));
    }
    
    encoderLastPosition = pos;

    bool isButtonStateLow = (digitalRead(ENCODER_BUTTON) == LOW);

    if (isButtonStateLow) {
        data->state = LV_INDEV_STATE_PR;
        //Serial.println("BUTTON PRESSED");
        //RPC1.println("Button pressed");
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    return false;
}

static void handleButtonClick(lv_obj_t *obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        if (obj == btnStart) {
            RPC1.println("SET mode/running 1");
        } else if (obj == btnStop) {
            RPC1.println("SET mode/running 0");
        }
    }
}

void setup() {
    RPC1.begin();

    // input initialization
    encoderInterruptA.rise(&checkPosition);
    encoderInterruptA.fall(&checkPosition);

    encoderInterruptB.rise(&checkPosition);
    encoderInterruptB.fall(&checkPosition);

    pinMode(ENCODER_BUTTON, INPUT_PULLUP);

    // lvgl stuff
    lv_init();

    // tft initialization
    tft.begin(60000000);
    tft.setRotation(1);

    // display buffer
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, COLOR_BUFFER_SIZE);

    // display driver
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = my_flush_cb;
    //disp_drv.gpu_fill_cb = gpu_fill;
    //disp_drv.gpu_blend_cb = gpu_blend;
    disp_drv.buffer = &disp_buf;
    lv_disp_t *disp;
    disp = lv_disp_drv_register(&disp_drv);

    // encoder driver
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    lv_indev_t *encoderIndev = lv_indev_drv_register(&indev_drv);

    // lvgl init done, setup widgets now
    //label = lv_label_create(lv_scr_act(), NULL);
    //lv_label_set_text(label, "WICHSER!");
    //lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    mainGroup = lv_group_create();
    lv_indev_set_group(encoderIndev, mainGroup);
    
    btnStart = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btnStart, handleButtonClick);
    lv_obj_align(btnStart, NULL, LV_ALIGN_CENTER, 0, -30);
    lv_group_add_obj(mainGroup, btnStart);

    btnStartLabel = lv_label_create(btnStart, NULL);    
    lv_label_set_text(btnStartLabel, "Start");

    btnStop = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btnStop, handleButtonClick);
    lv_obj_align(btnStop, NULL, LV_ALIGN_CENTER, 0, 30);
    lv_group_add_obj(mainGroup, btnStop);

    btnStopLabel = lv_label_create(btnStop, NULL);
    lv_label_set_text(btnStopLabel, "Stop");
    
    //lv_demo_benchmark();
    //lv_demo_keypad_encoder();
}

void loop() {
    lv_task_handler();
    rtos::ThisThread::sleep_for((uint32_t) 1);
}
