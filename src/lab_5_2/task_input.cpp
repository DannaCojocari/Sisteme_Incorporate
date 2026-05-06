#include "task_input.h"
#include "lab_5_2_shared.h"
#include <stdio.h>
#include <string.h>

// ── Task 3: Serial Set-Point & PID Parameter Input (200 ms) ──────────────────
//
// Accepted commands (case-insensitive prefix, numeric value XX or XX.X):
//
//   SP:<val>   — sets the setpoint   [SETPOINT_MIN .. SETPOINT_MAX °C]
//   KP:<val>   — sets Kp            [0.0 .. 50.0]
//   KI:<val>   — sets Ki            [0.0 .. 10.0]
//   KD:<val>   — sets Kd            [0.0 .. 20.0]
//
// When the setpoint is changed, the integrator is automatically reset in
// taskControl (detection by delta > 0.05 C).
//
// Examples:
//   "SP:27.5"  → setpoint 27.5 °C
//   "KP:4.0"   → Kp = 4.0
//   "KI:0.15"  → Ki = 0.15  (the first decimal is taken → 0.1; for more
//                             precision send "KI:0.2" etc.)
//   "KD:1.5"   → Kd = 1.5
//

#define INPUT_BUF_SIZE  16   // enough for "KP:XX.X\0"

static char s_buf[INPUT_BUF_SIZE];
static int  s_buf_idx = 0;

// ── Helper: parse "XX" or "XX.X" into tenths (e.g. "25.5" → 255) ─────────────
// Returns false if the string contains unexpected characters.
static bool parse_tenths(const char *str, int *tenths_out) {
    int  integer_part = 0;
    int  decimal_part = 0;
    bool has_decimal  = false;

    for (int i = 0; str[i] != '\0'; i++) {
        char ch = str[i];
        if (ch >= '0' && ch <= '9') {
            if (!has_decimal) {
                integer_part = integer_part * 10 + (ch - '0');
            } else {
                decimal_part = (ch - '0');   // first decimal digit
                break;                        // the rest is ignored
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

// ── Helper: set a float field in the shared state under mutex ──────────────────
static void set_field(volatile float *field, float value) {
    if (xSemaphoreTake(g_l52_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        *field = value;
        xSemaphoreGive(g_l52_mutex);
    }
}

// ── Helper: compare 3-character prefix case-insensitively ───────────────────
static bool prefix3(const char *buf, char c0, char c1) {
    return (buf[0] == c0 || buf[0] == (c0 | 0x20)) &&
           (buf[1] == c1 || buf[1] == (c1 | 0x20)) &&
            buf[2] == ':' &&
            buf[3] != '\0';
}

// ── Process complete line ──────────────────────────────────────────────────
static void process_line(void) {
    int tenths = 0;

    if (prefix3(s_buf, 'S', 'P')) {
        // Command: SP:<val>
        if (!parse_tenths(&s_buf[3], &tenths)) {
            printf("ERR: invalid number in '%s'\n", s_buf);
            return;
        }
        int min_t = (int)(SETPOINT_MIN * 10.0f);
        int max_t = (int)(SETPOINT_MAX * 10.0f);
        if (tenths < min_t || tenths > max_t) {
            printf("ERR: %d.%d C outside range [%d..%d C]\n",
                   tenths / 10, tenths % 10,
                   (int)SETPOINT_MIN, (int)SETPOINT_MAX);
            return;
        }
        float new_sp = (float)tenths / 10.0f;
        // Write setpoint; taskControl will detect the delta and reset the integrator.
        set_field(&g_l52.setpoint, new_sp);
        printf("SP: %d.%d C\n", tenths / 10, tenths % 10);

    } else if (prefix3(s_buf, 'K', 'P')) {
        // Command: KP:<val>  range [0.0 .. 50.0]
        if (!parse_tenths(&s_buf[3], &tenths)) {
            printf("ERR: invalid number in '%s'\n", s_buf);
            return;
        }
        if (tenths < 0 || tenths > 500) {
            printf("ERR: Kp %d.%d outside range [0..50]\n",
                   tenths / 10, tenths % 10);
            return;
        }
        set_field(&g_l52.kp, (float)tenths / 10.0f);
        printf("KP: %d.%d\n", tenths / 10, tenths % 10);

    } else if (prefix3(s_buf, 'K', 'I')) {
        // Command: KI:<val>  range [0.0 .. 10.0]
        if (!parse_tenths(&s_buf[3], &tenths)) {
            printf("ERR: invalid number in '%s'\n", s_buf);
            return;
        }
        if (tenths < 0 || tenths > 100) {
            printf("ERR: Ki %d.%d outside range [0..10]\n",
                   tenths / 10, tenths % 10);
            return;
        }
        set_field(&g_l52.ki, (float)tenths / 10.0f);
        printf("KI: %d.%d\n", tenths / 10, tenths % 10);

    } else if (prefix3(s_buf, 'K', 'D')) {
        // Command: KD:<val>  range [0.0 .. 20.0]
        if (!parse_tenths(&s_buf[3], &tenths)) {
            printf("ERR: invalid number in '%s'\n", s_buf);
            return;
        }
        if (tenths < 0 || tenths > 200) {
            printf("ERR: Kd %d.%d outside range [0..20]\n",
                   tenths / 10, tenths % 10);
            return;
        }
        set_field(&g_l52.kd, (float)tenths / 10.0f);
        printf("KD: %d.%d\n", tenths / 10, tenths % 10);

    } else {
        printf("ERR: '%s' unknown. Commands: SP: KP: KI: KD:\n", s_buf);
    }
}

// ── Task body ─────────────────────────────────────────────────────────────────
void l52_taskInput(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Non-blocking: read all available characters in this cycle
        int c;
        while ((c = getchar()) != EOF) {
            if (c == '\n' || c == '\r') {
                if (s_buf_idx > 0) {
                    s_buf[s_buf_idx] = '\0';
                    process_line();
                    s_buf_idx = 0;
                }
            } else if (s_buf_idx < INPUT_BUF_SIZE - 1) {
                s_buf[s_buf_idx++] = (char)c;
            }
            // If the buffer is full and '\n' has not arrived, the character is ignored
            // (overflow protection).
        }

        vTaskDelayUntil(&xLastWakeTime, INPUT_PERIOD);
    }
}