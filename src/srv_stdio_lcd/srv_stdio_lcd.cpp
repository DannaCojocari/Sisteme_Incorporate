#include "srv_stdio_lcd.h"

#include <LiquidCrystal_I2C.h>

#include <stdio.h>

// Code ASCII used to clear the LCD
#define CLEAR_KEY 0x1b

int lcdColumns = 16;
int lcdRows = 2;

// Create an LCD object
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // Set the LCD I2C address, columns, and rows

int srvStdioLcdPutChar(char c, FILE *stream)
{
    // Print a character to the LCD
    if (c == CLEAR_KEY) {
        lcd.clear();
        lcd.setCursor(0, 0);
    } else {
        lcd.print(c);
    }
    return 0;
}

void srvStdioLcdSetup()
{
    // Initialize the LCD
    lcd.init();

    // Turn on the backlight
    lcd.backlight();

    // Set the cursor to the first column of the first row
    lcd.setCursor(0, 0);

    FILE *srvLcdStdioStream = fdevopen(srvStdioLcdPutChar, NULL);

    stdout = srvLcdStdioStream;
}

