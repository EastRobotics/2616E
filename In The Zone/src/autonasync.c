#include "main.h"

////////////////////////////////////////////////////////////////////////////////
//
//                         Auton Asynchronous Tasks
//
////////////////////////////////////////////////////////////////////////////////

// Mogo Action

int mogoSpeed = 127; // Speed to run the mogo at
int mogoTime = 500;  // Time to run the mogo for
int mogoDone = true;

void mogoAction(void *param) {
  motorSet(MOTOR_MOGO, mogoSpeed);
  delay(mogoTime);
  motorSet(MOTOR_MOGO, 0);
  mogoDone = true;
}

void runMogoAsync(int speed, int runTime) {
  mogoSpeed = speed;
  mogoTime = runTime;
  mogoDone = false;
  taskCreate(mogoAction, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void waitForMogoAsync() {
  while (!mogoDone)
    delay(10);
}

void runMogoSync(int speed, int runTime) {
  runMogoAsync(speed, runTime);
  waitForMogoAsync();
}

// Drive Straight

int drStTicks = 500;
bool drStCorrectBack = false;
bool drStCorrectDir = true;
bool drStDone = true;

void driveStraight(void *param) {
  pLoopDriveStraight(drStTicks, drStCorrectBack, drStCorrectDir);
  drStDone = true;
}

void pLoopDriveStraightAsync(int ticks, bool correctBackwards,
                             bool correctDir) {
  drStTicks = ticks;
  drStCorrectBack = correctBackwards;
  drStCorrectDir = correctDir;
  drStDone = false;
  taskCreate(driveStraight, TASK_DEFAULT_STACK_SIZE, NULL,
             TASK_PRIORITY_DEFAULT);
}

void waitForDriveStraight() {
  while (!drStDone)
    delay(10);
}

void pLoopDriveStraightSync(int ticks, bool correctBackwards, bool correctDir) {
  pLoopDriveStraight(ticks, correctBackwards, correctDir);
}

// Point Turn

int turnAngle = 90;
bool turnDone = true;

void turnPoint(void *param) {
  pLoopTurnPoint(turnAngle);
  turnDone = true;
}

void pLoopTurnPointAsync(int angle) {
  turnAngle = angle;
  turnDone = false;
  taskCreate(turnPoint, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void waitForTurnPoint() {
  while (!turnDone)
    delay(10);
}

void pLoopTurnPointSync(int angle) { pLoopTurnPoint(angle); }

// Lift Action

bool liftDone = true;
const int liftBreakout = 2000;

void liftAction(void *param) {
  int iterations = 0;
  while (!isLiftReady() && (iterations++ < (liftBreakout / 10))) {
    setLiftSpeed(pLoopDetermineLiftSpeed());
    delay(10);
  }
  liftDone = true;
}

void runLiftAsync(int target, bool shouldOvershoot) {
  setLiftTarget(target);
  setShouldOvershootLift(shouldOvershoot);
  liftDone = false;
  taskCreate(liftAction, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void waitForLiftAsync() {
  while (!isLiftReady() || !liftDone)
    delay(10);
}

void runLiftSync(int target, bool shouldOvershoot) {
  runLiftAsync(target, shouldOvershoot);
  waitForLiftAsync();
}

// Intake Action

bool intakeDone = true;
const int intakeBreakout = 1000;

void intakeAction(void *param) {
  int iterations = 0;
  while (!isIntakeReady() && (iterations++ < (intakeBreakout / 10))) {
    setIntakeSpeed(pLoopDetermineIntakeSpeed());
    delay(10);
  }
  intakeDone = true;
}

void runIntakeAsync(int target) {
  setIntakeTarget(target);
  taskCreate(intakeAction, TASK_DEFAULT_STACK_SIZE, NULL,
             TASK_PRIORITY_DEFAULT);
}

void waitForIntakeAsync() {
  while (!isIntakeReady() || !intakeDone)
    delay(10);
}

void runIntakeSync(int target) {
  runIntakeAsync(target);
  waitForIntakeAsync();
}
