#include "main.h"
#include "math.h"

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

// TODO Configure speeds
/*
** Constants to configure resting speeds of lift
*/
#define LIFT_SPEED_HOLDING 0
#define LIFT_SPEED_IDLE 0

// TODO Configure values
#define LIFT_MIN_HEIGHT 0
#define LIFT_MAX_HEIGHT 1000

//------------------------------------------------------------------------------

/*
** Variables used to track lift height
*/
int liftStart = 0;  // The position of sensor where the lift started
int liftTarget = 0; // Delta from start for the lift to reach

//------------------------------------------------------------------------------

// =============== COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM ===============
// int getLiftHeightLeft() {
//   return 0; // TODO Use actual sensor value
// }

// =============== COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM ===============
// int getLiftHeightRight() {
//   return 0; // TODO Use actual sensor value
// }

/*
** Gets the averaged height of the two lift sensors
**
** RETURNS:
**    int: The averaged height of the two sensors w/out filter
*/
int getLiftHeight() {
  // COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM
  // return floor((((double)getLiftHeightLeft() / getLiftHeightRight()) / 2.0) +
  //              0.5);
  return encoderGet(getEncoderLift());
}

// =============== COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM ===============
// int getLiftOffset() { return getLiftHeightLeft() - getLiftHeightRight(); }

int getLiftError() { return getLiftHeight() - liftTarget; }

//------------------------------------------------------------------------------

// Used to get which part of the lift is moving faster than the other (bias)
// -1: Left is behind
// 0: No correction needed
// 1: Right is behind
// =============== COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM ===============
// char getLiftBias(bool direction) {
//   // If the lift has enough offset to need to be fixed
//   if (abs(getLiftOffset()) > LIFT_BIAS_THRESH) {
//     if (DIR_UP == direction) { // If lift is moving upward
//       if (getLiftHeightLeft() > getLiftHeightRight())
//         return 1; // Lift is going up, so right is behind (1)
//       else
//         return -1; // Lift is going up, so left  is behind (-1)
//     } else {       // If lift is moving downward
//       if (getLiftHeightLeft() > getLiftHeightRight())
//         return -1; // Lift is going down, so left  is behind (-1)
//       else
//         return 1; // Lift is going down, so right is behind (1)
//     }
//   }
//   return 0; // Lift sides don't need correction, return 0
// }

// Fixes malignant bias by slowing the biased side
// Corrects left side by slowing right and vise versa, keep in mind when reading
// =============== COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM ===============
// int corretLiftBias(bool side, int speed, bool direction) {
//   int bias = getLiftBias(direction); // Get the current bias
//   // If bias isn't 0 and the bias is the side we're checking for, return speed
//   if (bias != 0 &&
//       ((bias == -1 && side == DIR_RIGHT) || (bias == 1 && side == DIR_LEFT))) {
//     // If going up, negative speed to dampen. Vice versa for down
//     return speed +
//            (LIFT_BIAS_CORRECT_P * abs(getLiftOffset()) * (direction ? -1 : 1));
//   }
//   return speed; // No correction needed, return the speed
// }

//------------------------------------------------------------------------------

// Directly sets lift motor speeds
// Shouldn't be used unless making control loops
// COMMENTED OUT BECAUSE LIFT HAS ONE SIDE
//void setLiftSpeedRaw(int speedLeft, int speedRight) {
void setLiftSpeedRaw(int speed) {
  motorSet(MOTOR_LIFT_1, speed);
  motorSet(MOTOR_LIFT_2, speed);
  motorSet(MOTOR_LIFT_3, speed);
}

// Sets lift speed using bias correction
// Positive speed upward, negative downward
void setLiftSpeed(int speed) {
  // COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM
  // if (speed == 0)
  //   setLiftSpeedRaw(0, 0);
  // bool direction = speed > 0;
  // // TODO Check bounds
  // setLiftSpeedRaw(corretLiftBias(DIR_LEFT, speed, direction),
  //                 corretLiftBias(DIR_RIGHT, speed, direction));

  // TODO Check bounds
  setLiftSpeedRaw(speed);
}

// Sets the value for the lift to try and reach
void setLiftTarget(int target) {
  liftTarget = target;
  // TODO Add anything else needed
}

//------------------------------------------------------------------------------

// Transforms a goal constant into a height constant
int getGoalHeight(int goal) {
  switch (goal) {
  case POSITION_GOAL_STATIC:
    return HEIGHT_GOAL_STATC;
  case POSITION_GOAL_BASE_INTERNAL:
    return HEIGHT_GOAL_BASE_INTERNAL;
  case POSITION_GOAL_BASE_EXTERNAL:
    return HEIGHT_GOAL_BASE_EXTERNAL;
  case POSITION_INTAKE_GROUND:
    return HEIGHT_INTAKE_GROUND;
  case POSITION_INTAKE_LOADER:
    return HEIGHT_INTAKE_LOADER;
  case POSITION_GOAL_NONE:
  default:
    return liftStart;
  }
}

// Sets the lift target to the right height for the goal type and cone count
void setLiftTargetSmart(int goal, int cones) {
  setLiftTarget(getGoalHeight(goal) + (HEIGHT_INCREMENT_CONE * cones));
}

int getLiftTarget() {
  return liftTarget;
}

// Whether or not the lift is at it's target
bool isLiftReady() { return abs(getLiftError()) <= LIFT_TARGET_THRESH; }

// Wait until the lift is at it's desired target
void waitForLift() {
  while (!isLiftReady())
    delay(10);
}

//------------------------------------------------------------------------------

// Task to handle the control of the lift
void liftControl(void *ignored) {
  while(true) {
    // TODO Handle upper and lower bounds
    // If the error is great enough, move lift towards target
    if (!isLiftReady()) {
      // If lift is higher than target, move down, otherwise up
      bool correctionDirection =
          (getLiftHeight() - liftTarget) > 0 ? DIR_DOWN : DIR_UP;
      // Set lift speed to Kp * error * directionMultiplier
      setLiftSpeed(LIFT_TARGET_CORRECT_P * abs(getLiftError()) *
                   (correctionDirection ? -1 : 1));
    } else { // Otherwise let the lift be still
      // TODO Determine whether or not to use LIFT_SPEED_HOLDING or
      //    LIFT_SPEED_IDLE
      setLiftSpeed(LIFT_SPEED_HOLDING);
    }
    delay(10);
  }
}
