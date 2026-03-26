#ifndef DD_SERVO_H
#define DD_SERVO_H

#include <Arduino.h>

void  ddServoSetup(int pin);
void  ddServoSetPercent(float pct);   // 0.0–100.0 %
float ddServoGetPercent();
int   ddServoGetAngle();              // 0–180 degrees

#endif // DD_SERVO_H