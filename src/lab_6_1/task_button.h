#ifndef LAB_6_1_TASK_BUTTON_H
#define LAB_6_1_TASK_BUTTON_H

// Task 1 – Button Debounce (20 ms)
//
// Reads the button every 20ms and applies debounce by counter:
//   - counter increments when the button is pressed (LOW)
//   - counter decrements when the button is released (HIGH)
//   - counter clamped in [0, DEBOUNCE_COUNT]
//
// When the counter reaches DEBOUNCE_COUNT → stable state = PRESSED
// When the counter reaches 0           → stable state = RELEASED
//
// On each RELEASED→PRESSED transition (debounced rising edge),
// sends g_l61_btn_semaphore to signal task_fsm.
//
// Updates g_l61.button_stable in the shared state.
void l61_taskButton(void *pvParameters);

#endif // LAB_6_1_TASK_BUTTON_H