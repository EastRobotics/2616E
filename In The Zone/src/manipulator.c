#include "main.h"

// This file combines the functions from lift.c and intake.c to control the
// lift and intake in a much cleaner way, handling all the required movements

// TODO Adjust constants
#define MANIPULATOR_AVOID_THRESH 15
#define ACTION_SITTING_DOWN 0
#define ACTION_GOING_UP 1
#define ACTION_GOING_DOWN 2

int currentAction = ACTION_SITTING_DOWN;
int cones = 0; // How many cones we're holding inside us
bool completedAction = true; // Are we done doing what we want to be doing

void setCurrentAction(int action) {
  currentAction = action;
}

// Get how many cones are on the current target goal
int getConeCount() {
  return cones;
}

// Set how many cones are on the current target goal
void setConeCount(int coneCount) { cones = coneCount; }

// Whether or not the manipulator is still doing something
bool isManipulatorReady() {
  // NOTE isLiftReady() && isIntakeReady() can be removed
  // NOTE (and may actually cause problems)
  return isLiftReady() && isIntakeReady() && completedAction;
}

// Wait until the intake is at it's desired target
void waitForManipulator() {
  while (!isManipulatorReady())
    delay(10);
}

void manipulatorScore() {
  // TODO Populate
}

void manipulatorIntake() {
  // TODO Populate
}

void score() {
  if (!isManipulatorReady())
    return;
  completedAction = false;
  setCurrentAction(ACTION_GOING_UP);
}

// Task to handle the control of the manipulator system
void manipulatorControl(void *ignored) {
  while (true) {
    switch (currentAction) {
      //------------------------------------------------------------------------
      case ACTION_SITTING_DOWN: {

        break;
      }
      //------------------------------------------------------------------------
      case ACTION_GOING_UP: {
        // TODO Set lift target to actual end goal
        // TODO Set 4bar to avoid
        // TODO Once we can, move 4bar to score
        // TODO Once 4 bar is score, move on
        // TODO Set current action to going down
        break;
      }
      //------------------------------------------------------------------------
      case ACTION_GOING_DOWN: {
        // TODO Go down enough to place cone
        // TODO Move 4bar down to release cone
        // TODO Open claw (should be async, doesn't need to finish)
        // TODO Make sure 4bar is just somewhere between intaking to avoid
        // TODO Lower lift to 0
        // TODO Return 4bar to intaking pos
        // TODO Set current action to sitting down
        // TODO Set completed to true
        break;
      }
      //------------------------------------------------------------------------
    }
  }
}
