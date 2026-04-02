#include "task_motor_ctrl.h"
#include "lab_4_2_shared.h"
#include "dd_motor/dd_motor.h"

static float s_ramped_pct = 0.0f;

float actuator_get_speed() {
    return s_ramped_pct;
}

void l42_taskMotorCtrl(void *pvParameters) {
    (void)pvParameters;

    ddMotorSetup(MOTOR_ENA_PIN, MOTOR_IN1_PIN, MOTOR_IN2_PIN);

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // 1. Read conditioned target
        float target = 0.0f;
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            target = g_l42_motor.conditioned;
            xSemaphoreGive(g_l42_mutex);
        }

        // 2. Ramp: max RAMP_STEP_PCT % per cycle
        float diff = target - s_ramped_pct;
        if (diff > (float)RAMP_STEP_PCT) {
            s_ramped_pct += (float)RAMP_STEP_PCT;
        } else if (diff < -(float)RAMP_STEP_PCT) {
            s_ramped_pct -= (float)RAMP_STEP_PCT;
        } else {
            s_ramped_pct = target;
        }

        // 3. Physical clamp
        if (s_ramped_pct < (float)MOTOR_MIN_PCT) s_ramped_pct = (float)MOTOR_MIN_PCT;
        if (s_ramped_pct > (float)MOTOR_MAX_PCT) s_ramped_pct = (float)MOTOR_MAX_PCT;

        // 4. Limit detection
        bool limit = (s_ramped_pct <= (float)MOTOR_MIN_PCT) ||
                     (s_ramped_pct >= (float)MOTOR_MAX_PCT);

        // 5. Actuate hardware
        ddMotorSetPercent(s_ramped_pct);

        // 6. Publish
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l42_motor.ramped        = s_ramped_pct;
            g_l42_motor.pwm_value     = ddMotorGetPwm();
            g_l42_motor.limit_reached = limit;
            xSemaphoreGive(g_l42_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, MOTOR_CTRL_PERIOD);
    }
}