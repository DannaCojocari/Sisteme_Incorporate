#include "dd_motor.h"

static int   s_ena_pin     = 9;
static int   s_in1_pin     = 30;
static int   s_in2_pin     = 31;
static float s_current_pct = 0.0f;

void ddMotorSetup(int ena, int in1, int in2) {
    s_ena_pin = ena;
    s_in1_pin = in1;
    s_in2_pin = in2;
    s_current_pct = 0.0f;

    pinMode(s_ena_pin, OUTPUT);
    pinMode(s_in1_pin, OUTPUT);
    pinMode(s_in2_pin, OUTPUT);

    // Fixed direction: FORWARD
    digitalWrite(s_in1_pin, HIGH);
    digitalWrite(s_in2_pin, LOW);

    // Motor initially stopped
    analogWrite(s_ena_pin, 0);
}

void ddMotorSetPercent(float pct) {
    if (pct < 0.0f)   pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;
    s_current_pct = pct;

    int pwm = (int)((pct / 100.0f) * 255.0f);
    analogWrite(s_ena_pin, pwm);
}

void ddMotorStop() {
    s_current_pct = 0.0f;
    analogWrite(s_ena_pin, 0);
}

float ddMotorGetPercent() {
    return s_current_pct;
}

int ddMotorGetPwm() {
    return (int)((s_current_pct / 100.0f) * 255.0f);
}