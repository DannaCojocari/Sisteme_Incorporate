#include "lab_3_1_app.h"
#include "lab_3_1_shared.h"
#include "srv_threshold/srv_threshold.h"
#include "task_acq.h"
#include "task_threshold.h"
#include "task_display.h"
#include "srv_serial_stdio/srv_serial_stdio.h"
#include "dd_led/dd_led.h"
#include "dd_ntc/dd_ntc.h"
#include "dd_dht11/dd_dht11.h"

#include <string.h>
#include <stdio.h>

// ─── Shared globals (declared extern in lab_3_1_shared.h) ────────────────────
analog_data_t      g_analog;
digital_data_t     g_digital;
threshold_channel_t g_thr_analog;
threshold_channel_t g_thr_digital;

// ─── Mutexes (declared extern in lab_3_1_shared.h) ───────────────────────────
SemaphoreHandle_t g_mutex_analog;
SemaphoreHandle_t g_mutex_digital;
SemaphoreHandle_t g_mutex_alert;

// ─── Setup ────────────────────────────────────────────────────────────────────
void lab3_1Setup() {
    srvSerialSetup();
    ddLedSetup();
    ddNtcSetup();
    ddDht11Setup();
    taskDisplayInit();  // LCD init (owned by task_display module)

    memset(&g_analog,  0, sizeof(g_analog));
    memset(&g_digital, 0, sizeof(g_digital));
    srvThresholdInit(&g_thr_analog,  TEMP_HIGH_THRESH, TEMP_LOW_THRESH, DEBOUNCE_SAMPLES);
    srvThresholdInit(&g_thr_digital, TEMP_HIGH_THRESH, TEMP_LOW_THRESH, DEBOUNCE_SAMPLES);

    g_mutex_analog  = xSemaphoreCreateMutex();
    g_mutex_digital = xSemaphoreCreateMutex();
    g_mutex_alert   = xSemaphoreCreateMutex();

    // AVR printf does not support %f – use dtostrf for float values
    char s_high[6], s_low[6];
    dtostrf(TEMP_HIGH_THRESH, 4, 1, s_high);
    dtostrf(TEMP_LOW_THRESH,  4, 1, s_low);
    printf("Lab 3.1 - Dual Sensor Temperature Monitor\n");
    printf("Analog: NTC  on A%d  |  Digital: DHT11 on pin %d\n",
           NTC_PIN - A0, DHT11_PIN);
    printf("Thresholds: LOW=%sC  HIGH=%sC  |  Debounce: %dx50ms=%dms\n",
           s_low, s_high, DEBOUNCE_SAMPLES, DEBOUNCE_SAMPLES * 50);
    printf("LEDs: RED=analog  GREEN=digital  YELLOW=both\n");

    // Combined acquisition task: NTC every 50 ms, DHT11 every 2000 ms
    xTaskCreate(taskAcq,       "T_ACQ",  512, NULL, 3, NULL);
    xTaskCreate(taskThreshold, "T_THR",  256, NULL, 2, NULL);
    xTaskCreate(taskDisplay,   "T_DISP", 640, NULL, 1, NULL);
}

void lab3_1Loop() {
    // FreeRTOS scheduler owns the CPU; Arduino loop() is effectively idle.
}

