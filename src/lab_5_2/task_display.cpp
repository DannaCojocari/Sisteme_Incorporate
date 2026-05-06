#include "task_display.h"
#include "lab_5_2_shared.h"
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

// ── Helper: write a string of exactly 16 characters on an LCD row ─────────────
static void lcd_row(uint8_t row, const char *str) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", str);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

// ── Helper: format a float with sign and one decimal place without %f ────────────────
// Avoids dependency on sprintf %f on AVR (consumes a lot of flash).
// The result is written in buf[0..max_len-1], returns the number of characters.
static int fmt_temp(char *buf, int max_len, float val) {
    bool  neg = (val < 0.0f);
    float abs = neg ? -val : val;
    int   i   = (int)abs;
    int   d   = (int)(abs * 10.0f) % 10;
    return snprintf(buf, (size_t)max_len, "%s%d.%d", neg ? "-" : "", i, d);
}

// ── LCD Initialization (called before startScheduler) ─────────────────────
void l52_taskDisplayInit() {
    lcd.init();
    lcd.backlight();
}

// ── Task 4: Display & Reporting ───────────────────────────────────────────────
void l52_taskDisplay(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Startup message
    printf("\n=== Lab 5.2 – PID Temperature Control ===\n");
    printf("Serial commands: SP:<val>  KP:<val>  KI:<val>  KD:<val>\n");
    printf("Example: SP:27.5  KP:3.0  KI:0.2  KD:1.0\n");
    printf("output > 0 -> Heater ON | output < 0 -> Fan ON (proportional)\n");
    printf("==========================================\n\n");

    for (;;) {
        // ── Snapshot shared state ─────────────────────────────────────────────
        // Snapshot initialized with safe values: if the mutex were to fail (timeout),
        // we display the previous data instead of undefined behavior.
        l52_state_t s = g_l52;
        if (xSemaphoreTake(g_l52_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            s = g_l52;
            xSemaphoreGive(g_l52_mutex);
        }

        // ── LCD 2x16 ──────────────────────────────────────────────────────────
        if (s.sensor_error) {
            lcd_row(0, "T:ERR   SP:--.-C");
            lcd_row(1, "SENSOR ERROR!   ");
        } else {
            char t_str[8], sp_str[8], sramp_str[8];
            fmt_temp(t_str,     sizeof(t_str),     s.temperature);
            fmt_temp(sp_str,    sizeof(sp_str),    s.setpoint);
            fmt_temp(sramp_str, sizeof(sramp_str), s.setpoint_ramp);

            char row0[17], row1[17];

            // Row 0: "T:24.3C SP:25.0C"
            snprintf(row0, sizeof(row0), "T:%sC SP:%sC", t_str, sp_str);

            // Row 1: "Out:+123 H:ON F:--"
            // Fixed format 16 characters, without %f on AVR.
            // "Out:" = 4, sign+3digits = 4, " H:XX F:XX" = 10  => exactly 16+1
            {
                int  out_abs  = (int)(s.output >= 0.0f ? s.output : -s.output);
                char out_sign = (s.output >= 0.0f) ? '+' : '-';
                snprintf(row1, sizeof(row1), "O:%c%-3d H:%s F:%s",
                         out_sign, out_abs,
                         s.relay_on ? "ON" : "--",
                         s.fan_on   ? "ON" : "--");
            }

            lcd_row(0, row0);
            lcd_row(1, row1);
        }

        // ── Serial: human-readable report ────────────────────────────────────
        printf("\n=== PID Temp Control ===\n");
        if (s.sensor_error) {
            printf("  Temperature  : SENSOR ERROR\n");
        } else {
            char t_str[10];
            fmt_temp(t_str, sizeof(t_str), s.temperature);
            printf("  Temperature  : %s C\n", t_str);
        }

        {
            char sp_str[10], sr_str[10];
            fmt_temp(sp_str, sizeof(sp_str), s.setpoint);
            fmt_temp(sr_str, sizeof(sr_str), s.setpoint_ramp);
            printf("  Set-point    : %s C\n", sp_str);
            printf("  SP ramp      : %s C\n", sr_str);
        }

        // Kp/Ki/Kd with one decimal place (without %f)
        {
            int kp_i = (int)(s.kp * 10.0f);
            int ki_i = (int)(s.ki * 10.0f);
            int kd_i = (int)(s.kd * 10.0f);
            printf("  PID Kp/Ki/Kd : %d.%d / %d.%d / %d.%d\n",
                   kp_i / 10, kp_i % 10,
                   ki_i / 10, ki_i % 10,
                   kd_i / 10, kd_i % 10);
        }

        {
            int out_i = (int)s.output;
            int int_i = (int)(s.integral >= 0.0f ? s.integral : -s.integral);
            int int_d = (int)((s.integral >= 0.0f ? s.integral : -s.integral) * 10.0f) % 10;
            printf("  PID output   : %d\n", out_i);
            printf("  Integral     : %s%d.%d\n",
                   s.integral < 0.0f ? "-" : "", int_i, int_d);
        }

        printf("  Heater       : %s\n", s.relay_on ? "ON"  : "OFF");

        if (s.fan_on) {
            int fan_i = (int)s.fan_pct;
            int fan_d = (int)(s.fan_pct * 10.0f) % 10;
            printf("  Fan          : ON  (%d.%d%%)\n", fan_i, fan_d);
        } else {
            printf("  Fan          : OFF\n");
        }

        printf("========================\n");

        // ── Serial Plotter (Arduino Serial Plotter compatible) ────────────────
        // Format "Var:val" on a single line — displays 3 curves simultaneously.
        if (!s.sensor_error) {
            // We use fmt_temp to avoid %f on AVR
            char t_str[10], sp_str[10];
            fmt_temp(t_str,  sizeof(t_str),  s.temperature);
            fmt_temp(sp_str, sizeof(sp_str), s.setpoint_ramp);
            int out_i = (int)s.output;
            printf("SetPoint:%s Value:%s Output:%d\n", sp_str, t_str, out_i);
        }

        vTaskDelayUntil(&xLastWakeTime, DISPLAY_PERIOD);
    }
}