#include "lab_2_1_app.h"
#include "srv_serial_stdio/srv_serial_stdio.h"
#include "Arduino.h"
#include "dd_led/dd_led.h"
#include "dd_button/dd_button.h"
#include "srv_os_seq/srv_os_seq.h"

// Simple global variables for task communication
int button_pressed = 0;
int last_button_state = 0;
unsigned long press_duration = 0;
int new_press = 0;
unsigned char debounce_counter = 0;

// Debouncing constants
#define DEBOUNCE_TICKS 2  // 2 task cycles = 20ms debounce

// Statistics
int total_presses = 0;
int short_presses = 0;
int long_presses = 0;
unsigned long total_short_time = 0;
unsigned long total_long_time = 0;

// Blinking control
int blink_count = 0;
int blinks_needed = 0;
int led_state = 0;

void lab2_1Setup() {
    // Initialize hardware
    srvSerialSetup();
    ddLedSetup();
    ddButtonSetup();
    
    // Initialize all button monitoring variables
    button_pressed = 0;
    last_button_state = 0;
    press_duration = 0;
    new_press = 0;
    debounce_counter = 0;
    
    // Initialize statistics
    total_presses = 0;
    short_presses = 0;
    long_presses = 0;
    total_short_time = 0;
    total_long_time = 0;
    
    // Initialize blinking control
    blink_count = 0;
    blinks_needed = 0;
    led_state = 0;
    
    // Setup scheduler
    srvOsSeqSetup();
    
    printf("Lab 2.1 - Simple Button Monitor\n");
    printf("Press button: <500ms=short, >=500ms=long\n");
    printf("LEDs: Green=short, Red=long, Yellow=blinks\n");
    printf("Report every 10 seconds\n\n");
}

void lab2_1Loop() {
    // Use sequential OS scheduler instead of delay
    srvOsSeqRun();
}

// Task 1: Button monitoring (10ms)
void lab2_1_task1() {
    int current_button = ddButtonIsPressed();
    static unsigned int press_ticks = 0;
    
    // Debouncing: update state only if stable
    if (current_button == last_button_state) {
        if (debounce_counter > 0) {
            debounce_counter--;
        }
    } else {
        last_button_state = current_button;
        debounce_counter = DEBOUNCE_TICKS;
    }
    
    // Proceed only if button is stable (debouncing complete)
    if (debounce_counter == 0) {
        if (current_button) {
            // Button is pressed
            if (!button_pressed) {
                // Just pressed - start counting
                button_pressed = 1;
                press_ticks = 0;
                ddLedTurnOff(LED_GREEN);
                ddLedTurnOff(LED_RED);
            } else {
                // Still pressed - keep counting
                press_ticks++;
            }
        } else {
            // Button is not pressed
            if (button_pressed) {
                // Just released - calculate duration
                press_duration = press_ticks * 10; // Convert ticks to ms
                new_press = 1;
                button_pressed = 0;
                
                // Visual feedback
                if (press_duration < 500) {
                    ddLedTurnOn(LED_GREEN);
                    ddLedTurnOff(LED_RED);
                } else {
                    ddLedTurnOff(LED_GREEN);
                    ddLedTurnOn(LED_RED);
                }
            }
        }
    }
}

// Task 2: Statistics and blinking (100ms)
void lab2_1_task2() {
    // Process new button press
    if (new_press) {
        new_press = 0;  // Clear flag immediately to prevent multiple processing
        
        // Validate press duration (should be reasonable)
        if (press_duration > 0 && press_duration < 10000) {  // Between 0ms and 10 seconds
            total_presses++;
            
            if (press_duration < 500) {
                short_presses++;
                total_short_time += press_duration;
                blinks_needed = 5;
            } else {
                long_presses++;
                total_long_time += press_duration;
                blinks_needed = 10;
            }
            
            printf("Press: %lu ms, Total: %d\n", press_duration, total_presses);
        } else {
            printf("Invalid press duration: %lu ms - Ignored\n", press_duration);
        }
        
        blink_count = 0;
        press_duration = 0;  // Reset duration
    }
    
    // Handle LED blinking
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
}

// Task 3: Periodic reporting (10 seconds)
void lab2_1_task3() {
    printf("\n=== STATISTICS REPORT ===\n");
    printf("Total presses: %d\n", total_presses);
    printf("Short presses (<500ms): %d\n", short_presses);
    printf("Long presses (>=500ms): %d\n", long_presses);
    
    if (short_presses > 0) {
        unsigned long avg_short = total_short_time / short_presses;
        printf("Avg short: %lu ms\n", avg_short);
    }
    if (long_presses > 0) {
        unsigned long avg_long = total_long_time / long_presses;
        printf("Avg long: %lu ms\n", avg_long);
    }
    if (total_presses > 0) {
        unsigned long avg_overall = (total_short_time + total_long_time) / total_presses;
        printf("Overall avg: %lu ms\n", avg_overall);
    }
    printf("========================\n\n");
    
    // Reset statistics
    total_presses = 0;
    short_presses = 0;
    long_presses = 0;
    total_short_time = 0;
    total_long_time = 0;
}