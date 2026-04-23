#include "task_control.h"
#include "lab_5_1_shared.h"
#include "dd_relay/dd_relay.h"
#include "dd_motor/dd_motor.h"

// ── Control task ──────────────────────────────────────────────────────────────
void l51_taskControl(void *pvParameters) {
    (void)pvParameters;

    // Initialise heater relay (pin 7, active HIGH)
    ddRelaySetup();

    // Initialise fan motor via L298N driver (same pins as lab_4_2)
    ddMotorSetup(FAN_ENA_PIN, FAN_IN1_PIN, FAN_IN2_PIN);
    ddMotorStop();   // fan off at startup

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // 1. Snapshot shared state
        float temp = 0.0f;
        float sp   = DEFAULT_SETPOINT;
        bool  err  = false;

        if (xSemaphoreTake(g_l51_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            temp = g_l51.temperature;
            sp   = g_l51.setpoint;
            err  = g_l51.sensor_error;
            xSemaphoreGive(g_l51_mutex);
        }

        // 2. Only actuate when sensor data is valid
        if (!err) {
            if (temp > sp + HYSTERESIS_BAND) {
                // Too HOT → cut heater, run fan at full speed
                ddRelayOff();
                ddMotorSetPercent(FAN_SPEED_PCT);
            } else if (temp < sp - HYSTERESIS_BAND) {
                // Too COLD → power heater, stop fan
                ddRelayOn();
                ddMotorStop();
            }
            // Inside dead-band → hold current actuator state (hysteresis)
        }

        // 3. Publish actuator states back to shared struct
        if (xSemaphoreTake(g_l51_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            g_l51.relay_on = ddRelayIsOn();
            g_l51.fan_on   = (ddMotorGetPercent() > 0.0f);
            xSemaphoreGive(g_l51_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, CONTROL_PERIOD);
    }
}