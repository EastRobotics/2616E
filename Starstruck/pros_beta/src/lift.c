#include "main.h"
#include "math.h"

#define LIFT_ACC_MOV_RANGE 30
#define LIFT_MIN_SPEED 30

TaskHandle holdLiftTask; // task handler for the task that holds the lift in place
bool holdActive = false; // is the lift currently attempting to hold in place
int desiredHeight = 0;   // desired height for the hold task to hold at

// Sets all of the lift motors to the desired speed
// (Note: positive speed is intended to move the lift up)
// PARAMS:
//  int: the speed of the motors
void setLiftMotors(int speed){
  //TODO: put in lift motors set to the inputted speed
}

//Task that holds the lift in it's current position
void holdLift(void * ignored){
  int currHeight;
  while(true){
    // get current potentiometer value
    currHeight = analogRead(ANALOG_POT_LIFT);
    //if the lift has moved too far from the desired position
    if (abs(currHeight-desiredHeight) > LIFT_ACC_MOV_RANGE) {
      // set the motors to move up if below target, or down if above target
      setLiftMotors(currHeight<desiredHeight ? 50 : -50);
    } else {
      // hold the lift in it's current position
      setLiftMotors(0);
    }
  delay(25);
  }
}

// Setup the lift to hold the position of it's current potentiometer value
void lockLift(){
  if(!holdActive){
    holdActive = true;
    desiredHeight = analogRead(ANALOG_POT_LIFT);
    taskResume(holdLiftTask);
  }
}

// Stop the lift from holding the position of it's current potentiometer value
void unlockLift(){
  if(holdActive){
    taskSuspend(holdLiftTask);
    holdActive = false;
  }
}

// Sets the lift motors with a range of speed, and can auto override the hold
// (WARNING: If you wish to reinstate a hold afterwards you must call lockLift().
// This function will not automatically reset the hold when you are done, if you
// wish to do so, it only will automatically disable the hold)
// PARAMS:
//  int: the speed to set to the motors
//  bool: whether or not to turn off the hold if it is active
void moveLiftWithLogic(int speed, bool overrideHold){
  // if the speed is too low, do nothing
  if(abs(speed) < LIFT_MIN_SPEED){
    return;
  }
  //unlock lift if it was locked
  if(holdActive && overrideHold){
    unlockLift();
  } else if(holdActive) { // if the hold is active, but can't override, do nothing
    return;
  }

  // set the lift motors to the desired speed
  setLiftMotors(speed);
}

// Sets up all necessary tasks and instance data for the lift
void initLift(){
  holdLiftTask = taskCreate(holdLift, TASK_DEFAULT_STACK_SIZE,
    NULL, TASK_PRIORITY_DEFAULT);
  unlockLift();
  holdActive = false;
  desiredHeight = analogRead(ANALOG_POT_LIFT);
}
