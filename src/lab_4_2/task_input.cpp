#include "task_input.h"
#include "lab_4_2_shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUF_SIZE 8

// Persistent buffer between cycles — accumulates characters until '\n' arrives
static char s_buf[INPUT_BUF_SIZE];
static int  s_buf_idx = 0;

void l42_taskInput(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Read all available characters in this cycle (non-blocking).
        // getchar() calls srvSerialGetChar which returns EOF if there is
        // no data — it does not block the task.
        int c;
        while ((c = getchar()) != EOF) {
            if (c == '\n' || c == '\r') {
                // Full line — parse
                if (s_buf_idx > 0) {
                    s_buf[s_buf_idx] = '\0';
                    int val = atoi(s_buf);

                    if (val >= 0 && val <= 100) {
                        if (xSemaphoreTake(g_l42_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                            g_l42_motor.raw_cmd = (float)val;
                            xSemaphoreGive(g_l42_mutex);
                        }
                        printf("CMD: %d%%\n", val);
                    } else {
                        printf("ERR: '%s' invalid (0-100)\n", s_buf);
                    }
                    s_buf_idx = 0;
                }
            } else if (s_buf_idx < INPUT_BUF_SIZE - 1) {
                s_buf[s_buf_idx++] = (char)c;
            }
            // If the buffer is full and '\n' has not arrived, ignore extra characters
        }

        // Yield the CPU — next read after 100 ms
        vTaskDelayUntil(&xLastWakeTime, INPUT_PERIOD);
    }
}