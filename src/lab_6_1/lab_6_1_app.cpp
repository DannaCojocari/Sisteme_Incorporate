#include "lab_6_1_app.h"
#include "lab_6_1_shared.h"
#include "task_button.h"
#include "task_fsm.h"
#include "task_display.h"
#include "srv_serial_stdio/srv_serial_stdio.h"

// ── Globals defined here, declared extern in shared.h ───────────────────────
l61_state_t       g_l61            = { FSM_STATE_LED_OFF, false, false, 0 };
SemaphoreHandle_t g_l61_mutex      = NULL;
SemaphoreHandle_t g_l61_btn_semaphore = NULL;

void lab6_1Setup() {
    // 1. Initialize serial STDIO
    srvSerialSetup();

    // 2. Create synchronization primitives
    g_l61_mutex         = xSemaphoreCreateMutex();
    g_l61_btn_semaphore = xSemaphoreCreateBinary();

    // 3. Initialize LCD (before starting the scheduler)
    l61_taskDisplayInit();

    // 4. Create FreeRTOS tasks
    //
    // Justified priorities:
    //   - task_button (3): highest priority — must read
    //     the button promptly to avoid missing short presses
    //   - task_fsm    (2): medium priority — evaluates the FSM when signaled
    //   - task_display(1): low priority — periodic, non-critical reporting
    //
    xTaskCreate(l61_taskButton,  "T_BTN",   128, NULL, 3, NULL);
    xTaskCreate(l61_taskFsm,     "T_FSM",   128, NULL, 2, NULL);
    xTaskCreate(l61_taskDisplay, "T_DISP",  384, NULL, 1, NULL);
}

void lab6_1Loop() {
    // FreeRTOS takes control from setup() — loop() is not used
}