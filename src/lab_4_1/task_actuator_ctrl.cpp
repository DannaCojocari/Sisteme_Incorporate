#include "task_actuator_ctrl.h"
#include "lab_4_1_shared.h"
#include "dd_relay/dd_relay.h"
#include "dd_button/dd_button.h"

// ── Internal state ───────────────────────────────────────────────────────────
static bool s_current_state    = false;
static int  s_debounce_counter = 0;

// ── Public interface ─────────────────────────────────────────────────────────
bool actuator_get_state() {
    return s_current_state;
}

// ── Apply actuator output ────────────────────────────────────────────────────
// The LED (bulb) is connected through relay NO/COM contacts.
// MCU only controls the relay coil — the LED is powered by the relay circuit.
static void apply_state(bool on) {
    if (on) {
        ddRelayOn();   // closes NO contact → LED lights up via relay circuit
    } else {
        ddRelayOff();  // opens NO contact  → LED goes off
    }
}

// ── Task ─────────────────────────────────────────────────────────────────────
// Behaviour:
//   PRESSED  → debounce_counter increments each 50ms cycle.
//              When it reaches DEBOUNCE_SAMPLES the press is confirmed →
//              relay closes → LED (through NO/COM) turns ON.
//   RELEASED → relay opens → LED turns OFF immediately; counter resets.
//   Short glitch (< DEBOUNCE_SAMPLES cycles) → ignored, relay stays OFF.
void l41_taskActuatorCtrl(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    apply_state(false); // start with relay OFF (LED off)

    for (;;) {
        bool pressed = (bool)ddButtonIsPressed();

        if (pressed) {
            if (s_debounce_counter < DEBOUNCE_SAMPLES) {
                s_debounce_counter++;
            }
            // Confirmed stable press → turn ON (apply only on crossing edge)
            if (s_debounce_counter == DEBOUNCE_SAMPLES && !s_current_state) {
                s_current_state = true;
                apply_state(true);
            }
        } else {
            // Released → turn OFF immediately and reset debounce
            if (s_current_state) {
                s_current_state = false;
                apply_state(false);
            }
            s_debounce_counter = 0;
        }

        // ── Publish state to shared struct ────────────────────────────────
        if (xSemaphoreTake(g_l41_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_l41_actuator.requested     = pressed;
            g_l41_actuator.validated     = s_current_state;
            g_l41_actuator.cmd_received  = pressed;
            g_l41_actuator.confirm_count = s_debounce_counter;
            xSemaphoreGive(g_l41_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, ACTUATOR_CTRL_PERIOD);
    }
}