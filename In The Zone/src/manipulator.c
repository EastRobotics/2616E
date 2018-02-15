#include "main.h"

// This file combines the functions from lift.c and intake.c to control the
// lift and intake in a much cleaner way, handling all the required movements

int cones = 0; // How many cones we're holding inside us
int coneTarget = 0;
int scoring = false;

// AUTOSTACKING VARS
const int OVERSHOOT = 0; // How much to overshoot the stack. Pretty much however
                         // high we need to release cone
const int PARTY_HAT_THRESH = 50; // How soon before target we start party hat
const int FOURBAR_INTAKE = 3800; // Where we pick up cones
const int FOURBAR_EXTAKE = 1200; // Where we drop cones (party hat)
const int DROP_THRESH = 0; // How far from FOURBAR_EXTAKE to start dropping cone
const int LOWER_THRESH =
    0; // How far from FOURBAR_INTAKE to start lowering lift
// END AUTOSTACKING VARS

TaskHandle task;

// Get how many cones are on the current target goal
int getConeCount() { return cones; }

// Set how many cones are on the current target goal
void setConeCount(int coneCount) { cones = coneCount; }

// Whether or not the manipulator is still doing something
bool isManipulatorReady() { return !scoring; }

bool setScoring(bool isScoring) { scoring = isScoring; }

// Wait until the intake is at it's desired target
void waitForManipulator() {
  while (!isManipulatorReady())
    delay(10);
}

// NOTE: TO TUNE...
// 1 Get the y = mx+b so that the lift perfectly goes to the height to place
// cone
// 2 Then figure out how much PARTY_HAT_THRESH can be given to stack ASAP (also
// using DROP_THRESH)
// 3 Then, figure out how much overshoot needs to happen to release the cone
// 4 Then figure out how much four bar thresh is needed to avoid hitting stack
// 5 Then pretty much everything should work?

// Task to handle the control of the manipulator system
void manipulatorControl(void *ignored) {
  scoring = true; // We're scoring, let other things no
  int m = 100;
  int b = 1;
  int liftTarget = (m * coneTarget) + b + OVERSHOOT;

  // TODO: Special cases for first few

  if (coneTarget == 1) {
    setIntakeTarget(FOURBAR_EXTAKE);
    waitForIntake();
    motorSet(MOTOR_CLAW, 127);
    delay(400);
    setIntakeTarget(FOURBAR_INTAKE);
    scoring = false;
    return;
  }

  /*
  ** PARTY HAT! (Basically, go up, and swing the cone onto the top of the stack)
  */

  // Go to lift target
  setLiftTarget(liftTarget);

  // Wait until we can party hat
  while (abs(getLiftPos() - liftTarget) > (PARTY_HAT_THRESH + OVERSHOOT))
    delay(5);
  // We can party hat...
  setIntakeTarget(FOURBAR_EXTAKE);
  /* NOTE 2.2.1 start
  // Wait until we can start dropping
  while (abs(getIntakePos()-FOURBAR_EXTAKE) > DROP_THRESH)
    delay(5);
  NOTE 2.2.2 END */

  /*
  ** Cone is party hatted enough, and lift still going up. Extake that cone.
  */
  /* NOTE 2.2.2 start
  // Extake cone until we reach overshoot
  motorSet(MOTOR_CLAW, 127);
  NOTE 2.2.2 END */
  waitForLift();
  /* NOTE 4.1 start
  motorSet(MOTOR_CLAW, 0);
  // Return the four bar to intaking
  setIntakeTarget(FOURBAR_INTAKE);
  // Wait until we can start returning down
  while (abs(getIntakePos()-FOURBAR_INTAKE) > LOWER_THRESH)
    delay(5);
  NOTE 4.1 END */

  /*
  ** Okay, we have cleared the cone. Lets go back down.
  */
  // TODO Make some way to go from loader
  /* NOTE 4.2 start
  setLiftTarget(0);
  waitForLift();
  NOTE 4.2 END */

  scoring = false;
}

bool score() {
  if (!isManipulatorReady())
    return false;

  // TODO Do any checks that need to be done to score here
  coneTarget = cones + 1;
  // NOTE This might need to be after the task def? Idk
  task = taskCreate(manipulatorControl, TASK_DEFAULT_STACK_SIZE, NULL,
                    (TASK_PRIORITY_DEFAULT));
  return true;
}
