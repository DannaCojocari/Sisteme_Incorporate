#include "lab_2_2_app.h"
#include "srv_serial_stdio/srv_serial_stdio.h"
#include "dd_led/dd_led.h"
#include "dd_button/dd_button.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <string.h>

// Task timing constants
// Task 1: 1 tick per cycle (≈ 16 ms) – finest granularity available
#define TASK1_PERIOD_TICKS   ((TickType_t)1)
// Task 2: 8 ticks ≈ 128 ms (nearest multiple of 16 ms to 100 ms)
#define TASK2_PERIOD_TICKS   ((TickType_t)8)
// Task 3: ~10 s = 10000 / portTICK_PERIOD_MS ticks
#define TASK3_PERIOD_TICKS   (pdMS_TO_TICKS(10000))

// Offsets (in ticks)
#define TASK1_OFFSET_TICKS   ((TickType_t)0)   // Start immediately
#define TASK2_OFFSET_TICKS   ((TickType_t)1)   // 1 tick after task creation
#define TASK3_OFFSET_TICKS   ((TickType_t)8)   // 8 ticks after task creation

// Debounce threshold in task-1 cycles (2 × 16 ms ≈ 32 ms)
#define DEBOUNCE_TICKS    2


// Shared data types
typedef struct {
    int           total_presses;
    int           short_presses;
    int           long_presses;
    unsigned long total_short_time;
    unsigned long total_long_time;
} stats_t;


// Global shared variables
static stats_t           g_stats;
static volatile unsigned long g_press_duration;

// Binary semaphore 
static SemaphoreHandle_t g_press_semaphore;

// Mutex
static SemaphoreHandle_t g_stats_mutex;


static void task1_button_monitor(void *pvParameters);
static void task2_statistics(void *pvParameters);
static void task3_reporting(void *pvParameters);


void lab2_2Setup() {
    srvSerialSetup();
    ddLedSetup();
    ddButtonSetup();

    // Initialise shared data
    memset(&g_stats, 0, sizeof(g_stats));
    g_press_duration = 0;

    // Create FreeRTOS synchronisation primitives
    g_press_semaphore = xSemaphoreCreateBinary();
    g_stats_mutex     = xSemaphoreCreateMutex();

    printf("Lab 2.2 - FreeRTOS Preemptive Tasks\n");
    printf("Sync: binary semaphore (T1->T2), mutex (T2/T3)\n");
    printf("Tick: %u ms  T1:%u T2:%u T3:%u ticks\n",
           (unsigned)portTICK_PERIOD_MS,
           (unsigned)TASK1_PERIOD_TICKS,
           (unsigned)TASK2_PERIOD_TICKS,
           (unsigned)TASK3_PERIOD_TICKS);
    printf("Press button: <500ms=short, >=500ms=long\n\n");

    // Create tasks
    xTaskCreate(task1_button_monitor, "T1_BTN",   192, NULL, 2, NULL);  // no printf
    xTaskCreate(task2_statistics,     "T2_STATS", 320, NULL, 2, NULL);  // printf per press
    xTaskCreate(task3_reporting,      "T3_REPORT",384, NULL, 1, NULL);  // several printfs
}

void lab2_2Loop() {
    // Empty – FreeRTOS scheduler manages execution; loop() is its own idle task
}

