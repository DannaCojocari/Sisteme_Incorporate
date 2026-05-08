#include "task_button.h"
#include "lab_6_1_shared.h"
#include "dd_button/dd_button.h"

void l61_taskButton(void *pvParameters) {
    (void)pvParameters;

    ddButtonSetup();

    // Debounce counter – increments on LOW (pressed), decrements on HIGH (released)
    int  debounce_counter = 0;
    bool last_stable      = false;  // last known stable state

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // 1. Raw button reading
        bool raw_pressed = ddButtonIsPressed();  // true = pressed (LOW)

        // 2. Update debounce counter
        if (raw_pressed) {
            if (debounce_counter < DEBOUNCE_COUNT)
                debounce_counter++;
        } else {
            if (debounce_counter > 0)
                debounce_counter--;
        }

        // 3. Determine stable state
        bool stable_pressed = false;
        if (debounce_counter >= DEBOUNCE_COUNT) {
            stable_pressed = true;
        } else if (debounce_counter == 0) {
            stable_pressed = false;
        } else {
            stable_pressed = last_stable;  // in uncertainty zone → keep the last state
        }

        // 4. Detect rising edge (RELEASED → PRESSED)
        //    Send the semaphore only once per valid press
        if (stable_pressed && !last_stable) {
            xSemaphoreGive(g_l61_btn_semaphore);
        }

        last_stable = stable_pressed;

        // 5. Publish the debounced state in the shared state
        if (xSemaphoreTake(g_l61_mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
            g_l61.button_stable = stable_pressed;
            xSemaphoreGive(g_l61_mutex);
        }

        vTaskDelayUntil(&xLastWakeTime, BUTTON_PERIOD);
    }
}