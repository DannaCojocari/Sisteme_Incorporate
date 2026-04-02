#include "task_display.h"
#include "lab_4_2_shared.h"
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

static void lcd_row(uint8_t row, const char *str) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", str);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

void l42_taskDisplayInit() {
    lcd.init();
    lcd.backlight();
    lcd_row(0, "Lab 4.2 DC Motor");
    lcd_row(1, "Send 0-100");
}

void l42_taskDisplay(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Snapshot shared state
        l42_motor_state_t s = {0.0f, 0.0f, 0.0f, 0, false, false, 0};
        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            s = g_l42_motor;
            xSemaphoreGive(g_l42_mutex);
        }

        // LCD
        char row0[17], row1[17];
        snprintf(row0, sizeof(row0), "Mot:%03d%% PWM:%03d",
                 (int)s.ramped, s.pwm_value);
        snprintf(row1, sizeof(row1), "Cnd:%03d%% %s",
                 (int)s.conditioned,
                 s.alert_overload ? "[ALRT]" :
                 s.limit_reached  ? "[LIM] " : " OK   ");
        lcd_row(0, row0);
        lcd_row(1, row1);

        // Serial report
        printf("\n=== Motor Report ===\n");
        printf("  Raw cmd    : %d %%\n",   (int)s.raw_cmd);
        printf("  Conditioned: %d %%\n",   (int)s.conditioned);
        printf("  Ramped     : %d %%\n",   (int)s.ramped);
        printf("  PWM        : %d / 255\n", s.pwm_value);
        printf("  Overload   : %s\n",      s.alert_overload ? "YES (!)" : "NO");
        printf("  Limit      : %s\n",      s.limit_reached  ? "YES"     : "NO");
        printf("  Debounce   : %d / %d\n", s.debounce_count, ALERT_DEBOUNCE);
        printf("====================\n");

        vTaskDelayUntil(&xLastWakeTime, DISPLAY_PERIOD);
    }
}