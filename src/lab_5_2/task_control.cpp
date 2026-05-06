#include "task_control.h"
#include "lab_5_2_shared.h"
#include "dd_relay/dd_relay.h"
#include "dd_motor/dd_motor.h"

// ── Task 2: PID Control (500 ms) ─────────────────────────────────────────────
//
// Discrete PID algorithm:
//   error      = setpoint_ramp - temperature
//   integral  += error * dt            (with anti-windup ±INTEGRAL_CLAMP)
//   derivative = (error - prev_error) / dt
//   output     = Kp*error + Ki*integral + Kd*derivative  (clamped ±255)
//
//   output > 0  =>  relay ON  (heater),  fan OFF
//   output < 0  =>  relay OFF, fan proportional to |output|  (cooling)
//   output == 0 =>  relay OFF, fan OFF
//
// Setpoint ramping:
//   setpoint_ramp approaches setpoint with max SETPOINT_RAMP_STEP per cycle.
//   Prevents a large spike in the derivative component on a sudden SP change.
//
// Integrator reset:
//   When setpoint_ramp reaches setpoint (the ramp is finished), the integrator's
//   value is kept. But on a new setpoint change (|delta| > 0.05)
//   the integrator is reset to avoid windup accumulated from the
//   previous regime.
//
void l52_taskControl(void *pvParameters) {
    (void)pvParameters;

    // Actuator hardware initialization
    ddRelaySetup();
    ddMotorSetup(FAN_ENA_PIN, FAN_IN1_PIN, FAN_IN2_PIN);
    ddRelayOff();
    ddMotorStop();

    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Store the previous setpoint to detect changes
    float prev_setpoint = DEFAULT_SETPOINT;

    for (;;) {
        // ── 1. Snapshot shared state ──────────────────────────────────────────
        float    temp          = 0.0f;
        float    sp_target     = DEFAULT_SETPOINT;
        float    sp_ramp       = DEFAULT_SETPOINT;
        float    kp            = DEFAULT_KP;
        float    ki            = DEFAULT_KI;
        float    kd            = DEFAULT_KD;
        float    integral      = 0.0f;
        float    prev_error    = 0.0f;
        uint32_t last_tick     = 0;
        bool     err           = true;

        if (xSemaphoreTake(g_l52_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            temp       = g_l52.temperature;
            sp_target  = g_l52.setpoint;
            sp_ramp    = g_l52.setpoint_ramp;
            kp         = g_l52.kp;
            ki         = g_l52.ki;
            kd         = g_l52.kd;
            integral   = g_l52.integral;
            prev_error = g_l52.prev_error;
            last_tick  = g_l52.last_tick;
            err        = g_l52.sensor_error;
            xSemaphoreGive(g_l52_mutex);
        }

        if (!err) {
            // ── 2. Setpoint change detection → reset integrator ─────────────
            // If the user has changed the setpoint by more than 0.05 °C,
            // we reset the integrator to avoid windup accumulated from the
            // previous regime and to allow a clean PID response.
            float sp_delta = sp_target - prev_setpoint;
            if (sp_delta < 0.0f) sp_delta = -sp_delta;
            if (sp_delta > 0.05f) {
                integral   = 0.0f;
                prev_error = 0.0f;
            }
            prev_setpoint = sp_target;

            // ── 3. Setpoint ramping ───────────────────────────────────────────
            // Approach setpoint_ramp to sp_target with the SETPOINT_RAMP_STEP
            // per control cycle, to avoid a spike in the D component.
            float ramp_error = sp_target - sp_ramp;
            if (ramp_error >  SETPOINT_RAMP_STEP) {
                sp_ramp += SETPOINT_RAMP_STEP;
            } else if (ramp_error < -SETPOINT_RAMP_STEP) {
                sp_ramp -= SETPOINT_RAMP_STEP;
            } else {
                sp_ramp = sp_target;  // we have reached the target
            }

            // ── 4. Calculate dt ──────────────────────────────────────────────────
            uint32_t now = xTaskGetTickCount();
            float dt;
            if (last_tick == 0) {
                // First iteration: last_tick has not been set yet, we use
                // the nominal control period as a safe estimate.
                dt = (float)CONTROL_PERIOD / (float)configTICK_RATE_HZ;
            } else {
                dt = (float)(now - last_tick) / (float)configTICK_RATE_HZ;
                if (dt <= 0.0f) {
                    dt = (float)CONTROL_PERIOD / (float)configTICK_RATE_HZ;
                }
            }

            // ── 5. PID Algorithm ───────────────────────────────────────────────
            float error = sp_ramp - temp;

            // Integral component with anti-windup
            integral += error * dt;
            if (integral >  INTEGRAL_CLAMP) integral =  INTEGRAL_CLAMP;
            if (integral < -INTEGRAL_CLAMP) integral = -INTEGRAL_CLAMP;

            // Derivative component
            float derivative = (error - prev_error) / dt;

            // Clamped PID output
            float output = kp * error + ki * integral + kd * derivative;
            if (output >  PID_OUTPUT_MAX) output =  PID_OUTPUT_MAX;
            if (output <  PID_OUTPUT_MIN) output =  PID_OUTPUT_MIN;

            // ── 6. Action on actuators ──────────────────────────────────────
            bool  relay_on = false;
            float fan_pct  = 0.0f;

            if (output > 0.0f) {
                // Temperature below setpoint → heater ON, fan OFF
                ddRelayOn();
                ddMotorStop();
                relay_on = true;
                fan_pct  = 0.0f;
            } else if (output < 0.0f) {
                // Temperature above setpoint → heater OFF,
                // fan proportional to |output|
                ddRelayOff();
                fan_pct = (-output / 255.0f) * 100.0f;
                ddMotorSetPercent(fan_pct);
                relay_on = false;
            } else {
                // Output exactly zero → everything off
                ddRelayOff();
                ddMotorStop();
            }

            // ── 7. Publish state to shared struct ───────────────────────────
            if (xSemaphoreTake(g_l52_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                g_l52.setpoint_ramp = sp_ramp;
                g_l52.integral      = integral;
                g_l52.prev_error    = error;
                g_l52.output        = output;
                g_l52.last_tick     = now;
                g_l52.relay_on      = relay_on;
                g_l52.fan_on        = (fan_pct > 0.0f);
                g_l52.fan_pct       = fan_pct;
                xSemaphoreGive(g_l52_mutex);
            }
        } else {
            // Sensor error → stop actuators for safety
            ddRelayOff();
            ddMotorStop();
        }

        vTaskDelayUntil(&xLastWakeTime, CONTROL_PERIOD);
    }
}