#include "dd_dht11.h"

#include <DHT.h>

// Single static DHT object – owned exclusively by this driver.
// In Wokwi, use the wokwi-dht22 component (DHT22 protocol);
// on real hardware, use DHT11.
#ifdef WOKWI_SIMULATION
static DHT dht(DHT11_PIN, DHT22);
#else
static DHT dht(DHT11_PIN, DHT11);
#endif

void ddDht11Setup() {
    dht.begin();
}

float ddDht11ReadTemperature() {
    return dht.readTemperature(); // °C; returns NAN on error
}

