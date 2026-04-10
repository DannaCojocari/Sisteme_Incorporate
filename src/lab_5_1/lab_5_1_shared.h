#ifndef LAB_5_1_SHARED_H
#define LAB_5_1_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>

// ── Task periods ──────────────────────────────────────────────────────────────
#define SENSOR_PERIOD      pdMS_TO_TICKS(500)   // T1: DHT22 acquisition
#define CONTROL_PERIOD     pdMS_TO_TICKS(500)   // T2: ON-OFF relay control
#define INPUT_PERIOD       pdMS_TO_TICKS(200)   // T3: serial set-point input
#define DISPLAY_PERIOD     pdMS_TO_TICKS(1000)  // T4: LCD + serial report

// ── Default set-point and hysteresis ─────────────────────────────────────────
#define DEFAULT_SETPOINT   25.0f   // °C
#define HYSTERESIS_BAND    1.0f    // relay ON  when T > SP + HYST
                                   // relay OFF when T < SP - HYST

// ── Set-point limits ──────────────────────────────────────────────────────────
#define SETPOINT_MIN       10.0f
#define SETPOINT_MAX       45.0f

// ── Shared state ──────────────────────────────────────────────────────────────
typedef struct {
    float temperature;    // last valid reading from DHT22 (°C)
    float setpoint;       // current set-point (°C)
    bool  relay_on;       // current relay state
    bool  sensor_error;   // true when last DHT22 read returned NAN
} l51_state_t;

extern l51_state_t        g_l51;
extern SemaphoreHandle_t  g_l51_mutex;

#endif // LAB_5_1_SHARED_H