#ifndef LAB_3_2_SHARED_H
#define LAB_3_2_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "srv_threshold/srv_threshold.h"

#define TEMP_HIGH_THRESH  26.0f
#define TEMP_LOW_THRESH   24.0f
#define DEBOUNCE_SAMPLES  5

#define ACQ_ANALOG_PERIOD   pdMS_TO_TICKS(50)
#define ACQ_DIGITAL_PERIOD  pdMS_TO_TICKS(2000)
#define THRESHOLD_PERIOD    pdMS_TO_TICKS(50)
#define DISPLAY_PERIOD      pdMS_TO_TICKS(1500)

typedef struct {
    int   raw_adc;
    float temperature;        // raw from sensor
    float sat_temperature;    // after saturation
    float med_temperature;    // after median filter
    float wma_temperature;    // after WMA
    float conditioned_temperature; // final
    bool  valid;
} l32_analog_data_t;

typedef struct {
    float temperature;        // raw from sensor
    float conditioned_temperature; // final (without additional filtering)
    bool  valid;
} l32_digital_data_t;

extern l32_analog_data_t    g_l32_analog;
extern l32_digital_data_t   g_l32_digital;
extern threshold_channel_t  g_l32_thr_analog;
extern threshold_channel_t  g_l32_thr_digital;

extern SemaphoreHandle_t g_l32_mutex_analog;
extern SemaphoreHandle_t g_l32_mutex_digital;
extern SemaphoreHandle_t g_l32_mutex_alert;

#endif // LAB_3_2_SHARED_H