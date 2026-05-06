#ifndef LAB_5_2_TASK_DISPLAY_H
#define LAB_5_2_TASK_DISPLAY_H

// Initializes the LCD before startScheduler (called from lab5_2Setup).
void l52_taskDisplayInit();

// Task 4 – Display & Reporting (1000 ms)
//
// LCD 2x16:
//   Row 0: "T:24.3C SP:25.0C"
//   Row 1: "Out: +85 H:ON F:--"   (or "F:72%" when the fan is active)
//
// Serial (human-readable):
//   === PID Temp Control ===
//   Temperature  : 24.3 C
//   Set-point    : 25.0 C
//   SP ramp      : 24.8 C
//   PID Kp/Ki/Kd : 3.0 / 0.2 / 1.0
//   PID output   : 85
//   Integral     : 12.3
//   Heater       : ON
//   Fan          : OFF
//   ========================
//
// Serial Plotter (Arduino Serial Plotter):
//   SetPoint:25.0 Value:24.3 Output:85
void l52_taskDisplay(void *pvParameters);

#endif // LAB_5_2_TASK_DISPLAY_H