#include "main.h"
#include "math.h"

// TODO Configure values
/*
** Constants to configure movement of lift
*/
#define LIFT_BIAS_THRESH 0      // How far lift sides need to be off to correct
#define LIFT_BIAS_CORRECT_P 1.5 // P te`rm to use when correcting offset of lift
#define LIFT_TARGET_THRESH 25   // How far lift's from target to try go to it
#define LIFT_TARGET_CORRECT_P_UP                                               \
  0.25 // P term to use when setting speed to target up
#define LIFT_TARGET_CORRECT_P_DOWN                                             \
  0.25 // P term to use when setting speed to target down
#define HEIGHT_INCREMENT_CONE 117 // Height to add to goal per cone

// TODO Configure speeds
/*
** Constants to configure resting speeds of lift
*/
#define LIFT_SPEED_HOLDING 0
#define LIFT_SPEED_IDLE 0
#define LIFT_SPEED_MIN 20

// TODO Configure values
#define LIFT_MIN_HEIGHT 0
#define LIFT_MAX_HEIGHT 1511

//------------------------------------------------------------------------------

/*
** Variables used to track lift height
*/
int liftStartLeft = 0;      // The position of sensor where the lift started
int liftStartRight = 0;     // The position of sensor where the lift started
int liftTarget = 0;         // Delta from start for the lift to reach
bool netTargetDir = DIR_UP; // The overall direction of the lift movements
bool shouldOvershoot =
    true; // Whether or not the lift should oscillate to correct

//------------------------------------------------------------------------------

int getLiftPosLeft() { return analogRead(ANALOG_POT_LIFT_L) - liftStartLeft; }

int getLiftPosRight() { return analogRead(ANALOG_POT_LIFT_R) - liftStartRight; }

// Sets the starting position for the lift potentiometers
void setLiftStart(int posLeft, int posRight) {
  liftStartLeft = posLeft;
  liftStartRight = posRight;
}

// Sets the lift starting positions to be the current values
void setLiftStartAsNow() {
  liftStartLeft = 0;
  liftStartRight = 0;
  setLiftStart(getLiftPosLeft() + 5, getLiftPosRight() + 5);
}

/*
** Gets the averaged height of the two lift sensors
**
** RETURNS:
**    int: The averaged height of the two sensors w/out filter
*/
int getLiftPos() {
  return floor((((double)(getLiftPosLeft() + getLiftPosRight())) / 2.0) + 0.5);
}

int getLiftOffset() { return getLiftPosLeft() - getLiftPosRight(); }

int getLiftError() { return getLiftPos() - liftTarget; }

//------------------------------------------------------------------------------

// Used to get which part of the lift is moving faster than the other (bias)
// -1: Left is behind
// 0: No correction needed
// 1: Right is behind
char getLiftBias(bool direction) {
  // If the lift has enough offset to need to be fixed
  if (abs(getLiftOffset()) > LIFT_BIAS_THRESH) {
    if (DIR_UP == direction) { // If lift is moving upward
      if (getLiftPosLeft() > getLiftPosRight())
        return 1; // Lift is going up, so right is behind (1)
      else
        return -1; // Lift is going up, so left  is behind (-1)
    } else {       // If lift is moving downward
      if (getLiftPosLeft() > getLiftPosRight())
        return -1; // Lift is going down, so left  is behind (-1)
      else
        return 1; // Lift is going down, so right is behind (1)
    }
  }
  return 0; // Lift sides don't need correction, return 0
}

// Fixes alignant bias by slowing the biased side
// Corrects left side by slowing right and vise versa, keep in mind when reading
int corretLiftBias(bool side, int speed, bool direction) {
  int bias = getLiftBias(direction); // Get the current bias
  // If bias isn't 0 and the bias is the side we're checking for, return
  // speed
  if (bias != 0 &&                          // If bias isn't 0
      ((bias == -1 && side == DIR_RIGHT) || // If left is behind & is right
       (bias == 1 && side == DIR_LEFT)))    // If right is behind & is left
  {
    int speedABS = abs(speed);
    bool speedDir = speed > 0;
    // Slow down the ahead side by p * error
    // First convert the speed to RPM, then p correct to that linearized model
    // Then convert the RPM back to motor speed in order to set the motors
    // properly
    // ----
    // return RPMToMotor(motorToRPM(abs(speed)) -
    // (LIFT_BIAS_CORRECT_P * abs(getLiftOffset())));
    // ----
    // return (speedDir ? 1 : -1) * (RPMToMotor(motorToRPM(speedABS) -
    //  (LIFT_BIAS_CORRECT_P * abs(getLiftOffset()))));
    // ----
    return (speedDir ? 1 : -1) *
           (speedABS - (LIFT_BIAS_CORRECT_P * abs(getLiftOffset())));
  }
  return speed; // No correction needed, return the speed
}

