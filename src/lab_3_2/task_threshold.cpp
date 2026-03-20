#include "task_threshold.h"
#include "lab_3_2_shared.h"
#include "srv_threshold/srv_threshold.h"
#include "dd_led/dd_led.h"

void l32_taskThreshold(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        float a_temp  = 0.0f;
        bool  a_valid = false;
        float d_temp  = 0.0f;
        bool  d_valid = false;

        if (xSemaphoreTake(g_l32_mutex_analog, pdMS_TO_TICKS(10)) == pdTRUE) {
            a_temp  = g_l32_analog.conditioned_temperature;
            a_valid = g_l32_analog.valid;
            xSemaphoreGive(g_l32_mutex_analog);
        }

        if (xSemaphoreTake(g_l32_mutex_digital, pdMS_TO_TICKS(10)) == pdTRUE) {
            d_temp  = g_l32_digital.conditioned_temperature;
            d_valid = g_l32_digital.valid;
            xSemaphoreGive(g_l32_mutex_digital);
        }

        if (xSemaphoreTake(g_l32_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE) {

            if (a_valid) { srvThresholdUpdate(&g_l32_thr_analog,  a_temp); }
            if (d_valid) { srvThresholdUpdate(&g_l32_thr_digital, d_temp); }

            bool a_alert = srvThresholdIsAlert(&g_l32_thr_analog);
            bool d_alert = srvThresholdIsAlert(&g_l32_thr_digital);

            // LED_RED = analog (NTC)  |  LED_GREEN = digital (DHT11)
            a_alert ? ddLedTurnOn(LED_RED)   : ddLedTurnOff(LED_RED);
            d_alert ? ddLedTurnOn(LED_GREEN) : ddLedTurnOff(LED_GREEN);

            xSemaphoreGive(g_l32_mutex_alert);
        }

        vTaskDelayUntil(&xLastWakeTime, THRESHOLD_PERIOD);
    }
}