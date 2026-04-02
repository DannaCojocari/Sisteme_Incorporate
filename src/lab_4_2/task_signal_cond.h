#ifndef LAB_4_2_TASK_SIGNAL_COND_H
#define LAB_4_2_TASK_SIGNAL_COND_H

// Task 2 – Signal Conditioning (50 ms)
// Pipeline aplicat pe raw_cmd:
//   saturare [0–100%] → filtru median (n=5) → WMA (n=5) → threshold alert
// Rezultatul (conditioned) e scris in g_l42_motor.conditioned.
void l42_taskSignalCond(void *pvParameters);

#endif // LAB_4_2_TASK_SIGNAL_COND_H