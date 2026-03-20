#include "task_display.h"
#include "lab_3_2_shared.h"
#include "srv_threshold/srv_threshold.h"

#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <stdio.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

static void lcd_print_row(uint8_t row, const char *str) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", str);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

void l32_taskDisplayInit() {
    lcd.init();
    lcd.backlight();
    lcd_print_row(0, "Lab 3.2 Starting");
    lcd_print_row(1, "2-sensor monitor");
}

// Helper: float → string without leading spaces
static const char* ftos(char *buf, float val, int width, int prec) {
    dtostrf(val, width, prec, buf);
    const char *p = buf;
    while (*p == ' ') p++;
    return p;
}

void l32_taskDisplay(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // ── Snapshot ──────────────────────────────────────────────────────────
        l32_analog_data_t  a = {0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false};
        l32_digital_data_t d = {0.0f, 0.0f, false};
        bool a_alert = false, d_alert = false;
        int  a_conf  = 0,     d_conf  = 0;

        if (xSemaphoreTake(g_l32_mutex_analog,  pdMS_TO_TICKS(20)) == pdTRUE) {
            a = g_l32_analog;
            xSemaphoreGive(g_l32_mutex_analog);
        }
        if (xSemaphoreTake(g_l32_mutex_digital, pdMS_TO_TICKS(20)) == pdTRUE) {
            d = g_l32_digital;
            xSemaphoreGive(g_l32_mutex_digital);
        }
        if (xSemaphoreTake(g_l32_mutex_alert,   pdMS_TO_TICKS(20)) == pdTRUE) {
            a_alert = srvThresholdIsAlert(&g_l32_thr_analog);
            d_alert = srvThresholdIsAlert(&g_l32_thr_digital);
            a_conf  = srvThresholdDebounceCount(&g_l32_thr_analog);
            d_conf  = srvThresholdDebounceCount(&g_l32_thr_digital);
            xSemaphoreGive(g_l32_mutex_alert);
        }

        // ── Buffers for float → string conversion ───────────────────────────
        char b0[8], b1[8], b2[8], b3[8], b4[8], b5[8];

        // ── Zone labels ───────────────────────────────────────────────────────
        const char *a_zone = a.valid
            ? ((a.temperature > TEMP_HIGH_THRESH) ? "ABOVE" :
               (a.temperature < TEMP_LOW_THRESH)  ? "BELOW" : "BAND ")
            : "-----";
        const char *d_zone = d.valid
            ? ((d.temperature > TEMP_HIGH_THRESH) ? "ABOVE" :
               (d.temperature < TEMP_LOW_THRESH)  ? "BELOW" : "BAND ")
            : "-----";

        // ── Serial report ─────────────────────────────────────────────────────
        printf("\n=== Sensor Readings ===\n");

        if (a.valid) {
            printf("Analog (NTC):\n");
            printf("  raw     : %sC  ADC:%d\n",
                   ftos(b0, a.temperature, 5, 1), a.raw_adc);
            printf("  sat     : %sC\n",
                   ftos(b1, a.sat_temperature, 5, 1));
            printf("  median  : %sC\n",
                   ftos(b2, a.med_temperature, 5, 1));
            printf("  wma     : %sC\n",
                   ftos(b3, a.wma_temperature, 5, 1));
            printf("  final   : %sC  zone:%s  alert:%s (%d/%d)\n",
                   ftos(b4, a.conditioned_temperature, 5, 1),
                   a_zone, a_alert ? "ON" : "OFF", a_conf, DEBOUNCE_SAMPLES);
        } else {
            printf("Analog (NTC):   NO DATA\n");
        }

        printf("Digital (DHT11):\n");
        if (d.valid) {
            printf("  temp   : %sC  zone:%s  alert:%s (%d/%d)\n",
                   ftos(b5, d.conditioned_temperature, 5, 1),
                   d_zone, d_alert ? "ON" : "OFF", d_conf, DEBOUNCE_SAMPLES);
        } else {
            printf("  NO DATA\n");
        }

        // ── LCD: linia 0 = analog final, linia 1 = digital ───────────────────
        char row0[17], row1[17];

        if (a.valid) {
            snprintf(row0, sizeof(row0), "A:%sC %s",
                     ftos(b0, a.conditioned_temperature, 5, 1),
                     a_alert ? "ALERT!" : "OK    ");
        } else {
            snprintf(row0, sizeof(row0), "A: --- NO DATA  ");
        }

        if (d.valid) {
            snprintf(row1, sizeof(row1), "D:%sC %s",
                     ftos(b1, d.conditioned_temperature, 5, 1),
                     d_alert ? "ALERT!" : "OK    ");
        } else {
            snprintf(row1, sizeof(row1), "D: --- NO DATA  ");
        }

        lcd_print_row(0, row0);
        lcd_print_row(1, row1);

        vTaskDelayUntil(&xLastWakeTime, DISPLAY_PERIOD);
    }
}