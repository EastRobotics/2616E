#include "main.h"

void implUpdateLCD(bool userCaused, int page) {
  lcdClear(uart2);
  switch (page) {
  // [Page 1] Auton mode selector --------------------------------------------
  case 1: {
    lcdPrintTitle("AutonMode");
    const char *autonName;
    switch (getAutonMode()) {
    case 1:
      autonName = "Do Nothing";
      break;
    case 2:
      autonName = "PID Test";
      break;
    case 3:
      autonName = "Claw Test";
      break;
    case 4:
      autonName = "Line up Test";
      break;
    case 5:
      autonName = "TimeDrive Test";
      break;
    case 6:
      autonName = "Time Cube";
      break;
    case 7:
      autonName = "Corner Star";
      break;
    default: { // Unknown mode
      char *temp = NULL;
      snprintf(temp, 17, "Unknown %i", page);
      autonName = temp;
    }
    }
    static char temp[16];
    sprintf(temp, "%c %s %c", 0xBC, autonName, 0xBB);
    // lcdPrint(uart2, 2, "%c %s %c",0xBC, autonName, 0xBB);
    lcdPrintCentered(temp, 2);
  } break;

  // [Page 2] Auton position selector --------------------------------------
  case 2: {
    lcdPrintTitle("AutonPos");
    char temp[16];
    sprintf(temp, "%c %s %c", 0xBC, getAutonPosition() ? "Right" : "Left",
            0xBB);
    lcdPrintCentered(temp, 2);
  } break;

  // [Page 3] Auton color selector -----------------------------------------
  case 3: {
    lcdPrintTitle("AutonColor");
    char temp[16];
    sprintf(temp, "%c %s %c", 0xBC, getAutonColor() ? "Red" : "Blue", 0xBB);
    lcdPrintCentered(temp, 2);
  } break;

  // [Page 4] Home page ----------------------------------------------------
  case 4: {
    lcdPrint(uart2, 1, "%cEssential Evil%c", 0xCD, 0xCD);
    char batWarning = ' ';
    // If really bad
    if (batteryStatus(voltLevelExpander(ANALOG_POW_EXPAND, true)) >= 2 ||
        batteryStatus(voltLevelMain()) >= 2) {
      batWarning = 0xF5; // Noticeable down arrow thing
      // If pretty bad
    } else if (batteryStatus(voltLevelExpander(ANALOG_POW_EXPAND, true)) >= 1 ||
               batteryStatus(voltLevelMain()) >= 1) {
      batWarning = 0x21;
    }
    lcdPrint(uart2, 2, "%cbat | aut%d", batWarning, getAutonMode());
  } break;

  // [Page 5] Battery Stat -------------------------------------------------
  case 5: {
    lcdPrint(uart2, 1, "%s%c B:%.2fv", "BatStat", 0xF6, voltLevelBackup());
    lcdPrint(uart2, 2, " M:%.2fv E:%.2fv", voltLevelMain(),
             voltLevelExpander(ANALOG_POW_EXPAND, true));
  } break;
  // [Page 6] Claw pots ----------------------------------------------------
  case 6: {
    lcdPrintTitle("ClawPots");
    int clawPosLeft = encoderGet(getEncoderClawL());
    int clawPosRight = encoderGet(getEncoderClawR());
    lcdPrint(uart2, 2, "L:%.4d R:%.4d", clawPosLeft, clawPosRight);
  } break;
  // [Page 7] Line sensors -------------------------------------------------
  case 7: {
    lcdPrintTitle("LineSensors");
    lcdPrint(uart2, 2, "L: %.4d R: %.4d", analogRead(ANALOG_LINE_LEFT),
             analogRead(ANALOG_LINE_RIGHT));
  } break;
  // [Page 8] Arm pot ------------------------------------------------------
  case 8: {
    lcdPrintTitle("ArmPot");
    lcdPrint(uart2, 2, "Arm: %.4d", analogRead(ANALOG_POT_LIFT));
  } break;
  // [Page 9] Drive Encoders ----------------------------------------------------
  case 9: {
    lcdPrintTitle("DriveEnc");
    int backLeft = encoderGet(getEncoderBL());
    int backRight = encoderGet(getEncoderBR());
    lcdPrint(uart2, 2, "L:%.4d R:%.4d", backLeft, backRight);
  } break;
  // [Page ?] Unknown page ---------------------------------------------------
  default:
    lcdPrint(uart2, 1, "Page? %d", page);
    break;
    // -------------------------------------------------------------------------
  }
}

void implMenuNext(int page) {
  switch (page) {
  // [Page 1] Auton mode selector --------------------------------------------
  case 1: {
    int nextMode = getAutonMode() + 1;
    if (nextMode > getAutonModeMax())
      setAutonMode(getAutonModeMin());
    else
      setAutonMode(nextMode);
  } break;
  // [Page 2] Auton position selector --------------------------------------
  case 2: {
    setAutonPosition(!getAutonPosition());
  } break;
  // [Page 3] Auton color selector -------------------------------------------
  case 3: {
    setAutonColor(!getAutonColor());
  } break;
    // -------------------------------------------------------------------------
  }
  implUpdateLCD(true, page);
  lcdResetAutoRefresh();
}

void implMenuBack(int page) {
  switch (page) {
  // [Page 1] Auton mode selector --------------------------------------------
  case 1: {
    int nextMode = getAutonMode() - 1;
    if (nextMode < getAutonModeMin())
      setAutonMode(getAutonModeMax());
    else
      setAutonMode(nextMode);
  } break;
  // [Page 2] Auton position selector --------------------------------------
  case 2: {
    setAutonPosition(!getAutonPosition());
  } break;
  // [Page 3] Auton color selector -------------------------------------------
  case 3: {
    setAutonColor(!getAutonColor());
  } break;
    // -------------------------------------------------------------------------
  }
  implUpdateLCD(true, page);
  lcdResetAutoRefresh();
}
