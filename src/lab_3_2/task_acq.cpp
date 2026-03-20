#include "task_acq.h"
#include "lab_3_2_shared.h"
#include "dd_ntc/dd_ntc.h"
#include "dd_dht11/dd_dht11.h"

#include <math.h>

#define ACQ_DIGITAL_DIV  44u   // 44 x 50 ms = 2200 ms

void l32_taskAcq(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    unsigned int dht_counter = 0;

    for (;;) {
        int   raw  = ddNtcReadRaw();
        float temp = ddNtcReadTemperature();

        if (xSemaphoreTake(g_l32_mutex_analog, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l32_analog.raw_adc     = raw;
            g_l32_analog.temperature = temp;
            g_l32_analog.valid       = true;
            xSemaphoreGive(g_l32_mutex_analog);
        }

        if (++dht_counter >= ACQ_DIGITAL_DIV) {
            dht_counter = 0;
            float d_temp = ddDht11ReadTemperature();

            if (xSemaphoreTake(g_l32_mutex_digital, pdMS_TO_TICKS(10)) == pdTRUE) {
                if (!isnan(d_temp)) {
                    g_l32_digital.temperature             = d_temp;
                    g_l32_digital.conditioned_temperature = d_temp; // digital: passthrough
                    g_l32_digital.valid                   = true;
                }
                xSemaphoreGive(g_l32_mutex_digital);
            }
        }

        vTaskDelayUntil(&xLastWakeTime, ACQ_ANALOG_PERIOD);
    }
}