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
  #define MOTOR_CHOO_R1 1  // Right choo-choo 1, motor 1
  #define MOTOR_CHOO_R2 4  // Right choo-choo 2, motor 4
  #define MOTOR_CHOO_R3 5  // Right choo-choo 3, motor 5
  #define MOTOR_CHOO_L1 6  // Left  choo-choo 1, motor 6
  #define MOTOR_CHOO_L2 7  // Left  choo-choo 2, motor 7
  #define MOTOR_CHOO_L3 10 // Left  choo-choo 1, motor 10

/*
** Analog
** (1-8)
*/
  #define ANALOG_POW_EXPAND 1 // Power expander,     analog 1
  #define ANALOG_GYRO 2       // Gyroscope,          analog 2
  #define ANALOG_POT_LAUNCH 3 // Arm potentiometer,  analog 3

/*
** Digital
** (1-12) - Although not sure with PROS
*/

/*
** Extra
** (uart(1 & 2), I2C)
*/
  #define EXTRA_LCD uart2

#ifdef __cplusplus
}
#endif

#endif
