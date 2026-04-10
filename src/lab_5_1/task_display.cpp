#include "task_display.h"
#include "lab_5_1_shared.h"
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

static void lcd_row(uint8_t row, const char *str) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", str);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

void l51_taskDisplayInit() {
    lcd.init();
    lcd.backlight();
}

void l51_taskDisplay(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    printf("Lab 5.1 ready. Use SP:<value>\n");

    for (;;) {
        l51_state_t s;
        if (xSemaphoreTake(g_l51_mutex, pdMS_TO_TICKS(30)) == pdTRUE) {
            s = g_l51;
            xSemaphoreGive(g_l51_mutex);
        }

        // ── LCD ──────────────────────────────────────────────────────────────
        if (s.sensor_error) {
            lcd_row(0, "T: ERR  SP:--.-C");
            lcd_row(1, "SENSOR ERROR!   ");
        } else {
            char row0[17], row1[17];

            bool t_neg  = (s.temperature < 0.0f);
            bool sp_neg = (s.setpoint    < 0.0f);
            float t_abs  = t_neg  ? -s.temperature : s.temperature;
            float sp_abs = sp_neg ? -s.setpoint    : s.setpoint;

            int t_int  = (int)t_abs;
            int t_dec  = (int)(t_abs  * 10.0f) % 10;
            int sp_int = (int)sp_abs;
            int sp_dec = (int)(sp_abs * 10.0f) % 10;

            snprintf(row0, sizeof(row0), "T:%s%d.%dC SP:%s%d.%dC",
                     t_neg ? "-" : "", t_int, t_dec,
                     sp_neg ? "-" : "", sp_int, sp_dec);

            const char *zone;
            if      (s.temperature > s.setpoint + HYSTERESIS_BAND) zone = "[HIGH]";
            else if (s.temperature < s.setpoint - HYSTERESIS_BAND) zone = "[LOW] ";
            else                                                     zone = "[BAND]";

            snprintf(row1, sizeof(row1), "Relay:%-3s %s",
                     s.relay_on ? "ON " : "OFF", zone);

            lcd_row(0, row0);
            lcd_row(1, row1);
        }

        // ── Serial report ─────────────────────────────────────────────────────
        printf("\n=== Temp Control ===\n");
        if (s.sensor_error) {
            printf("  Temperature: ERROR\n");
        } else {
            bool t_neg = (s.temperature < 0.0f);
            float t_abs = t_neg ? -s.temperature : s.temperature;
            printf("  Temperature: %s%d.%d C\n",
                   t_neg ? "-" : "",
                   (int)t_abs, (int)(t_abs * 10.0f) % 10);
        }
        {
            bool sp_neg = (s.setpoint < 0.0f);
            float sp_abs = sp_neg ? -s.setpoint : s.setpoint;
            float lo = s.setpoint - HYSTERESIS_BAND;
            float hi = s.setpoint + HYSTERESIS_BAND;
            bool lo_neg = (lo < 0.0f);
            bool hi_neg = (hi < 0.0f);
            float lo_abs = lo_neg ? -lo : lo;
            float hi_abs = hi_neg ? -hi : hi;

            printf("  Set-point  : %s%d.%d C\n",
                   sp_neg ? "-" : "",
                   (int)sp_abs, (int)(sp_abs * 10.0f) % 10);
            printf("  Dead-band  : %s%d.%d .. %s%d.%d C\n",
                   lo_neg ? "-" : "", (int)lo_abs, (int)(lo_abs * 10.0f) % 10,
                   hi_neg ? "-" : "", (int)hi_abs, (int)(hi_abs * 10.0f) % 10);
        }
        printf("  Relay      : %s\n", s.relay_on ? "ON" : "OFF");
        printf("====================\n");

        // ── Serial Plotter ────────────────────────────────────────────────────
        if (!s.sensor_error) {
            bool t_neg  = (s.temperature < 0.0f);
            bool sp_neg = (s.setpoint    < 0.0f);
            float t_abs  = t_neg  ? -s.temperature : s.temperature;
            float sp_abs = sp_neg ? -s.setpoint    : s.setpoint;

            printf("SetPoint:%s%d.%d Value:%s%d.%d Output:%d\n",
                   sp_neg ? "-" : "", (int)sp_abs, (int)(sp_abs * 10.0f) % 10,
                   t_neg  ? "-" : "", (int)t_abs,  (int)(t_abs  * 10.0f) % 10,
                   s.relay_on ? 1 : 0);
        }

        vTaskDelayUntil(&xLastWakeTime, DISPLAY_PERIOD);
    }
}