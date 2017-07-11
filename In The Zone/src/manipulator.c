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
int previousTarget = 0;             // What the last target we scored on was
bool completedAction = true; // Are we done doing what we want to be doing

int getManipulatorIntakePos() { return currentIntakePos; }

int getGoalType() { return currentGoalType; }

int getAction() { return currentAction; }

// Get how many cones are on the current target goal
int getConeCount() {
  if (currentGoalType == POSITION_GOAL_BASE_INTERNAL)
    return internalCones;
  return externalCones;
}

void setInternalConeCount(int coneCount) { internalCones = coneCount; }

void setExternalConeCount(int coneCount) { externalCones = coneCount; }

// Whether or not the manipulator is still doing something
bool isManipulatorReady() {
  return isLiftReady() && isIntakeReady() && completedAction;
}

// Wait until the intake is at it's desired target
void waitForManipulator() {
  while (!isManipulatorReady())
    delay(10);
}

void setCurrentAction(int action) {
  // If we were scoring, save last target height
  if (currentAction == ACTION_SCORING) {
    previousTarget = getLiftTarget();
  }
  currentAction = action;
}

void manipulatorScore() {
  completedAction = false;
  setCurrentAction(ACTION_SCORING);
}

void manipulatorIntake() {
  completedAction = false;
  setCurrentAction(ACTION_INTAKING);
}

void manipulatorIntakeWait() {
  completedAction = false;
  setCurrentAction(ACTION_WAITING);
}

void manipulatorOffload() {
  completedAction = false;
  setCurrentAction(ACTION_OFFLOADING);
}

void setCurrGoalType(int goalType) { currentGoalType = goalType; }

void setIntakePos(int intakePos) { currentIntakePos = intakePos; }

int getCurrGoalType() { return currentGoalType; }

// Task to handle the control of the manipulator system
void manipulatorControl(void *ignored) {
  while (true) {
    switch (currentAction) {
    // Synchronizing intake and lift to score cone
    case ACTION_SCORING: {
      setLiftTargetSmart(currentGoalType, getConeCount());
      // If we are not close enough to move
      if (abs(getLiftTarget() - getLiftHeight()) > MANIPULATOR_AVOID_THRESH) {
        setIntakeTargetSmart(POSITION_BASE_AVOID);
      } else {
        setIntakeTargetSmart(currentGoalType);
        if (isIntakeReady() && isLiftReady()) {
          setCurrentAction(ACTION_EXTAKING);
          if (currentGoalType == POSITION_GOAL_BASE_INTERNAL)
            internalCones++;
          else
            externalCones++;
        }
      }
    } break;
    case ACTION_EXTAKING: {
      // Movement only necessary if scoring on internal goal
      if (currentGoalType == POSITION_GOAL_BASE_INTERNAL) {
        // TODO handle extaking motion
        // TODO handle completedAction
      } else
        completedAction = true; // Nothing needs to be done
    } break;
    // Synchronizing intake and lift to pick up cone
    case ACTION_WAITING: {
      setIntakeTargetSmart(currentGoalType);
      // If we are not close enough to move
      if (intakeIsOutOfWay()) {
        setLiftTarget(getGoalHeight(currentIntakePos));
        if (isIntakeReady() && isLiftReady())
          completedAction = true;
      }
    } break;
    case ACTION_INTAKING: {
      // TODO Bring the lift down to the ground
    } break;
    case ACTION_OFFLOADING: {
      setLiftTargetSmart(POSITION_GOAL_BASE_INTERNAL, getConeCount() + 1);
      setIntakeTargetSmart(POSITION_BASE_AVOID);

    } break;
    default: {
      print("ERROR -- MANIPULATOR HAS NO IDEA WHAT YOU ARE TRYING TO DO \n");
    }
    }
    delay(50);
  }
}
