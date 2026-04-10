#ifndef LAB_5_1_TASK_SENSOR_H
#define LAB_5_1_TASK_SENSOR_H

// Task 1 – Sensor Acquisition (500 ms)
// Reads temperature from DHT22 and publishes to shared state.
// Sets sensor_error = true if the read returns NAN.
void l51_taskSensor(void *pvParameters);

#endif // LAB_5_1_TASK_SENSOR_H