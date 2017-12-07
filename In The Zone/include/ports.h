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
  #define MOTOR_DRIVE_R 2 // Front right drive motor, motor 2
  #define MOTOR_DRIVE_L 9 // Back  left  drive motor, motor 9

  // Drive directions
  #define MOTOR_DRIVE_R_REV true  // Front right drive motor, motor 2
  #define MOTOR_DRIVE_L_REV false // Back  right drive motor, motor 9


  // Lift
  #define MOTOR_LIFT_1 4  // Lift 1, motor 4
  #define MOTOR_LIFT_2 7  // Lift 2, motor 7

  // Chain bar
  #define MOTOR_CHAIN_BAR 8  // Right claw, moto

  // MoGo Intake
  #define MOTOR_MOGO_L 5 // Left mogo intake,  motor 5
  #define MOTOR_MOGO_R 6 // Right mogo intake, motor 6

  // Claw
  #define MOTOR_CLAW 3 // Claw, motor 3

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
** Can't use 4 & 10 together because interrupts
*/
  // Auton
  #define DIGITAL_BREAKPOINT     1   // Breakpoint button,           digital 1
  // Claw
  #define DIGITAL_ENC_LIFT_TOP 2    // Encoder lift  top     left,   digital 2
  #define DIGITAL_ENC_LIFT_BOT 3    // Encoder lift  bottom  left,   digital 3
  #define DIGITAL_LIM_CLAW     4    // Encoder limit claw,           digital 4
  // Drive
  #define DIGITAL_ENC_DRIVE_BR_TOP 5  // Encoder drive top    right, digital 5
  #define DIGITAL_ENC_DRIVE_BR_BOT 6  // Encoder drive bottom right, digital 6
  #define DIGITAL_ENC_DRIVE_BL_TOP 7 // Encoder drive top      left, digital 7
  #define DIGITAL_ENC_DRIVE_BL_BOT 8 // Encoder drive bottom   left, digital 8
  // #define DO_NOT_USE 10


/*
** I2C (WARNING: STATIC SHOCK)
** (0-IME_ADDR_MAX, AKA 0x1F)
*/

#ifdef __cplusplus
}
#endif

#endif
