#ifndef LAB_6_1_SHARED_H
#define LAB_6_1_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>

// ── Task periods ──────────────────────────────────────────────────────────────
#define BUTTON_PERIOD   pdMS_TO_TICKS(20)    // T1: button + debounce (20ms)
#define FSM_PERIOD      pdMS_TO_TICKS(100)   // T2: FSM evaluation + LED (100ms)
#define DISPLAY_PERIOD  pdMS_TO_TICKS(500)   // T3: LCD + Serial report (500ms)

// ── Debounce configuration ────────────────────────────────────────────────────
// The button is considered "stably pressed" after DEBOUNCE_COUNT consecutive readings
// at 20ms/reading → 5 × 20ms = 100ms stabilization time
#define DEBOUNCE_COUNT  5

// ── FSM State IDs ─────────────────────────────────────────────────────────────
#define FSM_STATE_LED_OFF  0
#define FSM_STATE_LED_ON   1
#define FSM_STATE_COUNT    2

// ── LED index (from dd_led: 0=green, 1=red, 2=yellow) ────────────────────────
#define FSM_LED_INDEX  0   // we use the green LED for the ON/OFF state

// ── Shared state ──────────────────────────────────────────────────────────────
typedef struct {
    uint8_t fsm_state;      // current state of the automaton (FSM_STATE_LED_OFF / ON)
    bool    led_on;         // reflects the current output of the FSM
    bool    button_stable;  // debounced state of the button (true = stably pressed)
    uint32_t transitions;   // total number of transitions performed (for reporting)
} l61_state_t;

extern l61_state_t       g_l61;
extern SemaphoreHandle_t g_l61_mutex;

// Binary semaphore: task_button signals task_fsm that a valid press has occurred
extern SemaphoreHandle_t g_l61_btn_semaphore;

#endif // LAB_6_1_SHARED_H