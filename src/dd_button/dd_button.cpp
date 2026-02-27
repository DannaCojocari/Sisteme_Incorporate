#include "dd_button.h"

void ddButtonSetup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

int ddButtonIsPressed() {
    return digitalRead(BUTTON_PIN) == LOW;
}