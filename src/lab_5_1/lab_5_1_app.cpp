#include "lab_5_1_app.h"
#include "lab_5_1_shared.h"
#include "task_sensor.h"
#include "task_control.h"
#include "task_input.h"
#include "task_display.h"
#include "srv_serial_stdio/srv_serial_stdio.h"
#include <stdio.h>

// ── Globals ───────────────────────────────────────────────────────────────────
l51_state_t       g_l51       = { 0.0f, DEFAULT_SETPOINT, false, false };
SemaphoreHandle_t g_l51_mutex = NULL;

void lab5_1Setup() {
    g_l51_mutex = xSemaphoreCreateMutex();

    srvSerialSetup();
    l51_taskDisplayInit();

    // FIX: Increased stack for sensor (DHT lib + vTaskDelay consumes a lot)
    xTaskCreate(l51_taskSensor,  "T_SENS",  512, NULL, 4, NULL);
    xTaskCreate(l51_taskControl, "T_CTRL",  256, NULL, 3, NULL);
    xTaskCreate(l51_taskInput,   "T_INPUT", 256, NULL, 2, NULL);
    xTaskCreate(l51_taskDisplay, "T_DISP",  768, NULL, 1, NULL);
}

void lab5_1Loop() {}