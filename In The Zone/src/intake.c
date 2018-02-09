#include "main.h"
#include "math.h"

// TODO Configure values
/*
** Constants to configure movement of intake
*/
#define INTAKE_BIAS_THRESH 5      // How far sides need to be off to correct
#define INTAKE_BIAS_CORRECT_P 1.5 // P term to use when correcting offset
#define INTAKE_TARGET_THRESH 40   // How far from target to try go to it
#define INTAKE_TARGET_CORRECT_P_UP 0.2 // P term to use when setting speed up
#define INTAKE_TARGET_CORRECT_P_DOWN                                           \
  0.22                          // P term to use when setting speed down
#define INTAKE_MINIMUM_SPEED 20 // Minimum speed for the intake to move at
#define CLAW_MOVEMENT_TIME 300  // The amount of time the claw needs to open
#define CLAW_MOVEMENT_SPEED 127 // Movement speed of the claw

// TODO Configure speeds
/*
** Constants to configure resting speeds of intake
*/
#define INTAKE_SPEED_HOLDING 15
#define INTAKE_SPEED_IDLE 0
#define CLAW_SPEED_IDLE 0

//------------------------------------------------------------------------------

/*
** Variables used to track intake position
*/
int intakeStart =
    INTAKE_POS_SCORE; // The position of sensor where the intake started
int intakeTarget = INTAKE_POS_SCORE; // Delta from start for the intake to reach

/*
** Variables used to track the claw position
*/
bool clawOpen = false;         // Whether or not the claw is opened
bool clawOpenTarget = false;   // Whether or not the claw should be opening
bool clawInMotion = false;     // Whether or not the claw is in motion
bool holdingDirection = false; // Which direction to hold intake (false = down)
bool shouldHoldIntake = true;  // Allows intake holding to be disabled in manual
int clawMovementTime = 0;      // The amount of time the claw has been in motion

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
  return analogRead(ANALOG_POT_FOUR_BAR);
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
  motorSet(MOTOR_FOUR_BAR, speed); // TODO SLew?
}

// Sets intake speed using bias correction
// Positive speed upward, negative downward
void setIntakeSpeed(int speed) {
  // COMMENTED OUT BECAUSE INTAKE HAS ONE SIDE ATM
  // if (speed == 0) {
  //   setIntakeSpeedRaw(0, 0);
  //   return;
  // }
  // bool direction = speed > 0;
  // setIntakeSpeedRaw(correctIntakeBias(DIR_LEFT, speed, direction),
  //                   correctIntakeBias(DIR_RIGHT, speed, direction));

  // TODO Check bounds
  setIntakeSpeedRaw(speed);
}

// Sets the value for the intake to try and reach
void setIntakeTarget(int target) {
  intakeTarget = target;
  holdingDirection = (target > getIntakePos()) ? true : false;
  fprintf(uart1, "\r\nIntake Targ: %d\r\n", target);
  // TODO Add anything else needed
}

//------------------------------------------------------------------------------

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

bool intakeIsAbove(int value) { return getIntakePos() >= value; }

bool intakeIsAboveAccThresh(int value) {
  return getIntakePos() + INTAKE_TARGET_THRESH >= value;
}

void setShouldHoldIntake(bool shouldHold) { shouldHoldIntake = shouldHold; }

//------------------------------------------------------------------------------

void setClawOpen(bool isOpen) { clawOpen = isOpen; }

void openClaw() {
  if (!clawOpen) {
    // Shorten the movement of the claw if it is still moving in the other
    // direction for example if the claw just started closing for 10 ms, and is
    // told to open then it should not open for the full amount of time.
    if (clawInMotion && !clawOpenTarget)
      clawMovementTime = CLAW_MOVEMENT_TIME - clawMovementTime;
    clawOpenTarget = true;
  }
}

void closeClaw() {
  if (clawOpen) {
    // Shorten the movement of the claw if it is still moving in the other
    // direction for example if the claw just started opening for 10 ms, and is
    // told to close then it should not close for the full amount of time.
    if (clawInMotion && clawOpenTarget)
      clawMovementTime = CLAW_MOVEMENT_TIME - clawMovementTime;
    clawOpenTarget = false;
  }
}

bool isClawReady() { return !clawInMotion; }

void waitForClaw() {
  while (!isClawReady())
    delay(10);
}

int pLoopDetermineIntakeSpeed() {
  // If intake is higher than target, move down, otherwise up
  // NOTE: This is opposite of what it should be
  bool correctionDirection =
      (getIntakePos() - intakeTarget) > 0 ? DIR_DOWN : DIR_UP;
  // Set intake speed to Kp * error * directionMultiplier
  int speed = (correctionDirection ? INTAKE_TARGET_CORRECT_P_DOWN
                                   : INTAKE_TARGET_CORRECT_P_UP) *
              abs(getIntakeError());
  speed =
      (abs(speed) < INTAKE_MINIMUM_SPEED) ? INTAKE_MINIMUM_SPEED : speed;
  speed *= (correctionDirection ? -1 : 1);
  return speed;
}

//------------------------------------------------------------------------------

// Task to handle the control of the intake
void intakeControl(void *ignored) {
  while (true) {
    // TODO Handle upper and lower bounds
    // If the error is great enough, move intake towards target
    if (!isIntakeReady()) {
      // bprintf(1, "error: %d\r\n", getIntakePos());
      setIntakeSpeed(pLoopDetermineIntakeSpeed());
      shouldHoldIntake = true;
    } else if (shouldHoldIntake) { // Otherwise let the intake be still
      // TODO Determine whether or not to use INTAKE_SPEED_HOLDING or
      //    INTAKE_SPEED_IDLE
      setIntakeSpeed(INTAKE_SPEED_HOLDING * (holdingDirection ? 1 : -1));
    }

    /*
    ** Claw Control
    */

    // If the claw is not at the position it is trying to reach
    // Since there is no encoder just boolean values compared so no thresholds
    if (clawOpen != clawOpenTarget) {
      // If the claw is not already in motion
      if (!clawInMotion) {
        clawMovementTime = 0;
        clawInMotion = true;
      }

      // Move at proper speed in direction based on open or close direction
      motorSet(MOTOR_CLAW, CLAW_MOVEMENT_SPEED * ((clawOpenTarget) ? 1 : -1));

      // If the claw has reached the end of its movement
      if (clawMovementTime >= CLAW_MOVEMENT_TIME) {
        clawOpen = clawOpenTarget; // set current position to the target,
                                   // because it has finished moving
        clawInMotion = false;
        clawMovementTime = 0;

        // Run the claw at a low speed to hold it in place
        /*if (clawOpen)
          motorSet(MOTOR_CLAW, CLAW_SPEED_IDLE);
        else
          motorSet(MOTOR_CLAW, CLAW_SPEED_IDLE * -1);*/
      } else {
        clawMovementTime += 10;
      }
    }
    delay(10);
  }
}
