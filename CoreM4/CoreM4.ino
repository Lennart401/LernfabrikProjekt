#include <Arduino.h>
#include <RPC_internal.h>

#include <Adafruit_ILI9341.h>
#include <lvgl.h>

#include <InterruptIn.h>
#include <RotaryEncoder.h>
#include <Mutex.h>
#include <rtos.h>

// ---------------------------------------------------------
// defines
#define TFT_CS_PIN 7
#define TFT_DC_PIN 6

#define COLOR_BUFFER_SIZE LV_HOR_RES_MAX * 20

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
    rtos::ThisThread::sleep_for(1);
}

static void checkPosition() {
    encoder.tick();
}

// ---------------------------------------------------------
// display variables
static Adafruit_ILI9341 tft(TFT_CS_PIN, TFT_DC_PIN);

static lv_disp_buf_t disp_buf;

static lv_color_t buf_1[COLOR_BUFFER_SIZE];
static lv_color_t buf_2[COLOR_BUFFER_SIZE];

// ---------------------------------------------------------
// UI variables (temp)
static lv_group_t *mainGroup;

static lv_obj_t *btnStart;
static lv_obj_t *btnStartLabel;

static lv_obj_t *btnStop;
static lv_obj_t *btnStopLabel;

static lv_obj_t *btnCalibrate;
static lv_obj_t *btnCalibrateLabel;

static lv_obj_t *labelWiFi;
static lv_obj_t *labelWiFiStatus;

static lv_obj_t *labelBufferSize;
static lv_obj_t *barBufferSize;

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

static bool encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
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

    return false;
}

// ---------------------------------------------------------
// UI callbacks (temp)
static void handleButtonClick(lv_obj_t *obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        if (obj == btnStart) {
            RPC1.println("SET mode/running 1");
        } else if (obj == btnStop) {
            RPC1.println("SET mode/running 0");
        } else if (obj == btnCalibrate) {
            RPC1.println("DO sensors/calibrate");
        }
    }
}

// ---------------------------------------------------------
// extra threads
rtos::Thread m4RPCReceiverThread(osPriorityNormal);

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
                        if (payload == "connecting") {
                            lvglMutex.lock();
                            lv_label_set_text(labelWiFiStatus, "#ffc830 Connecting...");
                            lvglMutex.unlock();
                        } else if (payload == "OK") {
                            lvglMutex.lock();
                            lv_label_set_text(labelWiFiStatus, "#16a616 OK");
                            lvglMutex.unlock();
                        } else if (payload == "error") {
                            lvglMutex.lock();
                            lv_label_set_text(labelWiFiStatus, "#ff0000 Error");
                            lvglMutex.unlock();
                        } else {
                            lvglMutex.lock();
                            lv_label_set_text(labelWiFiStatus, payload.c_str());
                            lvglMutex.unlock();
                        }
                    }

                    if (subject == "buffer/fill") {
                        lv_bar_set_value(barBufferSize, payload.toInt(), LV_ANIM_ON);
                    }
                }

                bufferString = "";
            }
        }
        rtos::ThisThread::sleep_for((uint32_t) 50);
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

    // top layer ui initialization
    // status labels
    labelWiFi = lv_label_create(lv_layer_top(), NULL);
    lv_obj_align(labelWiFi, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_label_set_text(labelWiFi, "WiFi:");

    labelWiFiStatus = lv_label_create(lv_layer_top(), NULL);
    lv_obj_align(labelWiFiStatus, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 0);
    lv_label_set_recolor(labelWiFiStatus, true);
    lv_label_set_text(labelWiFiStatus, "#4d4d4d unknown");

    // ui initialization (temp)
    // buttons 
    mainGroup = lv_group_create();
    lv_indev_set_group(encoderIndev, mainGroup);
    
    btnStart = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btnStart, handleButtonClick);
    lv_obj_align(btnStart, NULL, LV_ALIGN_IN_RIGHT_MID, -10, -55);
    lv_group_add_obj(mainGroup, btnStart);

    btnStartLabel = lv_label_create(btnStart, NULL);    
    lv_label_set_text(btnStartLabel, "Start");

    btnStop = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btnStop, handleButtonClick);
    lv_obj_align(btnStop, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 0);
    lv_group_add_obj(mainGroup, btnStop);

    btnStopLabel = lv_label_create(btnStop, NULL);
    lv_label_set_text(btnStopLabel, "Stop");

    btnCalibrate = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btnCalibrate, handleButtonClick);
    lv_obj_align(btnCalibrate, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 55);
    lv_group_add_obj(mainGroup, btnCalibrate);

    btnCalibrateLabel = lv_label_create(btnCalibrate, NULL);
    lv_label_set_text(btnCalibrateLabel, "Calibrate");

    

    // buffer bar
    labelBufferSize = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(labelBufferSize, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -35);
    lv_label_set_text(labelBufferSize, "Buffer fill:");

    barBufferSize = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(barBufferSize, 150, 30);
    lv_obj_align(barBufferSize, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_bar_set_anim_time(barBufferSize, 1000);
    lv_bar_set_value(barBufferSize, 0, LV_ANIM_ON);

    // RPC comm
    m4RPCReceiverThread.start(runM4RPCReceiver);
}

void loop() {
    lvglMutex.lock();
    lv_task_handler();
    lvglMutex.unlock();
    rtos::ThisThread::sleep_for((uint32_t) 5);
}