#ifndef DD_LED_H
#define DD_LED_H

#include <Arduino.h>

// LED array configuration
#define LED_COUNT 3

// LED indices for easy reference
#define LED_GREEN 0
#define LED_RED 1
#define LED_YELLOW 2

// Default pin assignments (can be modified based on hardware)
#define LED_GREEN_PIN 12
#define LED_RED_PIN 11
#define LED_YELLOW_PIN 10

void ddLedSetup();
void ddLedTurnOn(int ledIndex);
void ddLedTurnOff(int ledIndex);

#endif //DD_LED_H
