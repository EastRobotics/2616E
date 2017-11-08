#include "main.h"
#include "math.h"

// TODO Create method to shove intake down

// TODO Configure pos
/*
** Constant pos of intake zones
*/
#define POS_INTAKE_GROUND 15 // Pos of intaking from the ground
#define POS_INTAKE_LOADER 30 // Pos of intaking off  the loader

// TODO Configure pos
/*
** Constant pos of release zones
*/
#define POS_EXTAKE_EXTERNAL 15 // Pos of offloading outside bot
#define POS_EXTAKE_INTERNAL 30 // Pos of offloading inside bot

// TODO Configure pos
/*
** Constant specials pos
*/
#define POS_POSITION_AVOID 15 // Pos for waiting for intake to be high enough

// TODO Configure values
/*
** Constants to configure movement of intake
*/
#define INTAKE_BIAS_THRESH 5        // How far sides need to be off to correct
#define INTAKE_BIAS_CORRECT_P 1.5   // P term to use when correcting offset
#define INTAKE_TARGET_THRESH 5      // How far from target to try go to it
#define INTAKE_TARGET_CORRECT_P 1.5 // P term to use when setting speed
#define CLAW_MOVEMENT_TIME 200      // The amount of time the claw needs to open
#define CLAW_MOVEMENT_SPEED 127     // Movement speed of the claw

// TODO Configure speeds
/*
** Constants to configure resting speeds of intake
*/
#define INTAKE_SPEED_HOLDING 0
#define INTAKE_SPEED_IDLE 0
#define CLAW_SPEED_IDLE 10

//------------------------------------------------------------------------------

/*
** Variables used to track intake position
*/
int intakeStart = 0;  // The position of sensor where the intake started
int intakeTarget = 0; // Delta from start for the intake to reach

/*
** Variables used to track the claw position
*/
bool clawOpen = false;       // Whether or not the claw is opened
bool clawOpenTarget = false; // Whether or not the claw should be opening
bool clawInMotion = false;   // Whether or not the claw is in motion
int clawMovementTime = 0;    // The amount of time the claw has been in motion

//------------------------------------------------------------------------------

// =============== COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM ===============
// int getIntakePosLeft() {
//   return 0; // TODO Use actual sensor value
// }

// =============== COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM ===============
// int getIntakePosRight() {
//   return 0; // TODO Use actual sensor value
// }

/*
** Gets the averaged pos of the two intake sensors
**
** RETURNS:
**    int: The averaged pos of the two sensors w/out filter
*/
int getIntakePos() {
  // COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM
  // return floor((((double)getIntakePosLeft() / getIntakePosRight()) / 2.0) +
  //              0.5);
  return encoderGet(getEncoderChain());
}

// =============== COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM ===============
// int getIntakeOffset() { return getIntakePosLeft() - getIntakePosRight(); }

int getIntakeError() { return getIntakePos() - intakeTarget; }

//------------------------------------------------------------------------------

// Used to get which part of the intake is moving faster than the other (bias)
// -1: Left is behind
// 0: No correction needed
// 1: Right is behind
// =============== COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM ===============
// char getIntakeBias(bool direction) {
//   // If the intake has enough offset to need to be fixed
//   if (abs(getIntakeOffset()) > INTAKE_BIAS_THRESH) {
//     if (DIR_FORWARD == direction) { // If intake is moving forward
//       if (getIntakePosLeft() > getIntakePosRight())
//         return 1; // Intake is going forward, so right is behind (1)
//       else
//         return -1; // Intake is going forward, so left  is behind (-1)
//     } else {       // If intake is moving downward
//       if (getIntakePosLeft() > getIntakePosRight())
//         return -1; // Intake is going backward, so left  is behind (-1)
//       else
//         return 1; // Intake is going backward, so right is behind (1)
//     }
//   }
//   return 0; // Intake sides don't need correction, return 0
// }

// Fixes malignant bias by slowing the biased side
// Corrects left side by slowing right and vise versa, keep in mind when reading
// =============== COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM ===============
// int correctIntakeBias(bool side, int speed, bool direction) {
//   int bias = getIntakeBias(direction); // Get the current bias
//   // If bias isn't 0 and the bias is the side we're checking for, return
//   speed
//   if (bias != 0 &&
//       ((bias == -1 && side == DIR_RIGHT) || (bias == 1 && side == DIR_LEFT)))
//       {
//     // If going up, negative speed to dampen. Vice versa for down
//     return speed + (INTAKE_BIAS_CORRECT_P * abs(getIntakeOffset()) *
//                     (direction ? -1 : 1));
//   }
//   return speed; // No correction needed, return the speed
// }

//------------------------------------------------------------------------------

