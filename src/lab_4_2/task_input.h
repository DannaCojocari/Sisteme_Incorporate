#ifndef LAB_4_2_TASK_INPUT_H
#define LAB_4_2_TASK_INPUT_H

// Task 3 – Keypad Input (100 ms)
// Reads the 4×4 keypad and converts a single digit key press (0–9)
// to a speed command in percent (0 % … 90 %).
//
// Key mapping:
//   '0' → 0 %
//   '1' → 10 %
//   ...
//   '9' → 90 %
//   '*' → 0 %   (emergency stop)
//   '#' → 100 % (full speed)
//   'A'–'D' → ignored
//
// The resulting raw_cmd is written to g_l42_servo.raw_cmd under mutex.
void l42_taskInput(void *pvParameters);

#endif // LAB_4_2_TASK_INPUT_H