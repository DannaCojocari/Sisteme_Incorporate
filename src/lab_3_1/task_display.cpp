#include "task_display.h"
#include "lab_3_1_shared.h"
#include "srv_threshold/srv_threshold.h"

#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <stdio.h>

// ─── LCD (owned exclusively by this module) ───────────────────────────────────
static LiquidCrystal_I2C lcd(0x27, 16, 2);

// Writes exactly 16 characters on `row`, left-justified, right-padded with spaces.
static void lcd_print_row(uint8_t row, const char *str) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", str);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

void taskDisplayInit() {
    lcd.init();
    lcd.backlight();
    lcd_print_row(0, "Lab 3.1 Starting");
    lcd_print_row(1, "2-sensor monitor");
}

// ─── Task: Display & Reporting ────────────────────────────────────────────────
void taskDisplay(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // --- Snapshot shared data (brief, independent mutex windows) ----------
        analog_data_t  a = {0, 0.0f, false};
        digital_data_t d = {0.0f, false};
        bool  a_alert = false, d_alert = false;
        int   a_conf  = 0,     d_conf  = 0;

        if (xSemaphoreTake(g_mutex_analog,  pdMS_TO_TICKS(20)) == pdTRUE) {
            a = g_analog;
            xSemaphoreGive(g_mutex_analog);
        }
        if (xSemaphoreTake(g_mutex_digital, pdMS_TO_TICKS(20)) == pdTRUE) {
            d = g_digital;
            xSemaphoreGive(g_mutex_digital);
        }
        if (xSemaphoreTake(g_mutex_alert,   pdMS_TO_TICKS(20)) == pdTRUE) {
            a_alert = srvThresholdIsAlert(&g_thr_analog);
            d_alert = srvThresholdIsAlert(&g_thr_digital);
            a_conf  = srvThresholdDebounceCount(&g_thr_analog);
            d_conf  = srvThresholdDebounceCount(&g_thr_digital);
            xSemaphoreGive(g_mutex_alert);
        }

        // --- Convert floats to strings (dtostrf – safe on AVR without -lprintf_flt) --
        char a_t[8], d_t[8];
        if (a.valid) {
            dtostrf(a.temperature, 5, 1, a_t);
        } else {
            strncpy(a_t, "  ---", sizeof(a_t));
        }
        if (d.valid) {
            dtostrf(d.temperature, 5, 1, d_t);
        } else {
            strncpy(d_t, "  ---", sizeof(d_t));
        }

        // Hysteresis zone label
        const char *a_zone = a.valid
            ? ((a.temperature > TEMP_HIGH_THRESH) ? "ABOVE" :
               (a.temperature < TEMP_LOW_THRESH)  ? "BELOW" : "BAND ")
            : "-----";
        const char *d_zone = d.valid
            ? ((d.temperature > TEMP_HIGH_THRESH) ? "ABOVE" :
               (d.temperature < TEMP_LOW_THRESH)  ? "BELOW" : "BAND ")
            : "-----";

        // Trim leading spaces from dtostrf results for compact printing
        const char *a_tp = a_t; while (*a_tp == ' ') a_tp++;
        const char *d_tp = d_t; while (*d_tp == ' ') d_tp++;

        // --- Serial report ---------------------------------------------------
        printf("\n=== Sensor Readings ===\n");
        printf("Analog (NTC):   T:%sC ADC:%d zone:%s; alert:%s (%d/%d)\n",
               a_tp, a.raw_adc, a_zone,
               a_alert ? "ON" : "OFF", a_conf, DEBOUNCE_SAMPLES);
        printf("Digital(DHT22): T:%sC zone:%s; alert:%s (%d/%d)\n",
               d_tp, d_zone,
               d_alert ? "ON" : "OFF", d_conf, DEBOUNCE_SAMPLES);

        // --- LCD update -------------------------------------------------------
        char row0[17], row1[17];

        if (a.valid) {
            snprintf(row0, sizeof(row0), "A:%sC %s",
                     a_t, a_alert ? " ALERT!" : "  OK   ");
        } else {
            snprintf(row0, sizeof(row0), "A:  ---  NO DATA");
        }

        if (d.valid) {
            snprintf(row1, sizeof(row1), "D:%sC %s",
                     d_t, d_alert ? "  ALERT!" : "  OK    ");
        } else {
            snprintf(row1, sizeof(row1), "D:  ---  NO DATA");
        }

        lcd_print_row(0, row0);
        lcd_print_row(1, row1);

        vTaskDelayUntil(&xLastWakeTime, DISPLAY_PERIOD);
    }
}
