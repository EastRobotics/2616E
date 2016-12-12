#include "main.h"

/*
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */
void initializeIO() {
}

void updateLCD(bool userCaused, int page) {
  lcdClear(EXTRA_LCD);
  lcdPrintTitle("Test page");
  lcdPrint(EXTRA_LCD, 2, "This is page %i", page);
}

void menuNext(int page) {
  // TODO Implement submenus
}

void menuBack(int page) {
  // TODO Implement submenus
}

/*
 * Runs user initialization code. This function will be started in its own task with the default
 * priority and stack size once when the robot is starting up. It is possible that the VEXnet
 * communication link may not be fully established at this time, so reading from the VEX
 * Joystick may fail.
 *
 * This function should initialize most sensors (gyro, encoders, ultrasonics), LCDs, global
 * variables, and IMEs.
 *
 * This function must exit relatively promptly, or the operatorControl() and autonomous() tasks
 * will not start. An autonomous mode selection menu like the pre_auton() in other environments
 * can be implemented in this task if desired.
 */
void initialize() {
  // Set up our drive
  driveInit(MOTOR_DRIVE_FL, MOTOR_DRIVE_BL, MOTOR_DRIVE_FR, MOTOR_DRIVE_BR);

  // Set up our autonomous to these modes
  const char* modes[3] = {"Mode one","Mode two","Mode three"};
  autonInit(modes);

  // Set up the LCD and start it
  lcdInitMenu(1,5,1);
  lcdSetUpdater(updateLCD);
  lcdSetMenuBack(menuBack);
  lcdSetMenuNext(menuNext);
}
