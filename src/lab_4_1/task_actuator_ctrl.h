#ifndef LAB_4_1_TASK_ACTUATOR_H
#define LAB_4_1_TASK_ACTUATOR_H

// Task 1 – Actuator Control (50 ms)
// Reads the physical button, applies debounce, and drives the relay + LEDs.
// This is the SOLE task that controls hardware outputs (relay, LED_GREEN, LED_RED).
void l41_taskActuatorCtrl(void *pvParameters);

// Public interface: returns the current validated (debounced) actuator state.
bool actuator_get_state();

#endif // LAB_4_1_TASK_ACTUATOR_H