// Directly sets intake motor speeds
// Shouldn't be used unless making control loops
// COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM
// void setIntakeSpeedRaw(int speedLeft, int speedRight) {
void setIntakeSpeedRaw(int speed) {
  motorSet(MOTOR_CHAIN_BAR, speed); // TODO SLew?
}

// Sets intake speed using bias correction
// Positive speed upward, negative downward
void setIntakeSpeed(int speed) {
  // COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM
  // if (speed == 0)
  //   setIntakeSpeedRaw(0, 0);
  // bool direction = speed > 0;
  // setIntakeSpeedRaw(correctIntakeBias(DIR_LEFT, speed, direction),
  //                   correctIntakeBias(DIR_RIGHT, speed, direction));

  // TODO Check bounds
  setIntakeSpeedRaw(speed);
}

// Sets the value for the intake to try and reach
void setIntakeTarget(int target) {
  intakeTarget = target;
  // TODO Add anything else needed
}

//------------------------------------------------------------------------------

// Transforms a goal constant into a pos constant
int getGoalPos(int goal) {
  switch (goal) {
  case POSITION_GOAL_STATIC:
  case POSITION_GOAL_BASE_EXTERNAL:
    return POS_EXTAKE_EXTERNAL;
  case POSITION_GOAL_BASE_INTERNAL:
    return POS_EXTAKE_INTERNAL;
  case POSITION_BASE_AVOID:
    return POS_POSITION_AVOID;
  case POSITION_GOAL_NONE:
  default:
    return getIntakePos();
  }
}

// Sets the intake target to the right pos for the goal type
void setIntakeTargetSmart(int goal) { setIntakeTarget(getGoalPos(goal)); }

int getIntakeTarget() { return intakeTarget; }

// Whether or not the intake is at it's target
bool isIntakeReady() { return abs(getIntakeError()) <= INTAKE_TARGET_THRESH; }

// Wait until the intake is at it's desired target
void waitForIntake() {
  while (!isIntakeReady())
    delay(10);
}

bool intakeIsOutOfWay() { return getIntakePos() <= POS_POSITION_AVOID; }

void openClaw() {
  if (!clawOpen) {
    // Shorten the movement of the claw if it is still moving in the other
    // direction for example if the claw just started closing for 10 ms, and is
    // told to open then it should not open for the full amount of time.
    if (clawInMotion)
      clawMovementTime = CLAW_MOVEMENT_TIME - clawMovementTime;
    clawOpenTarget = true;
  }
}

void closeClaw() {
  if (clawOpen) {
    // Shorten the movement of the claw if it is still moving in the other
    // direction for example if the claw just started opening for 10 ms, and is
    // told to close then it should not close for the full amount of time.
    if (clawInMotion)
      clawMovementTime = CLAW_MOVEMENT_TIME - clawMovementTime;
    clawOpenTarget = false;
  }
}

//------------------------------------------------------------------------------

// Task to handle the control of the intake
void intakeControl(void *ignored) {
  while (true) {
    // TODO Handle upper and lower bounds
    // If the error is great enough, move intake towards target
    if (!isIntakeReady()) {
      // If intake is higher than target, move down, otherwise up
      bool correctionDirection =
          (getIntakePos() - intakeTarget) > 0 ? DIR_DOWN : DIR_UP;
      // Set intake speed to Kp * error * directionMultiplier
      setIntakeSpeed(INTAKE_TARGET_CORRECT_P * abs(getIntakeError()) *
                     (correctionDirection ? -1 : 1));
    } else { // Otherwise let the intake be still
      // TODO Determine whether or not to use INTAKE_SPEED_HOLDING or
      //    INTAKE_SPEED_IDLE
      setIntakeSpeed(INTAKE_SPEED_HOLDING);
    }

    /*
    ** Claw Control
    */

    // If the claw is not at the position it is trying to reach
    if (clawOpen != clawOpenTarget) {
      // If the claw is not already in motion
      if (!clawInMotion) {
        clawMovementTime = 0;
        clawInMotion = true;
      }

      // If the claw should open
      if (clawOpenTarget)
        motorSet(MOTOR_CLAW, CLAW_MOVEMENT_SPEED);
      else
        motorSet(MOTOR_CLAW, CLAW_MOVEMENT_SPEED * -1);

      // If the claw has reached the end of its movement
      if (clawMovementTime >= CLAW_MOVEMENT_TIME) {
        clawOpen = clawOpenTarget;
        clawInMotion = false;
        clawMovementTime = 0;
        if (clawOpen)
          motorSet(MOTOR_CLAW, CLAW_SPEED_IDLE);
        else
          motorSet(MOTOR_CLAW, CLAW_SPEED_IDLE * -1);
      } else {
        clawMovementTime += 10;
      }
    }
    delay(10);
  }
}
