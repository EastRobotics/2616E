#include "JINX.h"
#include "main.h"

Gyro gyro;

Gyro getGyro() { return gyro; }

Encoder encDriveBR;
Encoder encDriveBL;

Encoder getEncoderBR() { return encDriveBR; }

Encoder getEncoderBL() { return encDriveBL; }

void killDriveEncoders() {
  encoderShutdown(encDriveBL);
  encoderShutdown(encDriveBR);
}

void initDriveEncoders() {
  // Init right encoder, reverse
  encDriveBR =
      encoderInit(DIGITAL_ENC_DRIVE_BR_TOP, DIGITAL_ENC_DRIVE_BR_BOT, true);
  // Init left encoder, reverse
  encDriveBL =
      encoderInit(DIGITAL_ENC_DRIVE_BL_TOP, DIGITAL_ENC_DRIVE_BL_BOT, true);
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
  driveInit(MOTOR_DRIVE_FL, MOTOR_DRIVE_BL, MOTOR_DRIVE_FR, MOTOR_DRIVE_BR);
  driveSetReverse(MOTOR_DRIVE_FL_REV, MOTOR_DRIVE_BL_REV, MOTOR_DRIVE_FR_REV,
                  MOTOR_DRIVE_BR_REV);
  // enableSlew(15); // Set slew rate to 15

  // Set up our autonomous to these modes
  print("[Init] Setting up autonomous modes\n");
  lcdSetText(uart2, 1, "Init auton...");
  autonInit(4); // 3 auton modes

  // Set up our gyroscope
  print("[Init] Setting gyroscope\n");
  lcdSetText(uart2, 1, "Init gyro...");
  // To tune: 196*((360*rotations)/gyroValue)
  gyro = gyroInit(ANALOG_GYRO, 190); // default is 196, this is after tune

  // Set up our encoders
  print("[Init] Setting up encoders\n");
  lcdSetText(uart2, 1, "Init Encs...");
  initDriveEncoders();
  // initPidControl();

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
  lcdStartMenu();

  // TODO Remove
  hc05Init(1, false);
}
