#ifndef LAB_5_2_TASK_CONTROL_H
#define LAB_5_2_TASK_CONTROL_H

// Task 2 – PID Control (500 ms)
//
// Implements the discrete PID algorithm for temperature control:
//   error      = setpoint_ramp - temperature
//   integral  += error * dt          (anti-windup ±INTEGRAL_CLAMP)
//   derivative = (error - prev_error) / dt
//   output     = Kp*error + Ki*integral + Kd*derivative  (clamped ±255)
//
// Setpoint ramping:
//   setpoint_ramp follows setpoint with max SETPOINT_RAMP_STEP per cycle,
//   avoiding a spike in the derivative component on sudden SP changes.
//
// Integrator reset:
//   Upon detecting a setpoint change (|delta| > 0.05 C), the integrator
//   is reset for a clean PID response.
//
// Actuator action:
//   output > 0  →  relay ON  (heater),  fan OFF
//   output < 0  →  relay OFF, fan proportional to |output| [0..100%]
//   output = 0  →  relay OFF, fan OFF
//
// Does not act if sensor_error == true (safety).
void l52_taskControl(void *pvParameters);

#endif // LAB_5_2_TASK_CONTROL_H