#include "task_signal_cond.h"
#include "lab_4_2_shared.h"
#include "srv_signal_conditioning/srv_signal_conditioning.h"
#include "srv_threshold/srv_threshold.h"

static median_filter_t     s_filter;
static threshold_channel_t s_thr;

void l42_taskSignalCond(void *pvParameters) {
    (void)pvParameters;

    // Initialize the filter with the initial value 0
    // to avoid attenuating the first commands
    srvSignalConditioningInit(&s_filter);
    // Pre-fill the buffer with 0 → first commands are not averaged with
    // undefined values; the filter converges immediately to the first real command.
    for (int i = 0; i < MEDIAN_FILTER_SIZE; i++) {
        srvSignalConditioningApply(&s_filter, 0.0f);
    }

    srvThresholdInit(&s_thr,
                     ALERT_HIGH_PCT,
                     ALERT_LOW_PCT,
                     ALERT_DEBOUNCE);

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // 1. Read raw command
        float raw = 0.0f;
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            raw = g_l42_motor.raw_cmd;
            xSemaphoreGive(g_l42_mutex);
        }

        // 2. Pipeline: saturation → median → WMA
        // srv_signal_conditioning saturates at [-20, 100];
        // manually clamp to [0, 100] beforehand for DC motor.
        if (raw < 0.0f)   raw = 0.0f;
        if (raw > 100.0f) raw = 100.0f;

        conditioning_result_t res = srvSignalConditioningApply(&s_filter, raw);

        float conditioned = res.final;
        if (conditioned < 0.0f)   conditioned = 0.0f;
        if (conditioned > 100.0f) conditioned = 100.0f;

        // 3. Threshold / hysteresis → overload alert
        bool alert = srvThresholdUpdate(&s_thr, conditioned);
        int  db    = srvThresholdDebounceCount(&s_thr);

        // 4. Publish
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l42_motor.conditioned    = conditioned;
            g_l42_motor.alert_overload = alert;
            g_l42_motor.debounce_count = db;
            xSemaphoreGive(g_l42_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, SIGNAL_COND_PERIOD);
    }
}