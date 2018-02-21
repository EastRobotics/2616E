#include "main.h"

void implUpdateLCD(bool userCaused, int page) {
  lcdClear(uart2);
  switch (page) {
  // [Page 1] Auton mode selector ----------------------------------------------
  case 1: {
    lcdPrintTitle("AutonMode");
    const char *autonName;
    switch (getAutonMode()) { // Set name based on auton mode
    case 1:                   // Mode 1 selected
      autonName = "Do Nothing";
      break;
    case 2:                // Mode 1 selected
      autonName = "Light"; // AKA Ra,
      break;
    case 3: // Mode 2 selected
      autonName = "20ptST";
      break;
    case 4: // Mode 3 selected
      autonName = "20pt";
      break;
    case 5: // Mode 3 selected
      autonName = "10ptST";
      break;
    case 6: // Mode 3 selected
      autonName = "10pt";
      break;
    case 7: // Mode 3 selected
      autonName = "Skills";
      break;
    case 8:
      autonName = "Custom";
      break;
    default: { // Unknown mode
      char temp[17];
      snprintf(temp, 17, "Unknown %d", page);
      autonName = temp;
    }
    }
    static char temp[16]; // Create buffer for following line
    sprintf(temp, "%c %s %c", 0xBC, autonName, 0xBB); // Set up auton name
    lcdPrintCentered(temp, 2);                        // Print auton name
  } break;
  // [Page 2] Example page -----------------------------------------------------
  case 2: {
    lcdPrint(uart2, 0, "L: %d", encoderGet(getEncoderBL()));
    lcdPrint(uart2, 1, "R: %d", encoderGet(getEncoderBR()));
  } break;
  // [Page 3] Example page -----------------------------------------------------
  case 3: {
    lcdPrintError("Dist", encoderGet(getEncoderBL()), 1000);
  } break;
  case 4: {
    lcdPrintTitle("Auton Pos");
    const char *autonSide;
    if (getAutonPosition()) {
      autonSide = "Right";
    } else {
      autonSide = "Left";
    }
    static char temp[16]; // Create buffer for following line
    sprintf(temp, "%c %s %c", 0xBC, autonSide, 0xBB); // Set up auton side
    lcdPrintCentered(temp, 2);                        // Print auton side
  } break;
  case 5: {
    lcdPrintTitle("Main Bat");
    static char temp[16]; // Create buffer for following line
    sprintf(temp, "%fV", voltLevelMain());
    lcdPrintCentered(temp, 2); // Print voltage level
  } break;
  case 6: {
    lcdPrintTitle("Cone Count");
    static char temp[16]; // Create buffer for following line
    sprintf(temp, "%d Cones", getConeCount());
    lcdPrintCentered(temp, 2); // Print cone count
  } break;
  case 7: {
    lcdPrintTitle("m");
    static char temp[16]; // Create buffer for following line
    sprintf(temp, "%d", getManM());
    lcdPrintCentered(temp, 2); // Print m
  } break;
  case 8: {
    lcdPrintTitle("b");
    static char temp[16]; // Create buffer for following line
    sprintf(temp, "%d", getManB());
    lcdPrintCentered(temp, 2); // Print b
  } break;
  case 9: {
    lcdPrintTitle("Party");
    static char temp[16]; // Create buffer for following line
    sprintf(temp, "%d", getManParty());
    lcdPrintCentered(temp, 2); // Print party
  } break;
  // [Page ?] Unknown page -----------------------------------------------------
  default:
    lcdPrint(uart2, 1, "Page? %d", page);
    break; // ------------------------------------------------------------------
  }
}

// Note, only pages that need in-menu actions should use this
void implMenuNext(int page) {
  switch (page) {
  // [Page 1] Auton mode selector ----------------------------------------------
  case 1: {
    int nextMode = getAutonMode() + 1;
    if (nextMode > getAutonModeMax())
      setAutonMode(getAutonModeMin());
    else
      setAutonMode(nextMode);
  } break; // ------------------------------------------------------------------
  case 4: {
    setAutonPosition(!getAutonPosition());
  } break; // ------------------------------------------------------------------
  case 7: {
    setLiftVars(getManM()+1, getManB(), getManParty());
  } break; // ------------------------------------------------------------------
  case 8: {
    setLiftVars(getManM(), getManB()+1, getManParty());
  } break; // ------------------------------------------------------------------
  case 9: {
    setLiftVars(getManM(), getManB(), getManParty()+1);
  } break; // ------------------------------------------------------------------
  }
  implUpdateLCD(true, page);
  lcdResetAutoRefresh();
}

// Note, only pages that need in-menu actions should use this
void implMenuBack(int page) {
  switch (page) {
  // [Page 1] Auton mode selector ----------------------------------------------
  case 1: {
    int nextMode = getAutonMode() - 1;
    if (nextMode < getAutonModeMin())
      setAutonMode(getAutonModeMax());
    else
      setAutonMode(nextMode);
  } break; // ------------------------------------------------------------------
  case 4: {
    setAutonPosition(!getAutonPosition());
  } break; // ------------------------------------------------------------------
  case 7: {
    setLiftVars(getManM()-1, getManB(), getManParty());
  } break; // ------------------------------------------------------------------
  case 8: {
    setLiftVars(getManM(), getManB()-1, getManParty());
  } break; // ------------------------------------------------------------------
  case 9: {
    setLiftVars(getManM(), getManB(), getManParty()-1);
  } break; // ------------------------------------------------------------------
  }
  implUpdateLCD(true, page);
  lcdResetAutoRefresh();
}