// Task 1 – Button monitoring  
static void task1_button_monitor(void *pvParameters) {
    (void)pvParameters;

    // One-shot initial delay
    vTaskDelay(TASK1_OFFSET_TICKS);

    // Capture reference time *after* the offset so the first period is correct
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = TASK1_PERIOD_TICKS;

    // Local (task-private) variables – no locking required
    int           button_pressed    = 0;
    int           last_button_state = 0;
    unsigned char debounce_counter  = 0;
    unsigned int  press_ticks       = 0;

    // Infinite loop
    for (;;) {
        int current_button = ddButtonIsPressed();

        // Debouncing 
        if (current_button == last_button_state) {
            if (debounce_counter > 0) debounce_counter--;
        } else {
            last_button_state = current_button;
            debounce_counter  = DEBOUNCE_TICKS;
        }

        if (debounce_counter == 0) {
            if (current_button) {
                // Button held down
                if (!button_pressed) {
                    button_pressed = 1;
                    press_ticks    = 0;
                    ddLedTurnOff(LED_GREEN);
                    ddLedTurnOff(LED_RED);
                } else {
                    press_ticks++;
                }
            } else {
                // Button released
                if (button_pressed) {
                    button_pressed = 0;

                    // press_ticks counts Task-1 cycles
                    g_press_duration = (unsigned long)press_ticks * portTICK_PERIOD_MS;

                    // Visual feedback (immediate, no lock needed)
                    if (g_press_duration < 500) {
                        ddLedTurnOn(LED_GREEN);
                        ddLedTurnOff(LED_RED);
                    } else {
                        ddLedTurnOff(LED_GREEN);
                        ddLedTurnOn(LED_RED);
                    }

                    // --- BINARY SEMAPHORE: signal press event to Task 2 -------
                    xSemaphoreGive(g_press_semaphore);
                }
            }
        }

        // Use vTaskDelayUntil for drift-free timing
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}


// Task 2 – Statistics + LED blinking
static void task2_statistics(void *pvParameters) {
    (void)pvParameters;

    // One-shot initial delay
    vTaskDelay(TASK2_OFFSET_TICKS);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = TASK2_PERIOD_TICKS;

    int blink_count   = 0;
    int blinks_needed = 0;
    int led_state     = 0;

    for (;;) {
        // BINARY SEMAPHORE: poll for a new press event (non-blocking) 
        if (xSemaphoreTake(g_press_semaphore, 0) == pdTRUE) {
            unsigned long duration = g_press_duration; // safe: semaphore as barrier

            if (duration > 0 && duration < 10000UL) {
                // MUTEX: acquire before modifying shared statistics 
                if (xSemaphoreTake(g_stats_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                    g_stats.total_presses++;
                    if (duration < 500) {
                        g_stats.short_presses++;
                        g_stats.total_short_time += duration;
                        blinks_needed = 5;
                    } else {
                        g_stats.long_presses++;
                        g_stats.total_long_time += duration;
                        blinks_needed = 10;
                    }
                    xSemaphoreGive(g_stats_mutex);
                    // MUTEX released 
                }
                blink_count = 0;
                printf("Press: %lu ms, Total: %d\n",
                       duration, g_stats.total_presses);
            } else {
                printf("Invalid press: %lu ms – ignored\n", duration);
            }
        }

        // Handle LED blinking (task-local state, no lock needed)
        if (blink_count < blinks_needed) {
            if (led_state) {
                ddLedTurnOff(LED_YELLOW);
                led_state = 0;
                blink_count++;
            } else {
                ddLedTurnOn(LED_YELLOW);
                led_state = 1;
            }
        } else if (blinks_needed > 0) {
            ddLedTurnOff(LED_YELLOW);
            blinks_needed = 0;
        }

        // PERIOD: use vTaskDelayUntil for drift-free timing ---------------
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}


// Task 3 – Periodic statistics report  (period ~10 s, prio 1)
static void task3_reporting(void *pvParameters) {
    (void)pvParameters;

    // One-shot initial delay
    vTaskDelay(TASK3_OFFSET_TICKS);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = TASK3_PERIOD_TICKS;

    for (;;) {
        stats_t snap; // local snapshot – avoids holding the mutex while printing

        // MUTEX: acquire to read + atomically reset shared statistics 
        if (xSemaphoreTake(g_stats_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            snap = g_stats;                       // copy under lock
            memset(&g_stats, 0, sizeof(g_stats)); // reset under same lock
            xSemaphoreGive(g_stats_mutex);
            // MUTEX released – print the snapshot without holding the lock
        } else {
            // Timeout acquiring mutex – skip this report cycle
            vTaskDelayUntil(&xLastWakeTime, xPeriod);
            continue;
        }

        printf("\n=== STATISTICS REPORT ===\n");
        printf("Total presses   : %d\n",  snap.total_presses);
        printf("Short (<500 ms) : %d\n",  snap.short_presses);
        printf("Long  (>=500 ms): %d\n",  snap.long_presses);
        if (snap.short_presses > 0)
            printf("Avg short       : %lu ms\n",
                   snap.total_short_time / (unsigned long)snap.short_presses);
        if (snap.long_presses > 0)
            printf("Avg long        : %lu ms\n",
                   snap.total_long_time  / (unsigned long)snap.long_presses);
        if (snap.total_presses > 0)
            printf("Overall avg     : %lu ms\n",
                   (snap.total_short_time + snap.total_long_time)
                   / (unsigned long)snap.total_presses);
        printf("=========================\n\n");

        // PERIOD: use vTaskDelayUntil for drift-free timing
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}
