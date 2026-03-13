#include "task_threshold.h"
#include "lab_3_1_shared.h"
#include "srv_threshold/srv_threshold.h"
#include "dd_led/dd_led.h"

void taskThreshold(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // --- Snapshot sensor temperatures (brief, independent mutex windows) --
        float a_temp  = 0.0f;
        bool  a_valid = false;
        float d_temp  = 0.0f;
        bool  d_valid = false;

        if (xSemaphoreTake(g_mutex_analog, pdMS_TO_TICKS(10)) == pdTRUE) {
            a_temp  = g_analog.temperature;
            a_valid = g_analog.valid;
            xSemaphoreGive(g_mutex_analog);
        }

        if (xSemaphoreTake(g_mutex_digital, pdMS_TO_TICKS(10)) == pdTRUE) {
            d_temp  = g_digital.temperature;
            d_valid = g_digital.valid;
            xSemaphoreGive(g_mutex_digital);
        }

        // --- Delegate conditioning to srv_threshold; mutex protects channel state --
        if (xSemaphoreTake(g_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE) {

            if (a_valid) { srvThresholdUpdate(&g_thr_analog,  a_temp); }
            if (d_valid) { srvThresholdUpdate(&g_thr_digital, d_temp); }

            bool a_alert = srvThresholdIsAlert(&g_thr_analog);
            bool d_alert = srvThresholdIsAlert(&g_thr_digital);

            a_alert ? ddLedTurnOn(LED_GREEN)   : ddLedTurnOff(LED_GREEN);
            d_alert ? ddLedTurnOn(LED_RED) : ddLedTurnOff(LED_RED);

            xSemaphoreGive(g_mutex_alert);
        }

        vTaskDelayUntil(&xLastWakeTime, THRESHOLD_PERIOD);
    }
}
