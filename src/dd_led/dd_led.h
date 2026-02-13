#ifndef DD_LED_H
#define DD_LED_H

#include <Arduino.h>

// ESP32 built-in LED is on GPIO2
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define LED_PIN LED_BUILTIN

void ddLedSetup();
void ddLedTurnOn();
void ddLedTurnOff();

#endif //DD_LED_H
