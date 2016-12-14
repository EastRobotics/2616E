#include "main.h"

Gyro gyro;

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
  lcdClear(uart2);
  switch (page) {
    // [Page 1] Auton mode selector --------------------------------------------
    case 1:
      {
        lcdPrintTitle("Auton Mode");
        printf("Auton mode: %d\n",getAutonMode());
        const char* autonName;
        switch (getAutonMode()) {
          case 1:
            autonName = "Mode 1";
            break;
          case 2:
            autonName = "Mode 2";
            break;
          default: // Unknown mode
            autonName = "Unknown";
        }
        lcdPrint(uart2, 2, "%c %s %c",0xBC, autonName, 0xBB);
      }
      break;

      // [Page 1] Auton mode selector --------------------------------------------
      case 2:
        {
          lcdPrintTitle("Gyro");
          printf("Gyro: %d\n",gyroGet(gyro));
          lcdPrint(uart2, 2, "Gyro: %d\n",gyroGet(gyro));
        }
        break;

    // [Page ?] Unknown page ---------------------------------------------------
    default:
      lcdPrint(uart2, 1, "Page? %d",page);
      break;
    // -------------------------------------------------------------------------
  }
}

void menuNext(int page) {
  switch (page) {
    // [Page 1] Auton mode selector --------------------------------------------
    case 1:
      {
        int nextMode = getAutonMode()+1;
        if (nextMode > getAutonModeMax())
          setAutonMode(getAutonModeMin());
        else
          setAutonMode(nextMode);
      }
      break;
    // -------------------------------------------------------------------------
  }
}

void menuBack(int page) {
  switch (page) {
    // [Page 1] Auton mode selector --------------------------------------------
    case 1:
      {
        int nextMode = getAutonMode()-1;
        if (nextMode < getAutonModeMin())
          setAutonMode(getAutonModeMax());
        else
          setAutonMode(nextMode);
      }
      break;
    // -------------------------------------------------------------------------
  }
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
  print("[Init] Initializing the robot\n");

  // Set up the LCD and start it
  print("[Init] Setting up the LCD\n");
  lcdInitMenu(1,8,4);
  lcdSetUpdater(updateLCD);
  lcdSetMenuBack(menuBack);
  lcdSetMenuNext(menuNext);

  // Set up our drive
  print("[Init] Setting up drive motors\n");
  lcdSetText(uart2, 1, "Init drive...");
  driveInit(MOTOR_DRIVE_FL, MOTOR_DRIVE_BL, MOTOR_DRIVE_FR, MOTOR_DRIVE_BR);

  // Set up our autonomous to these modes
  print("[Init] Setting up autonomous modes\n");
  lcdSetText(uart2, 1, "Init auton...");
  autonInit(4);

  // Set up our autonomous to these modes
  print("[Init] Setting gyroscope\n");
  lcdSetText(uart2, 1, "Init gyro...");
  gyro = gyroInit(ANALOG_GYRO, 0); // 0 multiplier = default, not * 0

  // Done init
  print("[Init] Finished, starting LCD menu\n");
  lcdSetText(uart2, 1, "Init menu...");
  lcdStartMenu();
}
