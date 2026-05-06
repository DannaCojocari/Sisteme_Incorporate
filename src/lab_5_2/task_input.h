#ifndef LAB_5_2_TASK_INPUT_H
#define LAB_5_2_TASK_INPUT_H

// Task 3 – Serial Input (200 ms)
//
// Waits for commands via UART and updates the shared state under a mutex.
// Accepted commands (case-insensitive):
//
//   SP:<val>  — sets the setpoint   [SETPOINT_MIN .. SETPOINT_MAX °C]
//   KP:<val>  — sets Kp            [0.0 .. 50.0]
//   KI:<val>  — sets Ki            [0.0 .. 10.0]
//   KD:<val>  — sets Kd            [0.0 .. 20.0]
//
// The value can be an integer or have one decimal place (e.g. "25" or "25.5").
// Values outside the range are rejected with an error message.
void l52_taskInput(void *pvParameters);

#endif // LAB_5_2_TASK_INPUT_H