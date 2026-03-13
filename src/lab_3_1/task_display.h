#ifndef TASK_DISPLAY_H
#define TASK_DISPLAY_H

// Initialises the 16x2 LCD I2C display.
// Must be called once from lab3_1Setup() before the scheduler starts.
void taskDisplayInit();

// Task: Display & Reporting
void taskDisplay(void *pvParameters);

#endif // TASK_DISPLAY_H
