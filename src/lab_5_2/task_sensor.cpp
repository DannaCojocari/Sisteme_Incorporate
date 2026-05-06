#include "task_sensor.h"
#include "lab_5_2_shared.h"
#include "dd_dht11/dd_dht11.h"
#include <math.h>

void l52_taskSensor(void *pvParameters) {
    (void)pvParameters;

    ddDht11Setup();

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        float temp = ddDht11ReadTemperature();  // °C; NAN on error
        bool  err  = isnan(temp);

        if (xSemaphoreTake(g_l52_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            if (!err) g_l52.temperature = temp;  // keep last valid on error
            g_l52.sensor_error = err;
            xSemaphoreGive(g_l52_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, SENSOR_PERIOD);
    }
}