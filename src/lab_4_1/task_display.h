#ifndef LAB_4_1_TASK_DISPLAY_H
#define LAB_4_1_TASK_DISPLAY_H

// Initialise the LCD (call once in setup before starting the scheduler).
void l41_taskDisplayInit();

// Task 3 – Display & Reporting (500 ms)
// Prints a structured report on the LCD (2 rows) and via Serial (printf).
void l41_taskDisplay(void *pvParameters);

#endif // LAB_4_1_TASK_DISPLAY_H