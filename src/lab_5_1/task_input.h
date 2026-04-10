#ifndef LAB_5_1_TASK_INPUT_H
#define LAB_5_1_TASK_INPUT_H

// Task 3 – Serial Set-Point Input (200 ms)
// Waits for commands via UART in the format:
//   "SP:<value>"  →  ex: "SP:25.5"  sets the set-point to 25.5 C
// Accepted range: SETPOINT_MIN .. SETPOINT_MAX
// Invalid values are rejected with an error message.
void l51_taskInput(void *pvParameters);

#endif // LAB_5_1_TASK_INPUT_H