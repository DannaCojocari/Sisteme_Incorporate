#include "lab_6_1_fsm.h"
#include "lab_6_1_shared.h"  // pentru FSM_STATE_* defines

// ── Moore State Table ────────────────────────────────────────────────────
//
// According to the guide Table 7.1:
//
//  No. | Name     | Out | next[0](in=0) | next[1](in=1)
//  ----+----------+-----+---------------+--------------
//   0  | LED_OFF  |  0  | LED_OFF       | LED_ON
//   1  | LED_ON   |  1  | LED_ON        | LED_OFF
//
// Moore Automaton: Output depends ONLY on the current state.
// Transition depends on the current state + input (button).
//
static const fsm_state_t s_fsm_table[FSM_STATE_COUNT] = {
    // output  next[button=0]     next[button=1]   name
    {  0,     {FSM_STATE_LED_OFF, FSM_STATE_LED_ON},  "LED_OFF" },
    {  1,     {FSM_STATE_LED_ON,  FSM_STATE_LED_OFF}, "LED_ON"  }
};

uint8_t fsmGetOutput(uint8_t state) {
    if (state >= FSM_STATE_COUNT) return 0;
    return s_fsm_table[state].output;
}

uint8_t fsmGetNextState(uint8_t current_state, uint8_t input) {
    if (current_state >= FSM_STATE_COUNT) return FSM_STATE_LED_OFF;
    uint8_t in = (input != 0) ? 1 : 0;
    return s_fsm_table[current_state].next[in];
}

const char* fsmGetStateName(uint8_t state) {
    if (state >= FSM_STATE_COUNT) return "UNKNOWN";
    return s_fsm_table[state].name;
}