#ifndef LAB_3_1_SHARED_H
#define LAB_3_1_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "srv_threshold/srv_threshold.h"

// ─── Alert thresholds – hysteresis band 25 °C ± 1 °C ─────────────────────────
#define TEMP_HIGH_THRESH  26.0f   // alert activates  when T > this
#define TEMP_LOW_THRESH   24.0f   // alert deactivates when T < this

// ─── Debounce: consecutive samples required to confirm a state change ─────────
// 5 × 50 ms = 250 ms minimum stable period before alert fires or clears
#define DEBOUNCE_SAMPLES  5

// ─── Task periods ─────────────────────────────────────────────────────────────
#define ACQ_ANALOG_PERIOD   pdMS_TO_TICKS(50)
#define ACQ_DIGITAL_PERIOD  pdMS_TO_TICKS(2000)
#define THRESHOLD_PERIOD    pdMS_TO_TICKS(50)
#define DISPLAY_PERIOD      pdMS_TO_TICKS(1500)

// ─── Shared data types ────────────────────────────────────────────────────────
typedef struct {
    int   raw_adc;
    float temperature;
    bool  valid;
} analog_data_t;

typedef struct {
    float temperature;
    bool  valid;
} digital_data_t;

// ─── Shared globals (defined in lab_3_1_app.cpp) ─────────────────────────────
extern analog_data_t       g_analog;
extern digital_data_t      g_digital;
// One threshold_channel_t per sensor (managed by task_threshold, read by task_display)
extern threshold_channel_t g_thr_analog;
extern threshold_channel_t g_thr_digital;

// ─── Mutexes (defined in lab_3_1_app.cpp) ────────────────────────────────────
extern SemaphoreHandle_t g_mutex_analog;
extern SemaphoreHandle_t g_mutex_digital;
extern SemaphoreHandle_t g_mutex_alert;

#endif // LAB_3_1_SHARED_H

