#include "task_servo_ctrl.h"
#include "lab_4_2_shared.h"
#include "dd_servo/dd_servo.h"

// ── Internal state ────────────────────────────────────────────────────────────
static float s_ramped_pct = 0.0f;  // current servo position (ramped)

// ── Public interface ──────────────────────────────────────────────────────────
float actuator_get_speed() {
    return s_ramped_pct;
}

// ── Task ──────────────────────────────────────────────────────────────────────
// Each cycle:
//   1. Read conditioned command from shared state (mutex-protected).
//   2. Apply ramp: move s_ramped_pct toward target by at most RAMP_STEP_PCT.
//   3. Detect limit (ramped == 0 or 100).
//   4. Drive servo hardware.
//   5. Publish updated ramped position and limit flag back to shared state.
void l42_taskServoCtrl(void *pvParameters) {
    (void)pvParameters;

    ddServoSetup(SERVO_PIN);

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // ── 1. Read target ────────────────────────────────────────────────
        float target = 0.0f;
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            target = g_l42_servo.conditioned;
            xSemaphoreGive(g_l42_mutex);
        }

        // ── 2. Ramp toward target ─────────────────────────────────────────
        float diff = target - s_ramped_pct;
        if (diff > RAMP_STEP_PCT) {
            s_ramped_pct += RAMP_STEP_PCT;
        } else if (diff < -RAMP_STEP_PCT) {
            s_ramped_pct -= RAMP_STEP_PCT;
        } else {
            s_ramped_pct = target;   // close enough – snap
        }

        // Clamp to physical limits
        if (s_ramped_pct < (float)SERVO_MIN_PCT) s_ramped_pct = (float)SERVO_MIN_PCT;
        if (s_ramped_pct > (float)SERVO_MAX_PCT) s_ramped_pct = (float)SERVO_MAX_PCT;

        // ── 3. Detect limit ───────────────────────────────────────────────
        bool limit = (s_ramped_pct <= (float)SERVO_MIN_PCT) ||
                     (s_ramped_pct >= (float)SERVO_MAX_PCT);

        // ── 4. Drive hardware ─────────────────────────────────────────────
        ddServoSetPercent(s_ramped_pct);

        // ── 5. Publish ────────────────────────────────────────────────────
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l42_servo.ramped        = s_ramped_pct;
            g_l42_servo.angle         = ddServoGetAngle();
            g_l42_servo.limit_reached = limit;
            xSemaphoreGive(g_l42_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, SERVO_CTRL_PERIOD);
    }
}