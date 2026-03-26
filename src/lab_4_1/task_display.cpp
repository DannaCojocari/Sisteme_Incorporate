#include "task_display.h"
#include "lab_4_1_shared.h"

#include <LiquidCrystal_I2C.h>
#include <stdio.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

static void lcd_print_row(uint8_t row, const char *str) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", str);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

void l41_taskDisplayInit() {
    lcd.init();
    lcd.backlight();
    lcd_print_row(0, "Lab 4.1 Ready");
    lcd_print_row(1, "Hold btn to ON");
}

void l41_taskDisplay(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        l41_actuator_state_t s = {false, false, false, 0, false};

        if (xSemaphoreTake(g_l41_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            s = g_l41_actuator;
            xSemaphoreGive(g_l41_mutex);
        }

        const char *state_str  = s.validated        ? "ON " : "OFF";
        const char *stable_str = s.stable_confirmed  ? "YES" : "NO ";

        // ── LCD ───────────────────────────────────────────────────────────────
        // Row 0: "Relay:  ON     "  /  "Relay: OFF     "
        // Row 1: "Stb:YES Db:4/4 "
        char row0[17], row1[17];
        snprintf(row0, sizeof(row0), "Relay: %-3s      ", state_str);
        snprintf(row1, sizeof(row1), "Stb:%-3s Db:%d/%-2d ",
                 stable_str, s.confirm_count, DEBOUNCE_SAMPLES);
        lcd_print_row(0, row0);
        lcd_print_row(1, row1);

        // ── Serial report ─────────────────────────────────────────────────────
        printf("\n=== Actuator Report ===\n");
        printf("  Button   : %s\n",  s.requested      ? "PRESSED" : "released");
        printf("  Debounce : %d / %d\n", s.confirm_count, DEBOUNCE_SAMPLES);
        printf("  Relay    : %s\n",  state_str);
        printf("  Stable   : %s\n",  stable_str);
        printf("  Bulb     : %s\n",  s.validated ? "ON  (relay closed)" : "OFF (relay open)");
        printf("=======================\n");

        vTaskDelayUntil(&xLastWakeTime, DISPLAY_PERIOD);
    }
}