#include "task_input.h"
#include "lab_5_1_shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUF_SIZE 16   // enough for "SP:XX.X\0"

static char s_buf[INPUT_BUF_SIZE];
static int  s_buf_idx = 0;

// Parse "XX" or "XX.X" from str into tenths (e.g. "25.5" → 255, "25" → 250).
// Returns false if any non-digit/dot character is found.
static bool parse_tenths(const char *str, int *tenths_out) {
    int integer_part = 0;
    int decimal_part = 0;   // first decimal digit only
    bool has_decimal = false;

    for (int i = 0; str[i] != '\0'; i++) {
        char ch = str[i];
        if (ch >= '0' && ch <= '9') {
            if (!has_decimal) {
                integer_part = integer_part * 10 + (ch - '0');
            } else {
                decimal_part = (ch - '0');  // take only first decimal digit
                break;                      // ignore further digits
            }
        } else if (ch == '.' && !has_decimal) {
            has_decimal = true;
        } else {
            return false;   // unexpected character
        }
    }

    *tenths_out = integer_part * 10 + decimal_part;
    return true;
}

void l51_taskInput(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Non-blocking: drain all available characters this cycle
        int c;
        while ((c = getchar()) != EOF) {
            if (c == '\n' || c == '\r') {
                if (s_buf_idx > 0) {
                    s_buf[s_buf_idx] = '\0';

                    // Check "SP:" prefix (case-insensitive)
                    bool prefix_ok = (s_buf[0] == 'S' || s_buf[0] == 's') &&
                                     (s_buf[1] == 'P' || s_buf[1] == 'p') &&
                                      s_buf[2] == ':' &&
                                      s_buf[3] != '\0';
                    if (prefix_ok) {
                        int tenths = 0;
                        if (parse_tenths(&s_buf[3], &tenths)) {
                            int min_tenths = (int)(SETPOINT_MIN * 10.0f);
                            int max_tenths = (int)(SETPOINT_MAX * 10.0f);

                            if (tenths >= min_tenths && tenths <= max_tenths) {
                                float new_sp = (float)tenths / 10.0f;
                                if (xSemaphoreTake(g_l51_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                                    g_l51.setpoint = new_sp;
                                    xSemaphoreGive(g_l51_mutex);
                                }
                                printf("SP set: %d.%d C\n", tenths / 10, tenths % 10);
                            } else {
                                printf("ERR: %d.%d out of range [%d-%d C]\n",
                                       tenths / 10, tenths % 10,
                                       (int)SETPOINT_MIN, (int)SETPOINT_MAX);
                            }
                        } else {
                            printf("ERR: invalid number in '%s'\n", s_buf);
                        }
                    } else {
                        printf("ERR: '%s' unknown. Use SP:<value> (e.g. SP:25.5)\n", s_buf);
                    }
                    s_buf_idx = 0;
                }
            } else if (s_buf_idx < INPUT_BUF_SIZE - 1) {
                s_buf[s_buf_idx++] = (char)c;
            }
        }

        vTaskDelayUntil(&xLastWakeTime, INPUT_PERIOD);
    }
}