#include "main.h"
#define MIN_AUTON_MODE 1
#define MAX_AUTON_MODE 5

int autonMode = MIN_AUTON_MODE;

/*
** Returns a string representation of the current autonomous mode.
**
** RETURNS:
**   const char*: The string representation of the mode
*/
const char * getAutonName() {
  switch (autonMode) {
    case 1:
      return "Mode 1";
    case 2:
      return "Mode 2";
    case 3:
      return "Mode 3";
    case 4:
      return "Mode 4";
    case 5:
      return "Mode 5";
    default:
      return "Unknown";
  }
}

bool setAutonMode(int _autonMode) {
  if ((_autonMode < MIN_AUTON_MODE) || (_autonMode > MAX_AUTON_MODE)) { // If valid input
    autonMode = _autonMode;
  }
  return false;
}

int getAutonMode() {
	return autonMode;
}

int getAutonModeMin() {
	return MIN_AUTON_MODE;
}

int getAutonModeMax() {
	return MAX_AUTON_MODE;
}

void autonomous() {

}
