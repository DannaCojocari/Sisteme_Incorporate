#ifndef LAB_5_1_TASK_CONTROL_H
#define LAB_5_1_TASK_CONTROL_H

// Task 2 – ON-OFF Control with Hysteresis (500 ms)
//
// Actuators:
//   - Heater: relay on pin 7 (dd_relay driver, active HIGH)
//   - Fan:    DC motor via L298N (dd_motor driver, ENA=6 IN1=4 IN2=5)
//
// Control law (ON-OFF with hysteresis):
//   T > SP + HYST  →  relay OFF + fan 100%   (cool down)
//   T < SP - HYST  →  relay ON  + fan 0%     (heat up)
//   dead-band       →  hold current state     (prevents chatter)
//
// Does NOT actuate if sensor_error == true.
// Publishes relay_on and fan_on to shared state after each cycle.
void l51_taskControl(void *pvParameters);

#endif // LAB_5_1_TASK_CONTROL_H