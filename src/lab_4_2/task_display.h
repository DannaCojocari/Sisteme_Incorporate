#ifndef LAB_4_2_TASK_DISPLAY_H
#define LAB_4_2_TASK_DISPLAY_H

// Initializeaza LCD o singura data inainte de scheduler.
void l42_taskDisplayInit();

// Task 4 – Display & Reporting (500 ms)
// LCD 2x16:
//   Row 0: "Mot:050% PWM:127"
//   Row 1: "Cnd:048%  OK    "  sau  "[ALRT]" / "[LIM]"
// Serial:
//   === Motor Report ===
//   Raw cmd    : 50 %
//   Conditioned: 48 %
//   Ramped     : 47 %
//   PWM        : 120 / 255
//   Overload   : NO
//   Limit      : NO
//   ====================
void l42_taskDisplay(void *pvParameters);

#endif // LAB_4_2_TASK_DISPLAY_H