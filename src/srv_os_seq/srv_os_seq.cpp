#include "srv_os_seq.h"
#include "timer-api.h"
#include "lab_2_1/lab_2_1_app.h"

// Static task table with context structures
static task_context_t task_table[] = {
    {lab2_1_task1, TASK1_PERIOD, TASK1_OFFSET, TASK1_OFFSET},  // Task 1: Button monitoring
    {lab2_1_task2, TASK2_PERIOD, TASK2_OFFSET, TASK2_OFFSET},  // Task 2: Statistics
    {lab2_1_task3, TASK3_PERIOD, TASK3_OFFSET, TASK3_OFFSET}   // Task 3: Reporting
};

#define TASK_COUNT (sizeof(task_table) / sizeof(task_context_t))

// Counter for ticks
static volatile unsigned int tick_counter = 0;

void srvOsSeqSetup() {
    // Setup timer for 1ms ticks
    timer_init_ISR_1KHz(TIMER_DEFAULT);
}


// Timer interrupt - called every 1ms
void timer_handle_interrupts(int timer) {
    tick_counter++; // Increment counter for each 1ms tick
}


// Non-preemptive bare-metal scheduler loop
void srvOsSeqRun() {
    // Process accumulated ticks
    unsigned int ticks_to_process = tick_counter;
    if (ticks_to_process > 0) {
        tick_counter = 0; // Reset counter
        
        // Decrement counters for all tasks with accumulated ticks
        for (unsigned int i = 0; i < TASK_COUNT; i++) {
            if (task_table[i].counter >= ticks_to_process) {
                task_table[i].counter -= ticks_to_process;
            } else {
                task_table[i].counter = 0;
            }
        }
    }
    
    // Non-preemptive execution of tasks in sequential order
    for (unsigned int i = 0; i < TASK_COUNT; i++) {
        if (task_table[i].counter == 0) {
            // Reset counter with period for next execution
            task_table[i].counter = task_table[i].period;
            
            // Execute task (non-preemptive - runs completely)
            task_table[i].task_function();
        }
    }
}