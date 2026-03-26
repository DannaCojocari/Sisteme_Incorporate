#ifndef LAB_4_2_TASK_DISPLAY_H
#define LAB_4_2_TASK_DISPLAY_H

// Initialise LCD once before the FreeRTOS scheduler starts.
void l42_taskDisplayInit();

// Task 4 – Display & Reporting (500 ms)
// Prints a structured status report on both outputs:
//
//   LCD (2 × 16):
//     Row 0:  "Srv:050% Ang:090"
//     Row 1:  "Cnd:048% [ALERT]"   or "Cnd:048%  OK   "
//
//   Serial (printf):
//     === Servo Report ===
//     Raw cmd    : 50 %
//     Conditioned: 48 %
//     Ramped     : 47 %
//     Angle      : 84 deg
//     Overload   : NO
//     Limit      : NO
//     Debounce   : 0 / 4
//     ====================
void l42_taskDisplay(void *pvParameters);

#endif // LAB_4_2_TASK_DISPLAY_H