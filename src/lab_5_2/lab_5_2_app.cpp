#include "lab_5_2_app.h"
#include "lab_5_2_shared.h"
#include "task_sensor.h"
#include "task_control.h"
#include "task_input.h"
#include "task_display.h"
#include "srv_serial_stdio/srv_serial_stdio.h"

// ── Global definitions (declared extern in shared.h) ──────────────────────
l52_state_t g_l52 = {
    .temperature  = 0.0f,
    .sensor_error = false,

    .setpoint      = DEFAULT_SETPOINT,
    .setpoint_ramp = DEFAULT_SETPOINT,  // ramp starts from setpoint, not from 0

    .kp = DEFAULT_KP,
    .ki = DEFAULT_KI,
    .kd = DEFAULT_KD,

    .integral   = 0.0f,
    .prev_error = 0.0f,
    .output     = 0.0f,
    .last_tick  = 0,   // 0 = "first iteration" — detected in taskControl

    .relay_on = false,
    .fan_on   = false,
    .fan_pct  = 0.0f,
};

SemaphoreHandle_t g_l52_mutex = NULL;

// ── Setup ─────────────────────────────────────────────────────────────────────
void lab5_2Setup() {
    g_l52_mutex = xSemaphoreCreateMutex();

    srvSerialSetup();
    l52_taskDisplayInit();

    // Priorities: Sensor > Control > Input > Display
    // Stacks increased from previous version:
    //   T_SENS : 512  (DHT + isnan)
    //   T_CTRL : 512  (floating-point PID on AVR consumes a lot of stack)
    //   T_INPUT: 384  (snprintf + getchar loop)
    //   T_DISP : 1024 (snprintf x4 + printf x8 + LiquidCrystal)
    xTaskCreate(l52_taskSensor,  "T_SENS",  512,  NULL, 4, NULL);
    xTaskCreate(l52_taskControl, "T_CTRL",  512,  NULL, 3, NULL);
    xTaskCreate(l52_taskInput,   "T_INPUT", 384,  NULL, 2, NULL);
    xTaskCreate(l52_taskDisplay, "T_DISP",  1024, NULL, 1, NULL);
}

void lab5_2Loop() {
    // Intentionally empty: logic is run in FreeRTOS tasks.
}