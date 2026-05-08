#ifndef LAB_6_1_FSM_H
#define LAB_6_1_FSM_H

#include <stdint.h>

// ── Moore Node Definition ──────────────────────────────────────────────────
// Moore Automaton: Output = f(CurrentState)
//                 NextState = f(CurrentState, Input)
typedef struct {
    uint8_t  output;        // output associated with the state (0 = LED off, 1 = LED on)
    uint8_t  next[2];       // next[0] = button not pressed, next[1] = button pressed
    const char *name;       // state name for display
} fsm_state_t;

// ── FSM API ───────────────────────────────────────────────────────────────────

// Returns the output for the given state
uint8_t fsmGetOutput(uint8_t state);

// Calculates the next state based on the current state and input
// input: 0 = button not pressed, 1 = button pressed
uint8_t fsmGetNextState(uint8_t current_state, uint8_t input);

// Returns the state name (for display)
const char* fsmGetStateName(uint8_t state);

#endif // LAB_6_1_FSM_H