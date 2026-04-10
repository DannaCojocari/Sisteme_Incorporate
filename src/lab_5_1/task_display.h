#ifndef LAB_5_1_TASK_DISPLAY_H
#define LAB_5_1_TASK_DISPLAY_H

// Initializes the LCD once, before starting the scheduler.
void l51_taskDisplayInit();

// Task 4 – Display & Reporting (1000 ms)
//
// LCD 2x16:
//   Row 0: "T:24.3C SP:25.0C"
//   Row 1: "Relay:ON  [HIGH]"  /  "Relay:OFF [LOW] "  /  "Relay:OFF [BAND]"
//
// Serial:
//   === Temp Control ===
//   Temperature: 24.3 C
//   Set-point  : 25.0 C
//   Dead-band  : 24.0 .. 26.0 C
//   Relay      : OFF
//   ====================
void l51_taskDisplay(void *pvParameters);

#endif // LAB_5_1_TASK_DISPLAY_H