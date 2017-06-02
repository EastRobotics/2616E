#include "main.h"

// This file combines the functions from lift.c and intake.c to control the
// lift and intake in a much cleaner way, handling all the required movements

// TODO Adjust constants
#define MANIPULATOR_AVOID_THRESH 15

int internalCones = 0; // How many cones we're holding inside us
int externalCones = 0; // How many cones are on whatever goal we're set to
int currentIntakePos = POSITION_INTAKE_GROUND;     // Where we're intaking from
int currentGoalType = POSITION_GOAL_BASE_INTERNAL; // Where we're scoring to
int currentAction = ACTION_SCORING; // What we're doing right now (start folded)
int previousTarget = 0; // What the last target we scored on was
bool completedAction = true; // Are we done doing what we want to be doing

int getManipulatorPos() { return currentIntakePos; }

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

void manipulatorSetCurrentPosition(int action) {
  // If we were scoring, save last target height
  if(currentAction == ACTION_SCORING) {
    previousTarget = getLiftTarget();
  }
  currentAction = action;
}

void manipulatorScore() {
  //setLiftTargetSmart(currentGoalType, getConeCount());
}

// Task to handle the control of the manipulator system
void manipulatorControl(void *ignored) {
  while(true) {
    switch(currentAction) {
      // Synchronizing intake and lift to score cone
      case ACTION_SCORING: {
        setLiftTargetSmart(currentGoalType, getConeCount());
        // If we are not close enough to move
        if(abs(previousTarget-getLiftTarget()) > MANIPULATOR_AVOID_THRESH) {
          // set intake to avoiding position
        } else {
          // set intake to scoring position
        }
        // TODO handle completedAction
      }
      break;
      case ACTION_EXTAKING: {
        // Movement only necessary if scoring on internal goal
        if(currentGoalType == POSITION_GOAL_BASE_INTERNAL) {
          // TODO handle extaking motion
          // TODO handle completedAction
        } else
            completedAction = true; // Nothing needs to be done
      }
      break;
      // Synchronizing intake and lift to pick up cone
      case ACTION_INTAKING: {
        setIntakeTargetSmart(currentIntakePos);
        // If we are not close enough to move
        if(intakeIsOutOfWay()) {
          setLiftTarget(getGoalHeight(POSITION_GOAL_NONE));
        }
        // TODO handle completedAction
      }
      break;
      default: {
        print("ERROR -- MANIPULATOR HAS NO IDEA WHAT YOU ARE TRYING TO DO \n");
      }
    }
  }
}
