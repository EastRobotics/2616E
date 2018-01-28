#include "main.h"

// This file combines the functions from lift.c and intake.c to control the
// lift and intake in a much cleaner way, handling all the required movements

// TODO Adjust constants
#define MANIPULATOR_AVOID_THRESH 15
#define ACTION_SITTING_DOWN 0
#define ACTION_GOING_UP 1
#define ACTION_GOING_DOWN 2

int currentAction = ACTION_SITTING_DOWN;
int cones = 0;               // How many cones we're holding inside us
bool completedAction = true; // Are we done doing what we want to be doing

void setCurrentAction(int action) { currentAction = action; }

// Get how many cones are on the current target goal
int getConeCount() { return cones; }

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
  bprint(1, "Scoring\r\n");
  if ((!isManipulatorReady()) || (cones == ROBOT_CONE_LIMIT))
    return;
  completedAction = false;
  bprint(1, "Is going to move\r\n");
  setCurrentAction(ACTION_GOING_UP);
}

// Task to handle the control of the manipulator system
void manipulatorControl(void *ignored) {
  bprint(1, "TASK STARTED\r\n");
  print("Task Started\n");
  while (true) {
    bprintf(1, "currAct: %d\r\n", currentAction);
    switch (currentAction) {
    //------------------------------------------------------------------------
    case ACTION_SITTING_DOWN: {

      break;
    }
    //------------------------------------------------------------------------
    case ACTION_GOING_UP: {
      bprint(1, "Starting scoring motion");
      setClawOpen(false);                // Make sure claw knows its closed
      setLiftTargetSmart(cones);         // Lift to height of cones
      setIntakeTarget(INTAKE_POS_AVOID); // Get that 4-bar out of way
      bprint(1, "Going to wait for intake\r\n");
      waitForIntake();
      bprint(1, "Done waiting for intake\r\n");
      // while (!(abs(getLiftTarget() - getLiftPos()) < LIFT_THRESH_AVOID))
      //  delay(10);
      waitForLift();
      bprint(1, "Done waiting for lift\r\n");
      setIntakeTarget(INTAKE_POS_SCORE);
      waitForIntake();
      waitForLift();
      setCurrentAction(ACTION_GOING_DOWN);
      break;
    }
    //------------------------------------------------------------------------
    case ACTION_GOING_DOWN: {
      // if the robot is not a pass through intake then lift up a bit
      if (INTAKE_TYPE_PASSTHROUGH) {
        if (cones > 0)
          setLiftTargetSmart(cones - 1);
        setIntakeTarget(INTAKE_POS_AVOID);
        openClaw();
      } else {
        openClaw();
        delay(25);
        if (cones <= ROBOT_CONE_LIMIT)
          setLiftTargetSmart(cones + 1);
        waitForLift();
        setIntakeTarget(INTAKE_POS_AVOID);
      }
      while (!intakeIsAboveAccThresh(INTAKE_POS_AVOID))
        delay(10);
      setLiftTarget(0);
      waitForLift();
      setIntakeTarget(INTAKE_POS_SIT);
      setCurrentAction(ACTION_SITTING_DOWN);
      completedAction = true;
      cones++;
      if (cones == ROBOT_CONE_LIMIT)
        bprint(1, "ROBOT CONE LIMIT REACHED!!!");
      break;
    }
      //------------------------------------------------------------------------
      // TODO RESET TO 20
      delay(500);
    }
  }
}
