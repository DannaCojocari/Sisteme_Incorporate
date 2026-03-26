#ifndef LAB_4_1_SHARED_H
#define LAB_4_1_SHARED_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>

// ── Task periods ─────────────────────────────────────────────────────────────
#define ACTUATOR_CTRL_PERIOD   pdMS_TO_TICKS(50)   // Task 1: button read + relay control
#define SIGNAL_COND_PERIOD     pdMS_TO_TICKS(50)   // Task 2: anti-flutter detection only
#define DISPLAY_PERIOD         pdMS_TO_TICKS(500)  // Task 3: LCD + Serial report

// ── Debounce config ───────────────────────────────────────────────────────────
// Button must be held for DEBOUNCE_SAMPLES consecutive 50ms cycles to be valid.
// Presses shorter than DEBOUNCE_SAMPLES * 50ms are treated as noise and ignored.
#define DEBOUNCE_SAMPLES  4   // 4 x 50ms = 200ms minimum hold time

// ── Shared actuator state ─────────────────────────────────────────────────────
typedef struct {
    bool  requested;         // raw button reading this cycle (true = physically pressed)
    bool  validated;         // confirmed actuator state after Task 1 debounce
    bool  cmd_received;      // true when button is currently pressed
    int   confirm_count;     // Task 1 debounce counter (0..DEBOUNCE_SAMPLES)
    bool  stable_confirmed;  // Task 2: true when no flutter detected (threshold stable)
} l41_actuator_state_t;

extern l41_actuator_state_t g_l41_actuator;
extern SemaphoreHandle_t    g_l41_mutex;

#endif // LAB_4_1_SHARED_H