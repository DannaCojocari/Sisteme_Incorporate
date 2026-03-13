#include "dd_ntc.h"
#include <math.h>

void ddNtcSetup() {
    // Analog pins are inputs by default; no explicit pinMode needed.
    // Configure internal ADC reference to DEFAULT (VCC = 5 V).
    analogReference(DEFAULT);
}

int ddNtcReadRaw() {
    return analogRead(NTC_PIN);
}

float ddNtcReadTemperature() {
    int raw = analogRead(NTC_PIN);
    // NTC thermistor in voltage divider: VCC -- R_series(10k) -- A0 -- NTC -- GND
    // Beta equation: T(K) = 1 / (1/T0 + (1/B)*ln(R/R0))
    const float R_series  = 10000.0f;
    const float R0        = 10000.0f;  // NTC nominal resistance at 25 °C
    const float T0        = 298.15f;   // 25 °C in Kelvin
    const float B         = 3950.0f;   // NTC Beta coefficient
    // log(R0) == log(10000) precomputed to avoid a second log() call
    const float log_R0    = 9.21034f;  // ln(10000)
    if (raw <= 0 || raw >= 1023) return NAN;
    float R_ntc = R_series * (float)raw / (1023.0f - (float)raw);
    float T_K   = 1.0f / (1.0f / T0 + (1.0f / B) * (log(R_ntc) - log_R0));
    return T_K - 273.15f;
}
