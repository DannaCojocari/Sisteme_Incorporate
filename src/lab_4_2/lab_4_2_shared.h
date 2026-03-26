#ifndef LAB_4_2_SHARED_H
#define LAB_4_2_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>

// ── Task periods ──────────────────────────────────────────────────────────────
#define SERVO_CTRL_PERIOD    pdMS_TO_TICKS(50)   // Task 1: servo control (50 ms)
#define SIGNAL_COND_PERIOD   pdMS_TO_TICKS(50)   // Task 2: signal conditioning (50 ms)
#define INPUT_PERIOD         pdMS_TO_TICKS(100)  // Task 3: keypad input (100 ms)
#define DISPLAY_PERIOD       pdMS_TO_TICKS(500)  // Task 4: LCD + Serial report (500 ms)

// ── Servo configuration ───────────────────────────────────────────────────────
#define SERVO_PIN            6       // OC4A — Timer4 Fast PWM, complet liber de timer-api
#define SERVO_MIN_ANGLE      0       // degrees
#define SERVO_MAX_ANGLE      180     // degrees
#define SERVO_MIN_PCT        0       // percent
#define SERVO_MAX_PCT        100     // percent

// ── Ramp configuration ────────────────────────────────────────────────────────
// How many % per 50 ms tick the servo position may change (ramp rate).
// 2 %/tick → full travel (100%) takes 50 ticks = 2.5 s.
#define RAMP_STEP_PCT        2       // max position change per SERVO_CTRL_PERIOD

// ── Debounce / threshold ──────────────────────────────────────────────────────
#define DEBOUNCE_SAMPLES     3       // consecutive identical readings required

// ── Alert thresholds ──────────────────────────────────────────────────────────
#define ALERT_HIGH_PCT       89.0f   // alert triggers when conditioned >= 90%
#define ALERT_LOW_PCT        85.0f   // alert clears below 85% (hysteresis)
#define ALERT_DEBOUNCE       4       // consecutive samples to confirm alert

// ── Shared servo state ────────────────────────────────────────────────────────
typedef struct {
    float  raw_cmd;          // raw value from keypad (0–100 %)
    float  conditioned;      // after saturation + median + WMA
    float  ramped;           // actual servo position after ramp (0–100 %)
    int    angle;            // mapped angle (0–180 deg)
    bool   alert_overload;   // true when conditioned > ALERT_HIGH_PCT
    bool   limit_reached;    // true when ramp is clamped at min/max
    int    debounce_count;   // threshold debounce progress
} l42_servo_state_t;

extern l42_servo_state_t  g_l42_servo;
extern SemaphoreHandle_t  g_l42_mutex;

#endif // LAB_4_2_SHARED_H