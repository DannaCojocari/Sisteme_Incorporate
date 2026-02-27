#ifndef DD_BUTTON_H
#define DD_BUTTON_H

#include "Arduino.h"

// Button pin definition
#define BUTTON_PIN 7

// Function declarations
void ddButtonSetup();
int ddButtonIsPressed();

#endif // DD_BUTTON_H