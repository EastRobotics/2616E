#include "main.h"
#include "math.h"

//------------------------------------------------------------------------------
// TODO Configure heights
/*
** Constant heights of goals
*/
#define HEIGHT_GOAL_BASE_INTERNAL 15 // Height of movable goals inside  bot
#define HEIGHT_GOAL_BASE_EXTERNAL 10 // Height of movable goals outside bot
#define HEIGHT_GOAL_STATC 30         // Height of static  goals outside bot

// TODO Configure heights
/*
** Constant heights of intake zones
*/
#define HEIGHT_INTAKE_GROUND 15 // Height of intaking from the ground
#define HEIGHT_INTAKE_LOADER 30 // Height of intaking off  the loader

// TODO Configure heights
/*
** Constant height increments
*/
#define HEIGHT_INCREMENT_CONE 2 // Height to add to goal per cone

// TODO Configure values
/*
** Constants to configure movement of lift
*/
#define LIFT_BIAS_THRESH 5      // How far lift sides need to be off to correct
#define LIFT_BIAS_CORRECT_P 1.5 // P term to use when correcting offset of lift
#define LIFT_TARGET_THRESH 5    // How far lift's from target to try go to it
#define LIFT_TARGET_CORRECT_P 1.5 // P term to use when setting speed to target

//------------------------------------------------------------------------------

/*
** Variables used to track lift height
*/
int liftStart = 0;  // The position of sensor where the lift started
int liftTarget = 0; // Delta from start for the lift to reach

//------------------------------------------------------------------------------

int getLiftHeightLeft() {
  return 0; // TODO Use actual sensor value
}

int getLiftHeightRight() {
  return 0; // TODO Use actual sensor value
}

/*
** Gets the averaged height of the two lift sensors
**
** RETURNS:
**    int: The averaged height of the two sensors w/out filter
*/
int getLiftHeight() {
  return floor((((double)getLiftHeightLeft() / getLiftHeightRight()) / 2.0) +
               0.5);
}

int getLiftOffset() { return getLiftHeightLeft() - getLiftHeightRight(); }

//------------------------------------------------------------------------------

// Used to get which part of the lift is moving faster than the other (bias)
// -1: Left is behind
// 0: No correction needed
// 1: Right is behind
char getLiftBias(bool direction) {
  // If the lift has enough offset to need to be fixed
  if (abs(getLiftOffset()) > LIFT_BIAS_THRESH) {
    if (DIR_UP) { // If lift is moving upward
      if (getLiftHeightLeft() > getLiftHeightRight())
        return 1; // Lift is going up, so right is behind (1)
      else
        return -1; // Lift is going up, so left  is behind (-1)
    } else {       // If lift is moving downward
      if (getLiftHeightLeft() > getLiftHeightRight())
        return -1; // Lift is going down, so left  is behind (-1)
      else
        return 1; // Lift is going down, so right is behind (1)
    }
  }
  return 0; // Lift sides don't need correction, return 0
}

// Fixes malignant bias by slowing the biased side
// Corrects left side by slowing right and vise versa, keep in mind when reading
int correctBias(bool side, int speed, bool direction) {
  int bias = getLiftBias(direction); // Get the current bias
  // If bias isn't 0 and the bias is the side we're checking for, return speed
  if (bias != 0 &&
      ((bias == -1 && side == DIR_RIGHT) || (bias == 1 && side == DIR_LEFT))) {
    // If going up, negative speed to dampen. Vice versa for down
    return speed +
           (LIFT_BIAS_CORRECT_P * abs(getLiftOffset()) * (direction ? -1 : 1));
  }
  return speed; // No correction needed, return the speed
}

// Directly sets lift motor speeds
// Shouldn't be used unless making control loops
void setLiftSpeedRaw(int speedLeft, int speedRight) {
  // TODO Directly set lift motors
}

// Sets lift speed using bias correction
// Positive speed upward, negative downward
void setLiftSpeed(int speed) {
  if (speed == 0)
    setLiftSpeedRaw(0, 0);
  bool direction = speed > 0;
  setLiftSpeedRaw(correctBias(DIR_LEFT, speed, direction),
                  correctBias(DIR_RIGHT, speed, direction));
}
