#include "dd_servo.h"

// Servo PWM parameters (standard hobby servo):
//   Period  = 20 ms  → 50 Hz
//   Min pulse = 544 µs  → 0°
//   Max pulse = 2400 µs → 180°
//
// On the Mega, Timer1/3/4/5 give 16-bit PWM.
// We use writeMicroseconds-style math but via analogWrite workaround:
// Since analogWrite only gives 0-255 duty over the full period, we
// instead bit-bang the pulse in a lightweight way using a dedicated
// hardware compare — but the simplest conflict-free approach is to
// use the AVR ICR/OCR registers directly on Timer4 (pin 6 on Mega).

#define SERVO_MIN_US   544
#define SERVO_MAX_US   2400
#define TIMER4_TOP     39999   // 20 ms period at 2 MHz (prescaler=8, 16MHz/8=2MHz, 2e6/50=40000 ticks, TOP=39999)

static int   s_pin        = 6;
static float s_current_pct = 0.0f;

static void _write_us(uint16_t us) {
    // OCR4A controls OC4A = pin 6 on Mega2560
    OCR4A = (uint16_t)((uint32_t)us * 2);  // 2 ticks per µs at 2 MHz
}

void ddServoSetup(int pin) {
    s_pin = pin;
    s_current_pct = 0.0f;

    // Configure Timer4 for Fast PWM, non-inverting on OC4A (pin 6)
    pinMode(pin, OUTPUT);

    // WGM4 = 14 (Fast PWM, TOP = ICR4)
    // CS41  = prescaler /8 → 2 MHz tick
    // COM4A1 = non-inverting
    TCCR4A = (1 << COM4A1) | (1 << WGM41);
    TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41);
    ICR4   = TIMER4_TOP;
    OCR4A  = (uint16_t)(SERVO_MIN_US * 2);  // start at 0°
}

void ddServoSetPercent(float pct) {
    if (pct < 0.0f)   pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;
    s_current_pct = pct;

    uint16_t us = (uint16_t)(SERVO_MIN_US + (pct / 100.0f) * (SERVO_MAX_US - SERVO_MIN_US));
    _write_us(us);
}

float ddServoGetPercent() {
    return s_current_pct;
}

int ddServoGetAngle() {
    return (int)((s_current_pct / 100.0f) * 180.0f);
}