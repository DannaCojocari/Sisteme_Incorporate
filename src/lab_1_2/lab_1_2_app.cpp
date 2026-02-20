#include "lab_1_2_app.h"
#include "srv_stdio_lcd/srv_stdio_lcd.h"
#include "srv_stdio_keypad/srv_stdio_keypad.h"
#include "dd_led/dd_led.h"
#include "stdio.h"
#include <Arduino.h>

void lab1_2Setup() {
    // Initialize LCD driver
    srvStdioLcdSetup();

    // Initialize keypad driver
    srvStdioKeypadSetup();
    
    // Initialize LED driver
    ddLedSetup();
}

char code[4] = {'1', '2', '3', '4'};
char input[4];

void lab1_2Loop() {
    // Clear the LCD
    putchar(0x1b);

    printf("Enter code: ");

    // Read a code from the keypad
    scanf("%c%c%c%c", &input[0], &input[1], &input[2], &input[3]);
    
    // Clear the LCD
    putchar(0x1b);

    // Print input code to console
    printf("Input code: %c%c%c%c\n", input[0], input[1], input[2], input[3]);
    
    delay(2000);

    // Clear the LCD
    putchar(0x1b);

    if (input[0] == code[0] && input[1] == code[1] && input[2] == code[2] && input[3] == code[3]) {
        printf("Correct code!");
        ddLedTurnOn(LED_GREEN);
        ddLedTurnOff(LED_RED);
        delay(1000);
       
    } else {
        printf("Wrong code!");
        ddLedTurnOn(LED_RED);
        ddLedTurnOff(LED_GREEN);
        delay(1000);
        
    }
}