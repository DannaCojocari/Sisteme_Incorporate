#ifndef LAB_6_1_TASK_FSM_H
#define LAB_6_1_TASK_FSM_H

// Task 2 – FSM Evaluation + LED Control
//
// Implements the Moore finite state machine loop in 4 steps:
//   1. Apply the output of the current state → turn the LED on/off
//   2. Wait for the input event (the semaphore from task_button)
//   3. Read the input (stable button from shared state)
//   4. Calculate and apply the next state
//
// Uses g_l61_btn_semaphore with FSM_PERIOD timeout to avoid blocking indefinitely.
// Updates g_l61.fsm_state, g_l61.led_on, and g_l61.transitions.
void l61_taskFsm(void *pvParameters);

#endif // LAB_6_1_TASK_FSM_H