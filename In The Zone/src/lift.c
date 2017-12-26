#include "main.h"
#include "math.h"

// TODO Configure values
/*
** Constants to configure movement of lift
*/
#define LIFT_BIAS_THRESH 5      // How far lift sides need to be off to correct
#define LIFT_BIAS_CORRECT_P 1.5 // P term to use when correcting offset of lift
#define LIFT_TARGET_THRESH 50   // How far lift's from target to try go to it
#define LIFT_TARGET_CORRECT_P_UP                                               \
  1.25 // P term to use when setting speed to target up
#define LIFT_TARGET_CORRECT_P_DOWN                                             \
  0.25 // P term to use when setting speed to target down
  #define HEIGHT_INCREMENT_CONE 225 // Height to add to goal per cone

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
// int getLiftPosLeft() {
//   return 0; // TODO Use actual sensor value
// }

// =============== COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM ===============
// int getLiftPosRight() {
//   return 0; // TODO Use actual sensor value
// }

/*
** Gets the averaged height of the two lift sensors
**
** RETURNS:
**    int: The averaged height of the two sensors w/out filter
*/
int getLiftPos() {
  // COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM
  // return floor((((double)getLiftPosLeft() / getLiftPosRight()) / 2.0) +
  //              0.5);
  return encoderGet(getEncoderLift());
}

// =============== COMMENTED OUT BECAUSE LIFT HAS ONE SIDE ATM ===============
// int getLiftOffset() { return getLiftPosLeft() - getLiftPosRight(); }

int getLiftError() { return getLiftPos() - liftTarget; }

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
//       if (getLiftPosLeft() > getLiftPosRight())
//         return 1; // Lift is going up, so right is behind (1)
//       else
//         return -1; // Lift is going up, so left  is behind (-1)
//     } else {       // If lift is moving downward
//       if (getLiftPosLeft() > getLiftPosRight())
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
//   // If bias isn't 0 and the bias is the side we're checking for, return
//   speed
//   if (bias != 0 &&
//       ((bias == -1 && side == DIR_RIGHT) || (bias == 1 && side == DIR_LEFT)))
//       {
//     // If going up, negative speed to dampen. Vice versa for down
//     return speed +
//            (LIFT_BIAS_CORRECT_P * abs(getLiftOffset()) * (direction ? -1 :
//            1));
//   }
//   return speed; // No correction needed, return the speed
// }

//------------------------------------------------------------------------------

// Directly sets lift motor speeds
// Shouldn't be used unless making control loops
// COMMENTED OUT BECAUSE LIFT HAS ONE SIDE
// void setLiftSpeedRaw(int speedLeft, int speedRight) {
void setLiftSpeedRaw(int speed) {
  motorSet(MOTOR_LIFT_1, speed);
  motorSet(MOTOR_LIFT_2, -speed);
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

// Sets the lift target to the right height for the goal type and cone count
void setLiftTargetSmart(int cones) {
  setLiftTarget(0 + (HEIGHT_INCREMENT_CONE * cones));
}

int getLiftTarget() { return liftTarget; }

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
  while (true) {
    // TODO Handle upper and lower bounds
    // If the error is great enough, move lift towards target
    if (!isLiftReady()) {
      // If lift is higher than target, move down, otherwise up
      bool correctionDirection =
          (getLiftPos() - liftTarget) > 0 ? DIR_UP : DIR_DOWN;
      int speed = (correctionDirection ? LIFT_TARGET_CORRECT_P_DOWN
                                       : LIFT_TARGET_CORRECT_P_UP) *
                  abs(getLiftError());
      speed *= (correctionDirection ? -1 : 1);
      // Set lift speed to Kp * error * directionMultiplier
      setLiftSpeed(speed);
    } else { // Otherwise let the lift be still
      // TODO Determine whether or not to use LIFT_SPEED_HOLDING or
      //    LIFT_SPEED_IDLE
      setLiftSpeed(LIFT_SPEED_HOLDING);
    }
    delay(10);
  }
}
