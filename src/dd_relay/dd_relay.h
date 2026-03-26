#ifndef DD_RELAY_H
#define DD_RELAY_H

#include <Arduino.h>

// Relay module control pin (active HIGH: HIGH = relay ON = bulb ON)
#define RELAY_PIN 7

void ddRelaySetup();
void ddRelayOn();
void ddRelayOff();
bool ddRelayIsOn();

#endif // DD_RELAY_H