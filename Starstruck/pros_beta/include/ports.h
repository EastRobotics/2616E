#ifndef PORTS_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define PORTS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Motors
** (1-10)
*/
  // Drive
  #define MOTOR_DRIVE_FR 2 // Front right drive motor, motor 2
  #define MOTOR_DRIVE_BR 3 // Back  right drive motor, motor 3
  #define MOTOR_DRIVE_FL 8 // Front left  drive motor, motor 8
  #define MOTOR_DRIVE_BL 9 // Back  right drive motor, motor 9

  // Choo-choo launcher
  #define MOTOR_CHOO_R1    4  // Right choo-choo 1,   motor 4
  #define MOTOR_Y_CHOO_R23 5  // Right choo-choo 2+3, motor 5
  #define MOTOR_Y_CHOO_L23 6  // Left  choo-choo 2+3, motor 6
  #define MOTOR_CHOO_L1    7  // Left  choo-choo 1,   motor 7

  // Claw
  #define MOTOR_CLAW_R     1  // Right claw, motor 1
  #define MOTOR_CLAW_L     10 // Left  claw, motor 10

/*
** Analog
** (1-8)
*/
  #define ANALOG_POW_EXPAND 1 // Power expander,     analog 1
  #define ANALOG_GYRO 2       // Gyroscope,          analog 2
  #define ANALOG_POT_LAUNCH 3 // Arm potentiometer,  analog 3
  #define ANALOG_POT_CLAW_L 4 // Claw pot Left,      analog 4
  #define ANALOG_POT_CLAW_R 5 // Claw pot Right,     analog 5

/*
** Digital
** (1-12) - Although not sure with PROS
*/

#ifdef __cplusplus
}
#endif

#endif
