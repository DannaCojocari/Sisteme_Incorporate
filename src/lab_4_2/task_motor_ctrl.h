#ifndef LAB_4_2_TASK_MOTOR_CTRL_H
#define LAB_4_2_TASK_MOTOR_CTRL_H

// Task 1 – Motor Control (50 ms)
// Singura sarcina care scrie hardware (dd_motor).
// Citeste g_l42_motor.conditioned si aplica rampa ±RAMP_STEP_PCT/ciclu.
// Publica ramped, pwm_value, limit_reached in shared state.
void l42_taskMotorCtrl(void *pvParameters);

// Interfata publica read-only pentru alte module.
float actuator_get_speed();

#endif // LAB_4_2_TASK_MOTOR_CTRL_H