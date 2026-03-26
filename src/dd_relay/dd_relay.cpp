#include "dd_relay.h"

static bool relay_state = false;

void ddRelaySetup() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    relay_state = false;
}

void ddRelayOn() {
    digitalWrite(RELAY_PIN, HIGH);
    relay_state = true;
}

void ddRelayOff() {
    digitalWrite(RELAY_PIN, LOW);
    relay_state = false;
}

bool ddRelayIsOn() {
    return relay_state;
}