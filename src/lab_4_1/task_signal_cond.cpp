#include "task_signal_cond.h"
#include "lab_4_1_shared.h"
#include "srv_threshold/srv_threshold.h"

// ── Threshold channel ─────────────────────────────────────────────────────────
// Monitors the validated actuator state for rapid oscillations (anti-flutter).
// Treats actuator state as float: ON=1.0, OFF=0.0.
// Debounce requires DEBOUNCE_SAMPLES consecutive identical samples before
// confirming a state change — blocks comutări false (flutter).
//
// This task ONLY detects instability and publishes stable_confirmed.
// Task 1 is the sole owner of the relay hardware.
static threshold_channel_t s_thr;

void l41_taskSignalCond(void *pvParameters) {
    (void)pvParameters;

    srvThresholdInit(&s_thr,
                     /*high*/ 0.5f,
                     /*low*/  0.5f,
                     /*debounce*/ DEBOUNCE_SAMPLES);

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        bool validated = false;

        if (xSemaphoreTake(g_l41_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            validated = g_l41_actuator.validated;
            xSemaphoreGive(g_l41_mutex);
        }

        // Feed binary state into threshold channel
        // Saturare: valoarea e deja binara (0 sau 1), clamping implicit
        float value  = validated ? 1.0f : 0.0f;

        // Debouncing software + validare persistenta a starii
        bool stable = srvThresholdUpdate(&s_thr, value);

        // Publish stability result — stable means no flutter detected
        // for DEBOUNCE_SAMPLES consecutive cycles
        if (xSemaphoreTake(g_l41_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l41_actuator.stable_confirmed = stable;
            xSemaphoreGive(g_l41_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, SIGNAL_COND_PERIOD);
    }
}