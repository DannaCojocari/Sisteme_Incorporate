#ifndef LAB_5_1_TASK_CONTROL_H
#define LAB_5_1_TASK_CONTROL_H

// Task 2 – ON-OFF Relay Control (500 ms)
// Reads temperature and set-point from shared state.
// Applies ON-OFF control with hysteresis:
//   temperature > setpoint + HYSTERESIS_BAND  →  relay ON
//   temperature < setpoint - HYSTERESIS_BAND  →  relay OFF
//   otherwise                                 →  maintains current state
// Does not act if sensor_error == true.
void l51_taskControl(void *pvParameters);

#endif // LAB_5_1_TASK_CONTROL_H