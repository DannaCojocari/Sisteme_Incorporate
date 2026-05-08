#include "task_fsm.h"
#include "lab_6_1_shared.h"
#include "lab_6_1_fsm.h"
#include "dd_led/dd_led.h"

void l61_taskFsm(void *pvParameters) {
    (void)pvParameters;

    ddLedSetup();

    // Initial state: LED_OFF
    uint8_t current_state = FSM_STATE_LED_OFF;

    for (;;) {
        // ── Step 1 (Moore): Apply the output of the CURRENT state ────────────────────
        // Output depends ONLY on the current state — Moore characteristic
        uint8_t output = fsmGetOutput(current_state);
        if (output) {
            ddLedTurnOn(FSM_LED_INDEX);
        } else {
            ddLedTurnOff(FSM_LED_INDEX);
        }

        // Publish the current state and output to the shared state
        if (xSemaphoreTake(g_l61_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l61.fsm_state = current_state;
            g_l61.led_on    = (output != 0);
            xSemaphoreGive(g_l61_mutex);
        }

        // ── Step 2: Wait for the event from task_button ───────────────────
        // We block for up to FSM_PERIOD ms waiting for the valid press semaphore.
        // If the timeout expires without a press → input = 0 (button not pressed)
        uint8_t input = 0;
        if (xSemaphoreTake(g_l61_btn_semaphore, FSM_PERIOD) == pdTRUE) {
            input = 1;  // valid press detected by task_button
        }

        // ── Step 3: Read the input (already have it in 'input' from the semaphore) ────
        // input=1 → button stably pressed (confirmed by debounce in task_button)
        // input=0 → no press in this period

        // ── Step 4: Calculate and apply the next state ────────────────────
        uint8_t next_state = fsmGetNextState(current_state, input);

        if (next_state != current_state) {
            // Record the transition in the shared state
            if (xSemaphoreTake(g_l61_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                g_l61.transitions++;
                xSemaphoreGive(g_l61_mutex);
            }
        }

        current_state = next_state;
    }
}