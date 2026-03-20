#include "task_signal_cond.h"
#include "lab_3_2_shared.h"
#include "srv_signal_conditioning/srv_signal_conditioning.h"

void l32_taskSignalCond(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    median_filter_t analog_filter;
    srvSignalConditioningInit(&analog_filter);

    for (;;) {

        // ── Analog: read → process → write ──────────────────────────────
        {
            float raw   = 0.0f;
            bool  valid = false;

            if (xSemaphoreTake(g_l32_mutex_analog, pdMS_TO_TICKS(10)) == pdTRUE) {
                raw   = g_l32_analog.temperature;
                valid = g_l32_analog.valid;
                xSemaphoreGive(g_l32_mutex_analog);
            }

            if (valid) {
                conditioning_result_t r = srvSignalConditioningApply(&analog_filter, raw);

                if (xSemaphoreTake(g_l32_mutex_analog, pdMS_TO_TICKS(10)) == pdTRUE) {
                    g_l32_analog.sat_temperature          = r.saturated;
                    g_l32_analog.med_temperature          = r.median;
                    g_l32_analog.wma_temperature          = r.wma;
                    g_l32_analog.conditioned_temperature  = r.final;
                    xSemaphoreGive(g_l32_mutex_analog);
                }
            }
        }

        // ── Digital: fără filtrare (senzor digital curat) ────────────────────
        // conditioned_temperature = raw (nicio modificare)

        vTaskDelayUntil(&xLastWakeTime, ACQ_ANALOG_PERIOD);
    }
}