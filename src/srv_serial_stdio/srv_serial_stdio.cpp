#include "srv_serial_stdio.h"

// Include device driver
#include <Arduino.h>
#include <stdio.h>


// Initialize driver
void srvSerialSetup() {
    Serial.begin(9600);

    // Create a stream that uses the serial driver
    FILE *srv_serial_stdio_stream = fdevopen(srvSerialPutChar, srvSerialGetChar);

    stdin = srv_serial_stdio_stream;
    stdout = srv_serial_stdio_stream;
    stderr = srv_serial_stdio_stream;
}


// Define the function to get a character from the serial driver
int srvSerialGetChar(FILE *f) {
    // Wait for data to be available
    while (!Serial.available()); 
    return Serial.read();
}


// Define the function to put a character to the serial driver
int srvSerialPutChar(char c, FILE *f) {
    // Write the character to the serial driver
    return Serial.write(c);
}