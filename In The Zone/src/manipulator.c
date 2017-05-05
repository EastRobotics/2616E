#include "main.h"

// This file combines the functions from lift.c and intake.c to control the
// lift and intake in a much cleaner way, handling all the required movements

int internalCones = 0; // How many cones we're holding inside us
int externalCones = 0; // How many cones are on whatever goal we're set to
int currentIntakePos = POSITION_INTAKE_GROUND;     // Where we're intaking from
int currentGoalType = POSITION_GOAL_BASE_INTERNAL; // Where we're scoring to
int currentAction = ACTION_SCORING; // What we're doing right now (start folded)

int getIntakePos() { return currentIntakePos; }

int getGoalType() { return currentGoalType; }

int getAction() { return currentAction; }

// Get how many cones are on the current target goal
int getConeCount() {
  if (currentGoalType == POSITION_GOAL_BASE_INTERNAL)
    return internalCones;
  return externalCones;
}

// Whether or not the manipulator is still doing something
bool isManipulatorReady() { return isLiftReady() && isIntakeReady(); }

// Wait until the intake is at it's desired target
void waitForManipulator() {
  while (!isManipulatorReady())
    delay(10);
}
