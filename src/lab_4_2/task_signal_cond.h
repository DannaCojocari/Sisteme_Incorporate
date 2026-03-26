#ifndef LAB_4_2_TASK_SIGNAL_COND_H
#define LAB_4_2_TASK_SIGNAL_COND_H

// Task 2 – Signal Conditioning (50 ms)
// Applies the full preprocessing pipeline to the raw keypad command:
//
//   raw_cmd
//     └─ saturation  (clamp to 0–100 %)
//     └─ median filter (size 5, impulse noise rejection)
//     └─ weighted moving average (smooth fluctuations)
//     └─ conditioned  → written to g_l42_servo.conditioned
//
// Also runs the threshold / hysteresis channel (srv_threshold) to detect
// overload (conditioned > ALERT_HIGH_PCT) and publishes the alert flag.
void l42_taskSignalCond(void *pvParameters);

#endif // LAB_4_2_TASK_SIGNAL_COND_H