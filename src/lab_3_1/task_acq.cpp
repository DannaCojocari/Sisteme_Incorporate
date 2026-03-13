#include "task_acq.h"
#include "lab_3_1_shared.h"
#include "dd_ntc/dd_ntc.h"
#include "dd_dht11/dd_dht11.h"

#include <math.h>

// DHT22 minimum sampling interval is 2 s.  Use 44 cycles (2200 ms) to stay
// safely clear of the cache boundary and avoid read races.
#define ACQ_DIGITAL_DIV  44u   // 44 x 50 ms = 2200 ms

void taskAcq(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    unsigned int dht_counter = 0;

    for (;;) {
        // --- NTC: read every cycle (50 ms) ------------------------------------
        int   raw  = ddNtcReadRaw();
        float temp = ddNtcReadTemperature();

        if (xSemaphoreTake(g_mutex_analog, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_analog.raw_adc     = raw;
            g_analog.temperature = temp;
            g_analog.valid       = true;
            xSemaphoreGive(g_mutex_analog);
        }

        if (++dht_counter >= ACQ_DIGITAL_DIV) {
            dht_counter = 0;
            float d_temp = ddDht11ReadTemperature();

            if (xSemaphoreTake(g_mutex_digital, pdMS_TO_TICKS(10)) == pdTRUE) {
                if (!isnan(d_temp)) {
                    g_digital.temperature = d_temp;
                    g_digital.valid       = true;
                }
                xSemaphoreGive(g_mutex_digital);
            }
        }

        vTaskDelayUntil(&xLastWakeTime, ACQ_ANALOG_PERIOD);
    }
}
