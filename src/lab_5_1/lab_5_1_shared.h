#ifndef LAB_5_1_SHARED_H
#define LAB_5_1_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>

// ── Task periods ──────────────────────────────────────────────────────────────
#define SENSOR_PERIOD      pdMS_TO_TICKS(500)   // T1: DHT11 acquisition
#define CONTROL_PERIOD     pdMS_TO_TICKS(500)   // T2: ON-OFF control
#define INPUT_PERIOD       pdMS_TO_TICKS(200)   // T3: serial set-point input
#define DISPLAY_PERIOD     pdMS_TO_TICKS(1000)  // T4: LCD + serial report

// ── Default set-point and hysteresis ─────────────────────────────────────────
#define DEFAULT_SETPOINT   25.0f   // °C
#define HYSTERESIS_BAND    1.0f    // ±1°C dead-band around setpoint
                                   // T > SP + HYST  → relay OFF (heater off) + fan ON (30%)
                                   // T < SP - HYST  → relay ON  (heater on)  + fan OFF (0%)
                                   // In dead-band   → hold last actuator state

// ── Set-point limits ──────────────────────────────────────────────────────────
#define SETPOINT_MIN       10.0f
#define SETPOINT_MAX       45.0f

// ── L298N fan motor pin configuration ────────────────────────────────────────
#define FAN_ENA_PIN        6    // PWM speed pin (ENA on L298N)
#define FAN_IN1_PIN        4    // direction pin IN1
#define FAN_IN2_PIN        5    // direction pin IN2
#define FAN_SPEED_PCT      30.0f  // fan runs at 30% when active (ON-OFF control)

// ── Shared state ──────────────────────────────────────────────────────────────
typedef struct {
    float temperature;    // last valid reading from DHT11 (°C)
    float setpoint;       // current set-point (°C)
    bool  relay_on;       // true = relay energised = heater resistor powered
    bool  fan_on;         // true = cooling fan is running via L298N
    bool  sensor_error;   // true when last DHT11 read returned NAN
} l51_state_t;

extern l51_state_t        g_l51;
extern SemaphoreHandle_t  g_l51_mutex;

#endif // LAB_5_1_SHARED_H