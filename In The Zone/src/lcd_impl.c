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
      autonName = "Test mode 1";
      break;
    case 2: // Mode 2 selected
      autonName = "Test mode 2";
      break;
    case 3: // Mode 3 selected
      autonName = "Test mode 3";
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
    lcdPrintTitle("Page2"); // Print auton name
  } break;
  // [Page 3] Example page -----------------------------------------------------
  case 3: {
    lcdPrintTitle("Page3"); // Print auton name
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
  }
  implUpdateLCD(true, page);
  lcdResetAutoRefresh();
}
