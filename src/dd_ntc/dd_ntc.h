#ifndef DD_NTC_H
#define DD_NTC_H

#include <Arduino.h>

// NTC thermistor analog temperature sensor driver
// Voltage divider: VCC -- R_series(10k) -- A0 -- NTC -- GND
// Beta equation:   T(K) = 1 / (1/T0 + (1/B)*ln(R/R0))

#define NTC_PIN     A0   // Analog input pin

void  ddNtcSetup();
int   ddNtcReadRaw();          // Returns raw 10-bit ADC value (0–1023)
float ddNtcReadTemperature();  // Returns temperature in °C

#endif // DD_NTC_H
