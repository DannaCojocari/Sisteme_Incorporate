#include "task_input.h"
#include "lab_4_2_shared.h"

#include <Keypad.h>

// ── Keypad hardware configuration ────────────────────────────────────────────
static const byte ROWS = 4;
static const byte COLS = 4;

static char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static byte rowPins[ROWS] = {2, 3, 4, 5};    // keypad row pins
static byte colPins[COLS] = {22, 23, 24, 25}; // keypad column pins (digital, no conflicts)

static Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// ── Key → percent conversion ──────────────────────────────────────────────────
// Returns -1 if key should be ignored.
static float key_to_percent(char k) {
    if (k >= '0' && k <= '9') return (float)(k - '0') * 10.0f;
    if (k == '*') return 0.0f;    // emergency stop
    if (k == '#') return 100.0f;  // full speed
    return -1.0f;                 // A–D: ignore
}

// ── Task ──────────────────────────────────────────────────────────────────────
void l42_taskInput(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        char key = keypad.getKey();

        if (key != NO_KEY) {
            float pct = key_to_percent(key);

            if (pct >= 0.0f) {
                if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                    g_l42_servo.raw_cmd = pct;
                    xSemaphoreGive(g_l42_mutex);
                }
            }
        }

        vTaskDelayUntil(&xLastWakeTime, INPUT_PERIOD);
    }
}