#include "lab_4_1_app.h"
#include "lab_4_1_shared.h"
#include "task_actuator_ctrl.h"
#include "task_signal_cond.h"
#include "task_display.h"
#include "dd_relay/dd_relay.h"
#include "dd_button/dd_button.h"
#include "srv_serial_stdio/srv_serial_stdio.h"

#include <stdio.h>

// ── Global definitions ────────────────────────────────────────────────────────
l41_actuator_state_t g_l41_actuator = {false, false, false, 0, false};
SemaphoreHandle_t    g_l41_mutex    = NULL;

// ── Setup ─────────────────────────────────────────────────────────────────────
void lab4_1Setup() {
    srvSerialSetup();   // needed for printf() in task_display
    ddRelaySetup();     // relay on D10 — controls LED via NO/COM contacts
    ddButtonSetup();    // button on D2 with INPUT_PULLUP

    l41_taskDisplayInit();

    g_l41_mutex = xSemaphoreCreateMutex();

    printf("Lab 4.1 - Binary Actuator Control\n");
    printf("Button: D%d  |  Relay: D%d\n", BUTTON_PIN, RELAY_PIN);
    printf("LED (bulb): connected via relay NO/COM contacts\n");
    printf("Debounce: %d x 50ms = %dms minimum hold\n",
           DEBOUNCE_SAMPLES, DEBOUNCE_SAMPLES * 50);
    printf("Hold button to turn ON, release to turn OFF.\n");
    printf("Tasks: T1=50ms  T2=50ms  T3=500ms\n");

    // Task 1: sole owner of relay, highest priority
    xTaskCreate(l41_taskActuatorCtrl, "T_CTRL", 256, NULL, 3, NULL);
    // Task 2: signal conditioning — detects flutter, publishes stable_confirmed
    xTaskCreate(l41_taskSignalCond,   "T_COND", 256, NULL, 2, NULL);
    // Task 3: display + serial report, lowest priority
    xTaskCreate(l41_taskDisplay,      "T_DISP", 512, NULL, 1, NULL);
}

void lab4_1Loop() {}