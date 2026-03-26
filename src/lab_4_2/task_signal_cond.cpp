#include "task_signal_cond.h"
#include "lab_4_2_shared.h"
#include "srv_threshold/srv_threshold.h"
#include "srv_signal_conditioning/srv_signal_conditioning.h"

static threshold_channel_t  s_thr;
static median_filter_t      s_filter;

void l42_taskSignalCond(void *pvParameters) {
    (void)pvParameters;

    // Initialise threshold channel (overload alert)
    srvThresholdInit(&s_thr,
                     (float)ALERT_HIGH_PCT,
                     (float)ALERT_LOW_PCT,
                     ALERT_DEBOUNCE);

    // Initialise median / WMA filter
    srvSignalConditioningInit(&s_filter);

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // ── 1. Read raw command (mutex) ───────────────────────────────────
        float raw = 0.0f;
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            raw = g_l42_servo.raw_cmd;
            xSemaphoreGive(g_l42_mutex);
        }

        // ── 2. Full conditioning pipeline ─────────────────────────────────
        // srvSignalConditioningApply does internally:
        //   a) saturation  → clamp to 0–100 %
        //   b) median filter (size 5) → impulse noise rejection
        //   c) WMA (size 5, linearly weighted) → smooth fluctuations
        //   d) final = 0.6 * median + 0.4 * wma
        conditioning_result_t res = srvSignalConditioningApply(&s_filter, raw);
        float conditioned = res.final;

        // Clamp final output to physical limits just in case
        if (conditioned < 0.0f)   conditioned = 0.0f;
        if (conditioned > 100.0f) conditioned = 100.0f;

        // ── 3. Threshold / hysteresis → overload alert ────────────────────
        bool alert = srvThresholdUpdate(&s_thr, conditioned);
        int  db    = srvThresholdDebounceCount(&s_thr);

        // ── 4. Publish (mutex) ────────────────────────────────────────────
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l42_servo.conditioned    = conditioned;
            g_l42_servo.alert_overload = alert;
            g_l42_servo.debounce_count = db;
            xSemaphoreGive(g_l42_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, SIGNAL_COND_PERIOD);
    }
}