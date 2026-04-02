#ifndef DD_MOTOR_H
#define DD_MOTOR_H

#include <Arduino.h>

// ── Device driver: DC Motor via L298N ───────────────────────────────────────
// Single channel: ENA (PWM speed), IN1+IN2 (direction).
// Direction fixed FORWARD (IN1=HIGH, IN2=LOW).
// Speed: 0–100 % → analogWrite 0–255 on ENA pin.

void  ddMotorSetup(int ena, int in1, int in2);
void  ddMotorSetPercent(float pct);   // 0.0–100.0 %
void  ddMotorStop();                  // immediate stop (PWM = 0)
float ddMotorGetPercent();
int   ddMotorGetPwm();                // PWM value 0–255

#endif // DD_MOTOR_H