#ifndef DD_DHT11_H
#define DD_DHT11_H

// DHT11 digital temperature and humidity sensor driver

#define DHT11_PIN  18   // Digital I/O pin connected to DHT11 DATA line

void ddDht11Setup();

// Returns temperature in °C, or NAN on read error.
float ddDht11ReadTemperature();

#endif // DD_DHT11_H
