#ifndef LAB_4_2_TASK_SERVO_CTRL_H
#define LAB_4_2_TASK_SERVO_CTRL_H

// Task 1 – Servo Control (50 ms)
// Reads the conditioned command from shared state and applies ramp-limited
// position changes to the servo.  This is the SOLE task that writes to hardware.
//
// Ramp logic:
//   Each 50 ms cycle the actual servo position may change by at most
//   RAMP_STEP_PCT.  This prevents abrupt mechanical jumps on large
//   command changes and protects the servo gearbox.
//
// Exposes: actuator_get_speed() — current ramped position in %.
void l42_taskServoCtrl(void *pvParameters);

// Public read-only interface used by other modules.
float actuator_get_speed();

#endif // LAB_4_2_TASK_SERVO_CTRL_H