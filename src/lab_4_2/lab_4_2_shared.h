#ifndef LAB_4_2_SHARED_H
#define LAB_4_2_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>

// ── Task periods ──────────────────────────────────────────────────────────────
#define MOTOR_CTRL_PERIOD    pdMS_TO_TICKS(50)   // T1: motor control
#define SIGNAL_COND_PERIOD   pdMS_TO_TICKS(50)   // T2: signal conditioning
#define INPUT_PERIOD         pdMS_TO_TICKS(100)  // T3: UART input
#define DISPLAY_PERIOD       pdMS_TO_TICKS(500)  // T4: LCD + Serial report

// ── L298N pin configuration ───────────────────────────────────────────────────
#define MOTOR_ENA_PIN        6    // PWM pin (Timer2 OC2B on Mega) → speed
#define MOTOR_IN1_PIN        4   // direction A
#define MOTOR_IN2_PIN        5   // direction B

// ── Motor limits ──────────────────────────────────────────────────────────────
#define MOTOR_MIN_PCT        0
#define MOTOR_MAX_PCT        100
#define MOTOR_MIN_PWM        0    // 0   → analogWrite 0
#define MOTOR_MAX_PWM        255  // 100% → analogWrite 255

// ── Ramp config ───────────────────────────────────────────────────────────────
// 2 %/tick × 50 ms = 100 ms for 0→10%, 2.5 s for 0→100%
#define RAMP_STEP_PCT        2

// ── Alert thresholds ──────────────────────────────────────────────────────────
#define ALERT_HIGH_PCT       90.0f   // overload if > 90%
#define ALERT_LOW_PCT        85.0f   // returns to normal below 85% (hysteresis)
#define ALERT_DEBOUNCE       4       // consecutive confirmations

// ── Shared state ──────────────────────────────────────────────────────────────
typedef struct {
    float  raw_cmd;          // raw command from UART (0–100 %)
    float  conditioned;      // after saturation + median + WMA
    float  ramped;           // actual position after ramp (0–100 %)
    int    pwm_value;        // PWM value (0–255) sent to ENA
    bool   alert_overload;   // true when conditioned > ALERT_HIGH_PCT
    bool   limit_reached;    // true when ramped == 0 or 100
    int    debounce_count;   // threshold debounce progress
} l42_motor_state_t;

extern l42_motor_state_t  g_l42_motor;
extern SemaphoreHandle_t  g_l42_mutex;

#endif // LAB_4_2_SHARED_H