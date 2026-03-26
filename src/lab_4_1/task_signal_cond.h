#ifndef LAB_4_1_TASK_SIGNAL_COND_H
#define LAB_4_1_TASK_SIGNAL_COND_H

// Task 2 – Signal Conditioning (50 ms)
// Validates the actuator state for stability using srv_threshold hysteresis.
// Detects and blocks rapid ON/OFF oscillations (anti-flutter).
void l41_taskSignalCond(void *pvParameters);

#endif // LAB_4_1_TASK_SIGNAL_COND_H