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
  // Drive motors
  #define MOTOR_DRIVE_FR 2 // Front right drive motor, motor 2
  #define MOTOR_DRIVE_BR 3 // Back  right drive motor, motor 3
  #define MOTOR_DRIVE_FL 8 // Front left  drive motor, motor 8
  #define MOTOR_DRIVE_BL 9 // Back  left  drive motor, motor 9

  // Drive directions
  #define MOTOR_DRIVE_FR_REV true  // Front right drive motor, motor 2
  #define MOTOR_DRIVE_BR_REV true  // Back  right drive motor, motor 3
  #define MOTOR_DRIVE_FL_REV false // Front left  drive motor, motor 8
  #define MOTOR_DRIVE_BL_REV false // Back  right drive motor, motor 9

  // Lift
  #define MOTOR_LIFT_R1 4  // Lift 1, motor 4
  #define MOTOR_LIFT_R2 5  // Lift 2, motor 5
  #define MOTOR_LIFT_L2 6  // Lift 3, motor 6

  // Chain bar
  #define MOTOR_CHAIN_BAR 1  // Right claw, motor 1

/*
** Analog
** (1-8)
*/
  #define ANALOG_POW_EXPAND 1       // Power expander,     analog 1
  #define ANALOG_GYRO 2             // Gyroscope,          analog 2
  #define ANALOG_POT_MOGO 3         // Mogo potentiometer, analog 3
  #define ANALOG_LINE_LEFT_FRONT  4 // Line Sensor Left,   analog 4
  #define ANALOG_LINE_RIGHT_FRONT 5 // Line Sensor Right,  analog 5
  #define ANALOG_LINE_LEFT_BACK  7  // Line Sensor Left,   analog 7
  #define ANALOG_LINE_RIGHT_BACK 6  // Line Sensor Right,  analog 6

/*
** Digital
** (1-12) - Although not sure with PROS
*/
  // Auton
  #define DIGITAL_BREAKPOINT     1   // Breakpoint button          , digital 1
  #define DIGITAL_LIFT_ALIGN     6   // Lift deploy sensor         , digital 6
  // Claw
  #define DIGITAL_ENC_CHAIN_TOP 2   // Encoder chain top     right, digital 2
  #define DIGITAL_ENC_CHAIN_BOT 3   // Encoder chain bottom  right, digital 3
  #define DIGITAL_ENC_LIFT_TOP 4    // Encoder lift  top     left,  digital 4
  #define DIGITAL_ENC_LIFT_BOT 5    // Encoder lift  bottom  left,  digital 5
  // Drive
  #define DIGITAL_ENC_DRIVE_BR_TOP 7  // Encoder drive top    right, digital 7
  #define DIGITAL_ENC_DRIVE_BR_BOT 8  // Encoder drive bottom right, digital 8
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
