#include "main.h"
#include "math.h"

#define LIFT_ACC_MOV_RANGE 30  // amount lift will move before it is held in place
#define LIFT_MIN_SPEED 30      // lowest speed that will turn on the lift
#define LIFT_MIN_HEIGHT 100    // lowest potentiometer value for the lift
#define LIFT_MAX_HEIGHT 2000   // highest potentiometer value for the lift
#define LIFT_MAX_DIFF 2000     // highest difference in angle for the lift
#define LIFT_SLOW_RANGE 100    // how close to the bounds the lift should slow down
#define LIFT_SLOW_MOD 0.8      // how much (0-1] that the lift should be slowed down at bounds

int startingAngle = 0;   // desired height for the hold task to hold at

int getLiftStartAngle() {
    return startingAngle;
}

// Sets all of the lift motors to the desired speed
// (Note: positive speed is intended to move the lift up)
// PARAMS:
//  int: the speed of the motors
void setLiftMotors(int speed){
  motorSet(MOTOR_LIFT_R1, speed);
  motorSet(MOTOR_Y_LIFT_R23, speed);
  motorSet(MOTOR_LIFT_L1, speed*-1);
  motorSet(MOTOR_Y_LIFT_L23, speed*-1);
}

// Sets the lift motors with a range of speed, and can auto override the hold
// (WARNING: If you wish to reinstate a hold afterwards you must call lockLift().
// This function will not automatically reset the hold when you are done, if you
// wish to do so, it only will automatically disable the hold)
// PARAMS:
//  int: the speed to set to the motors
//  bool: whether or not to turn off the hold if it is active
//  bool: whether or not to slow down the lift at it's bounds
void moveLiftWithLogic(int speed, bool dampenSpeed){
  // if the speed is too low, set it to zero
  if(abs(speed) < LIFT_MIN_SPEED){
    speed = 0;
  }

  int currHeight = analogRead(ANALOG_POT_LIFT);
  // if moving up
  if(speed>0) {
    // if within the slow zone, dampen the speed (if feature on)
    if(dampenSpeed && (abs(LIFT_MAX_HEIGHT-currHeight) < LIFT_SLOW_RANGE)){
      speed *= LIFT_SLOW_MOD;
    }
    // if greater than max and trying to go up, shut off
    if(currHeight > LIFT_MAX_HEIGHT){
      speed = 0;
    }
  } else { //if moving down
    // if within the slow zone, dampen the speed
    if(dampenSpeed && (abs(LIFT_MIN_HEIGHT-currHeight) < LIFT_SLOW_RANGE)){
      speed *= LIFT_SLOW_MOD;
    }
    // if less than min and trying to go down, shut off
    if(currHeight < LIFT_MIN_HEIGHT){
      speed = 0;
    }
  }

  // set the lift motors to the desired speed
  setLiftMotors(speed);
}

// Intended for autonomous, waits until the lift has fully raised or lowered.
// Which one it waits for depends on the direction of the motion.
// PARAMS:
//  int: the speed to set the motors (also +/- indicates direction (see above))
void waitForLift(int speed){
  // set the lift to the speed, and turn on inertial dampening, but only
  // if this will NOT slow it too much to even move
  moveLiftWithLogic(speed,(speed>=ceil(LIFT_MIN_SPEED/LIFT_SLOW_MOD)));
  // if the lift is being raised
  if(speed>=0){
    // wait until the lift is at it's max value
    while(analogRead(ANALOG_POT_LIFT) < LIFT_MAX_HEIGHT){
      moveLiftWithLogic(speed,(speed>=ceil(LIFT_MIN_SPEED/LIFT_SLOW_MOD))); // the speed must be reset for the auto slow down to work
      delay(25);
    }
  } else { // if the lift is being lowered
    // wait until the lift is at it's min value
    while(analogRead(ANALOG_POT_LIFT) > LIFT_MIN_HEIGHT){
      moveLiftWithLogic(speed,(speed>=ceil(LIFT_MIN_SPEED/LIFT_SLOW_MOD))); // the speed must be reset for the auto slow down to work
      delay(25);
    }
  }

  setLiftMotors(0);

}

// Setup the lift to hold the position of it's current potentiometer value
void lockLift(){
  moveLiftWithLogic(30,true);
}

// Sets up all necessary tasks and instance data for the lift
void initLift(){
  startingAngle = analogRead(ANALOG_POT_LIFT);
}
