#include "lab_4_2_app.h"
#include "lab_4_2_shared.h"
#include "task_servo_ctrl.h"
#include "task_signal_cond.h"
#include "task_input.h"
#include "task_display.h"
#include "srv_serial_stdio/srv_serial_stdio.h"

#include <stdio.h>

// ── Global definitions ────────────────────────────────────────────────────────
l42_servo_state_t  g_l42_servo  = {0.0f, 0.0f, 0.0f, 0, false, false, 0};
SemaphoreHandle_t  g_l42_mutex  = NULL;

// ── Setup ─────────────────────────────────────────────────────────────────────
void lab4_2Setup() {
    srvSerialSetup();          // printf → Serial

    l42_taskDisplayInit();     // LCD splash screen

    g_l42_mutex = xSemaphoreCreateMutex();

    printf("Lab 4.2 - Analog Actuator Control (Servo)\n");
    printf("Servo pin  : %d\n", SERVO_PIN);
    printf("Ramp step  : %d %% / 50ms\n", RAMP_STEP_PCT);
    printf("Alert high : %d %%   low: %d %%\n",
           (int)ALERT_HIGH_PCT, (int)ALERT_LOW_PCT);
    printf("Keys: 0-9 = 0-90%%   # = 100%%   * = 0%% (stop)\n");
    printf("Tasks: T1=50ms  T2=50ms  T3=100ms  T4=500ms\n\n");

    // T1: servo control (rampare) — ddServoSetup() creează intern T_SERVO_PWM la prio 5
    xTaskCreate(l42_taskServoCtrl,  "T_SERVO", 256, NULL, 4, NULL);
    // T2: signal conditioning – same urgency as servo ctrl
    xTaskCreate(l42_taskSignalCond, "T_COND",  256, NULL, 3, NULL);
    // T3: keypad input – medium priority
    xTaskCreate(l42_taskInput,      "T_INPUT", 256, NULL, 2, NULL);
    // T4: display / reporting – lowest priority
    xTaskCreate(l42_taskDisplay,    "T_DISP",  512, NULL, 1, NULL);
}

void lab4_2Loop() {
    // Empty – FreeRTOS scheduler runs the tasks.
}