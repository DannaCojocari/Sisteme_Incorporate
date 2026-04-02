#include "lab_4_2_app.h"
#include "lab_4_2_shared.h"
#include "task_motor_ctrl.h"
#include "task_signal_cond.h"
#include "task_input.h"
#include "task_display.h"
#include "srv_serial_stdio/srv_serial_stdio.h"
#include <stdio.h>

// ── Globale ───────────────────────────────────────────────────────────────────
l42_motor_state_t g_l42_motor = {0.0f, 0.0f, 0.0f, 0, false, false, 0};
SemaphoreHandle_t g_l42_mutex = NULL;

void lab4_2Setup() {
    srvSerialSetup();

    l42_taskDisplayInit();

    g_l42_mutex = xSemaphoreCreateMutex();

    printf("Lab 4.2 - DC Motor Control (L298N)\n");
    printf("ENA=%d  IN1=%d  IN2=%d\n", MOTOR_ENA_PIN, MOTOR_IN1_PIN, MOTOR_IN2_PIN);
    printf("Ramp: %d%%/50ms  Alert: >%d%%  Clear: <%d%%\n",
           RAMP_STEP_PCT, (int)ALERT_HIGH_PCT, (int)ALERT_LOW_PCT);
    printf("Send a number 0-100 + Enter for speed.\n");
    printf("Tasks: T1=50ms T2=50ms T3=100ms T4=500ms\n\n");

    // T1: the only task that writes to hardware — highest priority
    xTaskCreate(l42_taskMotorCtrl,  "T_MOTOR", 256, NULL, 4, NULL);
    // T2: signal conditioning
    xTaskCreate(l42_taskSignalCond, "T_COND",  256, NULL, 3, NULL);
    // T3: UART input
    xTaskCreate(l42_taskInput,      "T_INPUT", 256, NULL, 2, NULL);
    // T4: display — lowest priority
    xTaskCreate(l42_taskDisplay,    "T_DISP",  512, NULL, 1, NULL);
}

void lab4_2Loop() {}