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
  #define MOTOR_DRIVE_BL 9 // Back  left  drive motor, motor 9

  // Drive
  #define MOTOR_DRIVE_FR_REV true  // Front right drive motor, motor 2
  #define MOTOR_DRIVE_BR_REV true  // Back  right drive motor, motor 3
  #define MOTOR_DRIVE_FL_REV false // Front left  drive motor, motor 8
  #define MOTOR_DRIVE_BL_REV false // Back  right drive motor, motor 9

  // Lift
  #define MOTOR_LIFT_R1 4  // Right lift 1, motor 4
  #define MOTOR_LIFT_R2 5  // Right lift 2, motor 5
  #define MOTOR_LIFT_L2 6  // Left  lift 2, motor 6
  #define MOTOR_LIFT_L1 7  // Left  lift 1, motor 7

  // Claw
  #define MOTOR_CLAW_R     1  // Right claw, motor 1
  #define MOTOR_CLAW_L     10 // Left  claw, motor 10

/*
** Analog
** (1-8)
*/
  #define ANALOG_POW_EXPAND 1 // Power expander,    analog 1
  #define ANALOG_GYRO 2       // Gyroscope,         analog 2
  #define ANALOG_POT_LIFT 3   // Arm potentiometer, analog 3
  #define ANALOG_LINE_LEFT  4 // Line Sensor Left,  analog 4
  #define ANALOG_LINE_RIGHT 5 // Line Sensor Right, analog 5

/*
** Digital
** (1-12) - Although not sure with PROS
*/
  // Auton
  #define DIGITAL_BREAKPOINT     1   // Breakpoint button         , digital 1
  #define DIGITAL_ARM_DEPLOY     6   // Arm deploy button         , digital 6
  // Claw
  #define DIGITAL_ENC_CLAW_R_TOP 2   // Encoder claw top     right, digital 2
  #define DIGITAL_ENC_CLAW_R_BOT 3   // Encoder claw bottom  right, digital 3
  #define DIGITAL_ENC_CLAW_L_TOP 4   // Encoder claw top     left,  digital 4
  #define DIGITAL_ENC_CLAW_L_BOT 5   // Encoder claw bottom  left,  digital 5
  // Drive
  #define DIGITAL_ENC_DRIVE_BR_TOP 8  // Encoder drive top    right, digital 8
  #define DIGITAL_ENC_DRIVE_BR_BOT 9  // Encoder drive bottom right, digital 9
  #define DIGITAL_ENC_DRIVE_BL_TOP 11 // Encoder drive top    left,  digital 11
  #define DIGITAL_ENC_DRIVE_BL_BOT 12 // Encoder drive bottom left,  digital 12

/*
** I2C
** (0-IME_ADDR_MAX, AKA 0x1F)
*/

#ifdef __cplusplus
}
#endif

#endif
