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


// Define the function to get a character from the serial driver.
// Non-blocking: returneaza EOF daca nu exista date disponibile,
// in loc sa astepte. Aceasta permite FreeRTOS sa continue schedulingul
// celorlalte taskuri in loc sa fie blocat pe stdin.
int srvSerialGetChar(FILE *f) {
    if (!Serial.available()) return EOF;
    return Serial.read();
}


// Define the function to put a character to the serial driver.
// Translates '\n' -> '\r\n': without the carriage return (\r), each new line
// starts at the column where the previous one ended, causing a staircase effect.
int srvSerialPutChar(char c, FILE *f) {
    if (c == '\n') Serial.write('\r');
    return Serial.write(c);
}