//------------------------------------------------------------------------------

// Directly sets lift motor speeds
// Shouldn't be used unless making control loops
void setLiftSpeedRaw(int speedLeft, int speedRight) {
  // void setLiftSpeedRaw(int speed) {
  motorSet(MOTOR_LIFT_1, speedLeft);
  motorSet(MOTOR_LIFT_2, speedRight);
}

// Sets lift speed using bias correction
// Positive speed upward, negative downward
void setLiftSpeed(int speed) {
  /*if (speed == 0) {
    setLiftSpeedRaw(0, 0);
    return;
  }*/
  
  setLiftSpeedRaw(-speed, speed);
  return;

  // TODO Check bounds
  bool direction = speed > 0;
  bprint(1, "---------------------------\r\n");
  bprintf(1, "Pot Left: %d\r\n", analogRead(ANALOG_POT_LIFT_L));
  bprintf(1, "Pot Right: %d\r\n", analogRead(ANALOG_POT_LIFT_R));
  bprintf(1, "Off Left: %d\r\n", analogRead(ANALOG_POT_LIFT_L) - liftStartLeft);
  bprintf(1, "Off Right: %d\r\n",
          analogRead(ANALOG_POT_LIFT_R) - liftStartRight);
  bprintf(1, "Cor Left: %d\r\n", corretLiftBias(DIR_LEFT, speed, direction));
  bprintf(1, "Cor Right: %d\r\n", corretLiftBias(DIR_RIGHT, speed, direction));
  bprint(1, "---------------------------\r\n\r\n");

  setLiftSpeedRaw(-1 * corretLiftBias(DIR_LEFT, speed, direction),
                  corretLiftBias(DIR_RIGHT, speed, direction));
}

// Sets the value for the lift to try and reach
void setLiftTarget(int target) {
  liftTarget = target;
  // TODO Add anything else needed
  netTargetDir = (getLiftPos() - liftTarget) > 0 ? DIR_UP : DIR_DOWN;
}

//------------------------------------------------------------------------------

// Sets the lift target to the right height for the goal type and cone count
void setLiftTargetSmart(int cones) {
  setLiftTarget(200 + (HEIGHT_INCREMENT_CONE * cones));
}

int getLiftTarget() { return liftTarget; }

// Whether or not the lift is at it's target
bool isLiftAtTarget() { return abs(getLiftError()) <= LIFT_TARGET_THRESH; }

// Wait until the lift is at it's desired target
void waitForLift() {
  while (!isLiftReady())
    delay(10);
}

void setShouldOvershootLift(bool shouldOvershootLift) {
  shouldOvershoot = shouldOvershootLift;
}

bool isLiftOvershooting() {
  bool correctionDirection =
      (getLiftPos() - liftTarget) > 0 ? DIR_UP : DIR_DOWN;
  return ((shouldOvershoot) ? (correctionDirection != netTargetDir) : false);
}

bool isLiftReady() { return isLiftAtTarget() || isLiftOvershooting(); }

int pLoopDetermineLiftSpeed() {
  // If lift is higher than target, move down, otherwise up
  bool correctionDirection =
      (getLiftPos() - liftTarget) > 0 ? DIR_UP : DIR_DOWN;
  int speed = (correctionDirection ? LIFT_TARGET_CORRECT_P_DOWN
                                   : LIFT_TARGET_CORRECT_P_UP) *
              abs(getLiftError());
  speed = (speed > 127) ? 127 : speed;
  speed = (speed < LIFT_SPEED_MIN) ? LIFT_SPEED_MIN : speed;
  speed *= (correctionDirection ? -1 : 1);
  // Set lift speed to Kp * error * directionMultiplier
  return speed;
}

//------------------------------------------------------------------------------

// Task to handle the control of the lift
void liftControl(void *ignored) {
  while (true) {
    // TODO Handle upper and lower bounds
    // If the error is great enough, move lift towards target
    // also if it can overshoot, do not change direction just stop
    if (!isLiftReady()) {
      setLiftSpeed(pLoopDetermineLiftSpeed());
    } else { // Otherwise let the lift be still
      // TODO Determine whether or not to use LIFT_SPEED_HOLDING or
      //    LIFT_SPEED_IDLE
      // setLiftSpeed(LIFT_SPEED_HOLDING);
      // TODO UNCOMMENT ABOVE!!!!!
    }
    delay(10);
  }
}
