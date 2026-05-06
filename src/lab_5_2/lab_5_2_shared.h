#ifndef LAB_5_2_SHARED_H
#define LAB_5_2_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// ── Task periods ──────────────────────────────────────────────────────────────
#define SENSOR_PERIOD      pdMS_TO_TICKS(500)   // T1: DHT11 acquisition (500 ms)
#define CONTROL_PERIOD     pdMS_TO_TICKS(500)   // T2: PID control       (500 ms)
#define INPUT_PERIOD       pdMS_TO_TICKS(200)   // T3: serial input      (200 ms)
#define DISPLAY_PERIOD     pdMS_TO_TICKS(1000)  // T4: LCD + serial      (1000 ms)

// ── Set-point defaults and limits ────────────────────────────────────────────
#define DEFAULT_SETPOINT   25.0f   // °C
#define SETPOINT_MIN       10.0f   // °C
#define SETPOINT_MAX       45.0f   // °C

// ── Ramp (soft set-point change): max step per control cycle ─────────────────
// Avoids a sudden setpoint jump that would produce a large spike in the derivative
// PID. At 0.5 °C / cycle (500 ms) => 1 °C/s maximum ramp speed.
#define SETPOINT_RAMP_STEP  0.5f   // °C per control cycle

// ── L298N / motor fan pin configuration ──────────────────────────────────────
#define FAN_ENA_PIN        6    // PWM speed pin (ENA on L298N)
#define FAN_IN1_PIN        4    // direction pin IN1
#define FAN_IN2_PIN        5    // direction pin IN2

// ── PID controller default parameters ───────────────────────────────────────
// Can be modified at runtime via KP: / KI: / KD: serial commands.
#define DEFAULT_KP         3.0f
#define DEFAULT_KI         0.2f
#define DEFAULT_KD         1.0f

// ── PID output limits and anti-windup ────────────────────────────────────────
#define PID_OUTPUT_MIN    -255.0f
#define PID_OUTPUT_MAX     255.0f
#define INTEGRAL_CLAMP     100.0f   // anti-windup clamp on the integrator

// ── Shared state (protected by g_l52_mutex) ───────────────────────────────────
typedef struct {
    // Sensor
    float    temperature;     // measured temperature [°C]
    bool     sensor_error;    // true if the last DHT reading returned NAN

    // Set-point
    float    setpoint;        // desired setpoint from user [°C]
    float    setpoint_ramp;   // current setpoint after ramping [°C]

    // PID - parameters (configurable at runtime)
    float    kp, ki, kd;

    // PID - internal state
    float    integral;        // integrator sum
    float    prev_error;      // previous iteration's error (for derivative)
    float    output;          // clamped PID output [-255 .. +255]
    uint32_t last_tick;       // tick of the last control iteration (for dt)

    // Actuators
    bool     relay_on;        // state of the relay (heater)
    bool     fan_on;          // state of the fan (DC motor)
    float    fan_pct;         // fan speed [0..100 %]
} l52_state_t;

extern l52_state_t        g_l52;
extern SemaphoreHandle_t  g_l52_mutex;

#endif // LAB_5_2_SHARED_H