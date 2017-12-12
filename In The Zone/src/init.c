#include "main.h"

Gyro gyro;

Gyro getGyro() { return gyro; }

Encoder encDriveBR;
Encoder encDriveBL;
Encoder encChain;
Encoder encLift;

Encoder getEncoderBR() { return encDriveBR; }
Encoder getEncoderBL() { return encDriveBL; }
Encoder getEncoderChain() { return encChain; }
Encoder getEncoderLift() { return encLift; }

void killDriveEncoders() {
  encoderShutdown(encDriveBL);
  encoderShutdown(encDriveBR);
}

void initDriveEncoders() {
  // Init right encoder, reverse
  encDriveBR =
      encoderInit(DIGITAL_ENC_DRIVE_BR_TOP, DIGITAL_ENC_DRIVE_BR_BOT, true);
  // Init left encoder, not reversed
  encDriveBL =
      encoderInit(DIGITAL_ENC_DRIVE_BL_TOP, DIGITAL_ENC_DRIVE_BL_BOT, false);
}

/*
 * Runs pre-initialization code. This function will be started in kernel mode
 * one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API
 * functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes
 * (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It
 * can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */
void initializeIO() {}

/*
 * Runs user initialization code. This function will be started in its own task
 * with the default
 * priority and stack size once when the robot is starting up. It is possible
 * that the VEXnet
 * communication link may not be fully established at this time, so reading from
 * the VEX
 * Joystick may fail.
 *
 * This function should initialize most sensors (gyro, encoders, ultrasonics),
 * LCDs, global
 * variables, and IMEs.
 *
 * This function must exit relatively promptly, or the operatorControl() and
 * autonomous() tasks
 * will not start. An autonomous mode selection menu like the pre_auton() in
 * other environments
 * can be implemented in this task if desired.
 */
void initialize() {
  print("[Init] Initializing the robot\n");

  // Set up the LCD and start it
  print("[Init] Setting up the LCD\n");
  lcdInitMenu(1, 3, 1); // Min 1, max 3, home 1
  lcdSetUpdater(implUpdateLCD);
  lcdSetMenuBack(implMenuBack);
  lcdSetMenuNext(implMenuNext);
  lcdSetCycles(true);

  // Set up our drive
  print("[Init] Setting up drive motors\n");
  lcdSetText(uart2, 1, "Init drive...");
  driveInit(MOTOR_DRIVE_L, 0, MOTOR_DRIVE_R, 0);
  driveSetReverse(MOTOR_DRIVE_L_REV, false, MOTOR_DRIVE_R_REV, 0);
  // enableSlew(15); // Set slew rate to 15

  // Set up our autonomous to these modes
  print("[Init] Setting up autonomous modes\n");
  lcdSetText(uart2, 1, "Init auton...");
  autonInit(4); // 3 auton modes
  setAutonMode(3);

  // Set up our gyroscope
  print("[Init] Setting gyroscope\n");
  lcdSetText(uart2, 1, "Init gyro...");
  // To tune: 196*((360*rotations)/gyroValue)
  gyro = gyroInit(ANALOG_GYRO, 190); // default is 196, this is after tune

  // Set up our encoders
  print("[Init] Setting up encoders\n");
  lcdSetText(uart2, 1, "Init Encs...");
  initDriveEncoders();
  encLift = encoderInit(DIGITAL_ENC_LIFT_TOP, DIGITAL_ENC_LIFT_BOT, false);
  // initPidControl();

  // Done init
  print("[Init] Finished, starting LCD menu\n");
  lcdSetText(uart2, 1, "Init menu...");
  lcdStartMenu();

  // Init bluetooth
  // TODO Remove
  hc05Init(1, false);
}
