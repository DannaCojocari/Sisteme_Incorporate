#include "srv_stdio_keypad.h"

// Include STDIO library
#include <stdio.h>

// Include keypad library
#include <Keypad.h>

#define SRV_KEYPAD_REPEAT_DELAY 100

// Configuration for the keypad
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns

// Define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {2, 3, 4, 5}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; // connect to the column pinouts of the keypad

// initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Define the function to get a character from the keypad driver
int srvStdioKeypadGetKey(FILE *stream)
{
    char customKey;
    do
    {
        customKey = customKeypad.getKey();
    } while (customKey==NO_KEY);

    delay(SRV_KEYPAD_REPEAT_DELAY); // Read frequency 1/SRV_KEYPAD_REPEAT_DELAY Hz

    return customKey;
}


void srvStdioKeypadSetup()
{
    // Does not require any setup

    // Define stream
    FILE *srvKeypadStdioStream = fdevopen(NULL, srvStdioKeypadGetKey);

    // Link stream to keypad
    stdin = srvKeypadStdioStream;
}


