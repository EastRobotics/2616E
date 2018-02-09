#include "main.h"

////////////////////////////////////////////////////////////////////////////////
//
//                         Auton Asynchronous Tasks
//
////////////////////////////////////////////////////////////////////////////////

// Mogo Action

int mogoSpeed = 127; // Speed to run the mogo at
int mogoTime = 500; // Time to run the mogo for
int mogoDone = true;

void mogoAction(void* param) {
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
  while(!mogoDone)
    delay(10);
}

// Drive Straight

int drStTicks = 500;
bool drStCorrectBack = false;
bool drStCorrectDir = true;
bool drStDone = true;

void driveStraight(void* param) {
  pLoopDriveStraight(drStTicks, drStCorrectBack, drStCorrectDir);
  drStDone = true;
}

void pLoopDriveStraightAsync(int ticks, bool correctBackwards, bool correctDir) {
  drStTicks = ticks;
  drStCorrectBack = correctBackwards;
  drStCorrectDir = correctDir;
  drStDone = false;
  taskCreate(driveStraight, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void waitForDriveStraight() {
  while(!drStDone)
    delay(10);
}

// Point Turn

int turnAngle = 90;
bool turnDone = true;

void turnPoint(void* param) {
  pLoopTurnPoint(turnAngle);
  turnDone = true;
}

void pLoopTurnPointAsync(int angle) {
  turnAngle = angle;
  turnDone = false;
  taskCreate(turnPoint, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void waitForTurnPoint() {
  while(!turnDone)
    delay(10);
}

// Lift Action

bool liftDone = true;

void liftAction(void* param) {
  while(!isLiftReady()) {
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
  while(!isLiftReady() || !liftDone)
    delay(10);
}

// Intake Action

bool intakeDone = true;

void intakeAction(void* param) {
  while(!isIntakeReady()) {
    setIntakeSpeed(pLoopDetermineIntakeSpeed());
    delay(10);
  }
  intakeDone = true;
}

void runIntakeAsync(int target) {
  setIntakeTarget(target);
  taskCreate(intakeAction, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void waitForIntakeAsync() {
  while(!isIntakeReady() || !intakeDone)
    delay(10);
}
