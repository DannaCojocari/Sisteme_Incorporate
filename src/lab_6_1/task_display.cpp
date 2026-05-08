#include "task_display.h"
#include "lab_6_1_shared.h"
#include "lab_6_1_fsm.h"
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

// Writes a string on an LCD row, padding with spaces up to 16 characters
static void lcd_row(uint8_t row, const char *str) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", str);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

void l61_taskDisplayInit() {
    lcd.init();
    lcd.backlight();
    lcd_row(0, "FSM Button-LED");
    lcd_row(1, "Initializing...");
}

void l61_taskDisplay(void *pvParameters) {
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Welcome message on Serial
    printf("\n=== Lab 6.1 – FSM Button-LED (Moore) ===\n");
    printf("Press the button to toggle the LED.\n");
    printf("=========================================\n\n");

    for (;;) {
        // ── Atomic snapshot of the shared state ───────────────────────────────
        l61_state_t s;
        if (xSemaphoreTake(g_l61_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            s = g_l61;
            xSemaphoreGive(g_l61_mutex);
        }

        const char *state_name = fsmGetStateName(s.fsm_state);
        const char *led_str    = s.led_on        ? "ON " : "OFF";
        const char *btn_str    = s.button_stable ? "PR " : "-- ";

        // ── LCD Row 0: FSM state ─────────────────────────────────────────────
        char row0[17];
        snprintf(row0, sizeof(row0), "FSM: %-11s", state_name);
        lcd_row(0, row0);

        // ── LCD Row 1: button + transition count ────────────────────────────────
        char row1[17];
        snprintf(row1, sizeof(row1), "Btn:%s T:%4lu", btn_str, (unsigned long)s.transitions);
        lcd_row(1, row1);

        // ── Serial human-readable ─────────────────────────────────────────────
        printf("\n=== FSM Button-LED ===\n");
        printf("  FSM State  : %s\n", state_name);
        printf("  LED        : %s\n", led_str);
        printf("  Button     : %s\n", s.button_stable ? "PRESSED" : "RELEASED");
        printf("  Transitions: %lu\n", (unsigned long)s.transitions);
        printf("======================\n");

        // ── Serial Plotter (Arduino Serial Plotter compatible) ────────────────
        // Numeric values allow real-time graphical visualization
        // State: 0=LED_OFF, 1=LED_ON | LED: 0/1 | Button: 0/1
        printf("State:%d LED:%d Button:%d\n",
               s.fsm_state,
               s.led_on        ? 1 : 0,
               s.button_stable ? 1 : 0);

        vTaskDelayUntil(&xLastWakeTime, DISPLAY_PERIOD);
    }
}