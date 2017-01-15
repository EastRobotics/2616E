#include "main.h"

Gyro gyro;

Gyro getGyro() {
    return gyro;
}

Encoder driveBR;
Encoder driveBL;
Encoder clawL;
Encoder clawR;

Encoder getEncoderBR() {
  return driveBR;
}

Encoder getEncoderBL() {
  return driveBL;
}

Encoder getEncoderClawL() {
  return clawL;
}

Encoder getEncoderClawR() {
  return clawR;
}

void killDriveEncoders() {
  encoderShutdown(driveBL);
  encoderShutdown(driveBR);
}

void initDriveEncoders() {
  // Init right encoder, not reverse
  driveBR = encoderInit(DIGITAL_ENC_DRIVE_BR_TOP,
    DIGITAL_ENC_DRIVE_BR_BOT,false);
  // Init left encoder, not reverse
  driveBL = encoderInit(DIGITAL_ENC_DRIVE_BL_TOP,
    DIGITAL_ENC_DRIVE_BL_BOT,false);
}

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
        lcdPrintTitle("AutonMode");
        const char* autonName;
        switch (getAutonMode()) {
          case 1:
            autonName = "Do Nothing";
            break;
          case 2:
            autonName = "Max Points";
            break;
          default: // Unknown mode
            autonName = "Unknown";
        }
        char temp[16];
        sprintf(temp,"%c %s %c",0xBC, autonName, 0xBB);
        //lcdPrint(uart2, 2, "%c %s %c",0xBC, autonName, 0xBB);
        lcdPrintCentered(temp,2);
      }
      break;

      // [Page 2] Auton position selector --------------------------------------
      case 2:
        {
          lcdPrintTitle("AutonPos");
          char temp[16];
          sprintf(temp,"%c %s %c",0xBC, getAutonPosition() ? "Right"
          : "Left", 0xBB);
          lcdPrintCentered(temp,2);
        }
        break;

      // [Page 3] Auton color selector -----------------------------------------
      case 3:
        {
          lcdPrintTitle("AutonColor");
          char temp[16];
          sprintf(temp,"%c %s %c",0xBC, getAutonColor() ? "Red"
          : "Blue", 0xBB);
          lcdPrintCentered(temp,2);
        }
        break;

      // [Page 4] Home page ----------------------------------------------------
      case 4:
        {
          lcdPrint(uart2, 1, "%cEssential Evil%c",0xCD,0xCD);
          char batWarning = ' ';
          // If really bad
          if (batteryStatus(voltLevelExpander(ANALOG_POW_EXPAND, true)) >= 2 ||
            batteryStatus(voltLevelMain()) >= 2) {
            batWarning = 0xF5; // Noticeable down arrow thing
          // If pretty bad
          }
          else if (batteryStatus(voltLevelExpander(ANALOG_POW_EXPAND, true))
            >= 1 || batteryStatus(voltLevelMain()) >= 1) {
            batWarning = 0x21;
          }
          lcdPrint(uart2, 2,"%cbat | aut%d",batWarning,getAutonMode());
        }
        break;

      // [Page 5] Battery Stat -------------------------------------------------
      case 5:
        {
          lcdPrint(uart2, 1, "%s%c B:%.2fv", "BatStat", 0xF6, voltLevelBackup());
          lcdPrint(uart2, 2, " M:%.2fv E:%.2fv", voltLevelMain(),
            voltLevelExpander(ANALOG_POW_EXPAND, true));
        }
        break;
      // [Page 6] Claw pots ----------------------------------------------------
      case 6:
        {
          lcdPrintTitle("ClawPots");
          int clawPosLeft = encoderGet(clawL);
      		int clawPosRight = encoderGet(clawR);
          lcdPrint(uart2, 2, "L:%.4d R:%.4d", clawPosLeft, clawPosRight);
        }
        break;
      // [Page 7] Line sensors -------------------------------------------------
      case 7:
        {
          lcdPrintTitle("LineSensors");
          lcdPrint(uart2, 2, "L: %.4d R: %.4d", analogRead(ANALOG_LINE_LEFT),
          analogRead(ANALOG_LINE_RIGHT));
        }
        break;
      // [Page 8] Arm pot ------------------------------------------------------
      case 8:
        {
          lcdPrintTitle("ArmPot");
          lcdPrint(uart2, 2, "Arm: %.4d", analogRead(ANALOG_POT_LIFT));
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
    // [Page 2] Auton position selector --------------------------------------
    case 2:
      {
        setAutonPosition(!getAutonPosition());
      }
      break;
    // [Page 3] Auton color selector -------------------------------------------
    case 3:
      {
        setAutonColor(!getAutonColor());
      }
      break;
    // -------------------------------------------------------------------------
  }
  updateLCD(true, page);
  lcdResetAutoRefresh();
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
    // [Page 2] Auton position selector --------------------------------------
    case 2:
      {
        setAutonPosition(!getAutonPosition());
      }
      break;
    // [Page 3] Auton color selector -------------------------------------------
    case 3:
      {
        setAutonColor(!getAutonColor());
      }
      break;
    // -------------------------------------------------------------------------
  }
  updateLCD(true, page);
  lcdResetAutoRefresh();
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
  lcdInitMenu(1,8,4); // Min 1, max 8, home 4
  lcdSetUpdater(updateLCD);
  lcdSetMenuBack(menuBack);
  lcdSetMenuNext(menuNext);

  // Set up our drive
  print("[Init] Setting up drive motors\n");
  lcdSetText(uart2, 1, "Init drive...");
  driveInit(MOTOR_DRIVE_FL, MOTOR_DRIVE_BL, MOTOR_DRIVE_FR, MOTOR_DRIVE_BR);
  driveSetReverse(MOTOR_DRIVE_FL_REV, MOTOR_DRIVE_BL_REV, MOTOR_DRIVE_FR_REV,
    MOTOR_DRIVE_BR_REV);

  // Set up our autonomous to these modes
  print("[Init] Setting up autonomous modes\n");
  lcdSetText(uart2, 1, "Init auton...");
  autonInit(4);

  // Set up our autonomous to these modes
  print("[Init] Setting gyroscope\n");
  lcdSetText(uart2, 1, "Init gyro...");
  gyro = gyroInit(ANALOG_GYRO, 0); // 0 multiplier = default, not * 0
  delay(2000); // Give gyro some setup time

  // Set up our integrated incoders
  print("[Init] Setting up encoders\n");
  lcdSetText(uart2, 1, "Init Encs...");
  initDriveEncoders();
  // Init right encoder, not reverse
  clawR = encoderInit(DIGITAL_ENC_CLAW_R_TOP,DIGITAL_ENC_CLAW_R_BOT,false);
  // Init left encoder, not reverse
  clawL = encoderInit(DIGITAL_ENC_CLAW_L_TOP,DIGITAL_ENC_CLAW_L_BOT,false);


  // Done init
  print("[Init] Finished, starting LCD menu\n");
  lcdSetText(uart2, 1, "Init menu...");
  lcdSetCycles(false);
  lcdStartMenu();
}
