#include "dd_led.h"

// LED pins array
static const int ledPins[LED_COUNT] = {
    LED_GREEN_PIN,
    LED_RED_PIN
};

void ddLedSetup() {
    for (int i = 0; i < LED_COUNT; i++) {
        pinMode(ledPins[i], OUTPUT);
    }
}

void ddLedTurnOn(int ledIndex) {
    if (ledIndex >= 0 && ledIndex < LED_COUNT) {
        digitalWrite(ledPins[ledIndex], HIGH);
    }
}

void ddLedTurnOff(int ledIndex) {
    if (ledIndex >= 0 && ledIndex < LED_COUNT) {
        digitalWrite(ledPins[ledIndex], LOW);
    }
}
