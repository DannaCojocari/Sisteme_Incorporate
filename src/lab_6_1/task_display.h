#ifndef LAB_6_1_TASK_DISPLAY_H
#define LAB_6_1_TASK_DISPLAY_H

// Initializes the LCD once, before starting the scheduler.
void l61_taskDisplayInit();

// Task 3 – Display & Reporting (500 ms)
//
// LCD 2x16:
//   Row 0: "FSM: LED_OFF    "  /  "FSM: LED_ON     "
//   Row 1: "Btn:--  T:   0"  /  "Btn:PR  T:  12"
//
// Serial (human-readable):
//   === FSM Button-LED ===
//   FSM State  : LED_OFF
//   LED        : OFF
//   Button     : RELEASED
//   Transitions: 5
//   ======================
//
// Serial Plotter (Arduino Serial Plotter compatible):
//   State:0 LED:0 Button:0
void l61_taskDisplay(void *pvParameters);

#endif // LAB_6_1_TASK_DISPLAY_H