#include "lab_3_2_app.h"
#include "lab_3_2_shared.h"
#include "srv_threshold/srv_threshold.h"
#include "task_acq.h"
#include "task_signal_cond.h"
#include "task_threshold.h"
#include "task_display.h"
#include "srv_serial_stdio/srv_serial_stdio.h"
#include "dd_led/dd_led.h"
#include "dd_ntc/dd_ntc.h"
#include "dd_dht11/dd_dht11.h"

#include <string.h>
#include <stdio.h>

l32_analog_data_t   g_l32_analog;
l32_digital_data_t  g_l32_digital;
threshold_channel_t g_l32_thr_analog;
threshold_channel_t g_l32_thr_digital;

SemaphoreHandle_t g_l32_mutex_analog;
SemaphoreHandle_t g_l32_mutex_digital;
SemaphoreHandle_t g_l32_mutex_alert;

void lab3_2Setup() {
    srvSerialSetup();
    ddLedSetup();
    ddNtcSetup();
    ddDht11Setup();
    l32_taskDisplayInit();

    g_l32_analog  = {0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false};
    g_l32_digital = {0.0f, 0.0f, false};

    srvThresholdInit(&g_l32_thr_analog,  TEMP_HIGH_THRESH, TEMP_LOW_THRESH, DEBOUNCE_SAMPLES);
    srvThresholdInit(&g_l32_thr_digital, TEMP_HIGH_THRESH, TEMP_LOW_THRESH, DEBOUNCE_SAMPLES);

    g_l32_mutex_analog  = xSemaphoreCreateMutex();
    g_l32_mutex_digital = xSemaphoreCreateMutex();
    g_l32_mutex_alert   = xSemaphoreCreateMutex();

    char s_high[6], s_low[6];
    dtostrf(TEMP_HIGH_THRESH, 4, 1, s_high);
    dtostrf(TEMP_LOW_THRESH,  4, 1, s_low);
    printf("Lab 3.2 - Dual Sensor Temperature Monitor\n");
    printf("Analog: NTC on A%d  |  Digital: DHT11 on pin %d\n", NTC_PIN - A0, DHT11_PIN);
    printf("Thresholds: LOW=%sC  HIGH=%sC  |  Debounce: %dx50ms=%dms\n",
           s_low, s_high, DEBOUNCE_SAMPLES, DEBOUNCE_SAMPLES * 50);
    printf("LEDs: RED=analog  GREEN=digital\n");

    xTaskCreate(l32_taskAcq,        "T_ACQ",  512, NULL, 4, NULL);
    xTaskCreate(l32_taskSignalCond, "T_COND", 512, NULL, 3, NULL);
    xTaskCreate(l32_taskThreshold,  "T_THR",  256, NULL, 2, NULL);
    xTaskCreate(l32_taskDisplay,    "T_DISP", 640, NULL, 1, NULL);
}

void lab3_2Loop() {}