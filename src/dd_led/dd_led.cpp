#include "dd_led.h"

void ddLedSetup() {
    pinMode(LED_PIN, OUTPUT);
}

void ddLedTurnOn() {
    digitalWrite(LED_PIN, HIGH);
}

void ddLedTurnOff() {
    digitalWrite(LED_PIN, LOW);
}
