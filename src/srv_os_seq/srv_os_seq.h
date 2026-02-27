#ifndef SRV_OS_SEQ_H
#define SRV_OS_SEQ_H

// Task context structure
typedef struct {
    void (*task_function)();    // Pointer to task function
    unsigned int period;        // Recurrence (period) in ms
    unsigned int offset;        // Initial offset in ms
    unsigned int counter;       // Counter for timing
} task_context_t;

// Task definitions
#define TASK1_PERIOD 10     // 10ms for button monitoring
#define TASK2_PERIOD 100    // 100ms for statistics
#define TASK3_PERIOD 10000  // 10 seconds for reporting

#define TASK1_OFFSET 0      // Start immediately
#define TASK2_OFFSET 5      // Start after 5ms
#define TASK3_OFFSET 100    // Start after 100ms

void srvOsSeqSetup();
void srvOsSeqRun();      // Non-preemptive scheduler loop

#endif // SRV_OS_SEQ_H