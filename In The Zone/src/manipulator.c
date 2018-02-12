#include "main.h"

// This file combines the functions from lift.c and intake.c to control the
// lift and intake in a much cleaner way, handling all the required movements

int cones = 0;               // How many cones we're holding inside us

// Get how many cones are on the current target goal
int getConeCount() { return cones; }

// Set how many cones are on the current target goal
void setConeCount(int coneCount) { cones = coneCount; }

// Whether or not the manipulator is still doing something
bool isManipulatorReady() {
  // TODO Populate
  return true;
}

// Wait until the intake is at it's desired target
void waitForManipulator() {
  while (!isManipulatorReady())
    delay(10);
}

// Task to handle the control of the manipulator system
void manipulatorControl(void *ignored) {
  while(true) {

    delay(20);
  }
}
