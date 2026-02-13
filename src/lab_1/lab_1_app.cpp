#include "lab_1_app.h"

// Include the serial driver
#include "srv_serial_stdio/srv_serial_stdio.h"
#include <Arduino.h>

#include "dd_led/dd_led.h"

void lab1Setup() {
    // Initialize the serial driver
    srvSerialSetup();

    // Initialize the LED driver
    ddLedSetup();
    printf("Laboratory 1: SDTIO\n");
}

char cmd[32];
void lab1Loop() {
    printf("Enter command: led on / led off\n");
    fgets(cmd, sizeof(cmd), stdin);
    // Remove newline character from the end of the command
    cmd[strcspn(cmd, "\r\n")] = 0;

    printf("Command: %s\n", cmd);

    // Process the command
    if (strcmp(cmd, "led on") == 0) {
        ddLedTurnOn();
        printf("LED on\n");

    } else if (strcmp(cmd, "led off") == 0) {
        ddLedTurnOff();
        printf("LED off\n");
    } else {
        printf("Unknown command\n");
    }

    delay(1000);
}