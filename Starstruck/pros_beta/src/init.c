#include "main.h"
#include "JINX.h"

Gyro gyro;

Gyro getGyro() { return gyro; }

Encoder encDriveBR;
Encoder encDriveBL;
Encoder encClawL;
Encoder encClawR;

Encoder getEncoderBR() { return encDriveBR; }

Encoder getEncoderBL() { return encDriveBL; }

Encoder getEncoderClawL() { return encClawL; }

Encoder getEncoderClawR() { return encClawR; }

void killDriveEncoders() {
  encoderShutdown(encDriveBL);
  encoderShutdown(encDriveBR);
}

void initDriveEncoders() {
  // Init right encoder, not reverse
  encDriveBR =
      encoderInit(DIGITAL_ENC_DRIVE_BR_TOP, DIGITAL_ENC_DRIVE_BR_BOT, false);
  // Init left encoder, not reverse
   encDriveBL = encoderInit(DIGITAL_ENC_DRIVE_BL_TOP,
    DIGITAL_ENC_DRIVE_BL_BOT,false);
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
  lcdInitMenu(1, 9, 4); // Min 1, max 8, home 4
  lcdSetUpdater(implUpdateLCD);
  lcdSetMenuBack(implMenuBack);
  lcdSetMenuNext(implMenuNext);

  // Set up our drive
  print("[Init] Setting up drive motors\n");
  lcdSetText(uart2, 1, "Init drive...");
  driveInit(MOTOR_DRIVE_FL, MOTOR_DRIVE_BL, MOTOR_DRIVE_FR, MOTOR_DRIVE_BR);
  driveSetReverse(MOTOR_DRIVE_FL_REV, MOTOR_DRIVE_BL_REV, MOTOR_DRIVE_FR_REV,
                  MOTOR_DRIVE_BR_REV);
  // enableSlew(15); // Set slew rate to 25

  // Set up our autonomous to these modes
  print("[Init] Setting up autonomous modes\n");
  lcdSetText(uart2, 1, "Init auton...");
  autonInit(7);

  // Set up our gyroscope
  print("[Init] Setting gyroscope\n");
  lcdSetText(uart2, 1, "Init gyro...");
  gyro = gyroInit(ANALOG_GYRO, 0); // 0 multiplier = default, not * 0

  // Set up our encoders
  print("[Init] Setting up encoders\n");
  lcdSetText(uart2, 1, "Init Encs...");
  initDriveEncoders();
  // Init right encoder, not reverse
  encClawR = encoderInit(DIGITAL_ENC_CLAW_R_TOP, DIGITAL_ENC_CLAW_R_BOT, false);
  // Init left encoder, reversed
  encClawL = encoderInit(DIGITAL_ENC_CLAW_L_TOP, DIGITAL_ENC_CLAW_L_BOT, true);
  initPidControl();


  // Sets communication port for JINX data and start task to parse incoming
  // messages.
  print("[Init] Setting up JINX\n");
  lcdSetText(uart2, 1, "Init JINX...");
  initJINX(stdout);
  delay(100);
  taskCreate(JINXRun, TASK_DEFAULT_STACK_SIZE, NULL, (TASK_PRIORITY_DEFAULT));
  delay(100);

  // Done init
  print("[Init] Finished, starting LCD menu\n");
  lcdSetText(uart2, 1, "Init menu...");
  lcdSetCycles(false);
  lcdStartMenu();
}
