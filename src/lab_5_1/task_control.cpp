#include "task_control.h"
#include "lab_5_1_shared.h"
#include "dd_relay/dd_relay.h"

void l51_taskControl(void *pvParameters) {
    (void)pvParameters;

    ddRelaySetup();

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // 1. Read shared state
        float temp = 0.0f;
        float sp   = DEFAULT_SETPOINT;
        bool  err  = false;

        if (xSemaphoreTake(g_l51_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            temp = g_l51.temperature;
            sp   = g_l51.setpoint;
            err  = g_l51.sensor_error;
            xSemaphoreGive(g_l51_mutex);
        }

        // 2. Do not act if the sensor is in error
        if (!err) {
            // 3. ON-OFF control with hysteresis (latching)
            if (temp > sp + HYSTERESIS_BAND) {
                ddRelayOn();
            } else if (temp < sp - HYSTERESIS_BAND) {
                ddRelayOff();
            }
            // In dead-band: the relay maintains its current state
        }

        // 4. FIX: Publish the relay state in a single lock,
        //    combined with the read above — avoids two consecutive mutex take/give
        //    in the same cycle (risk of timeout on the second one)
        if (xSemaphoreTake(g_l51_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            g_l51.relay_on = ddRelayIsOn();
            xSemaphoreGive(g_l51_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, CONTROL_PERIOD);
    }
}