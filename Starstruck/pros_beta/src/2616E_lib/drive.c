#include "main.h"

#define DRIVE_THRESHOLD_FORWARD 15 // Joystick forward threshold
#define DRIVE_THRESHOLD_TURN 15 // Joystick turn threshold
#define DRIVE_THRESHOLD_STRAFE 15 // Joystick strafe threshold
#define INITIAL_DRIVE_POWER 25 //The power that drive with logic will start it's linear function at for drive power
#define JOYSTICK_MOVEMENT_THRESHOLD 15 //The amount the joystick has to move for it to be used in the linear function to calculate RPM

unsigned char driveFL, driveBL, driveFR, driveBR;
bool driveFLReverse, driveBLReverse, driveFRReverse, driveBRReverse;

/*
** Sets the drive motors we're using. This makes it so we don't need to provide
** the ports every time we want to do any drive methods.
**
** PARAMETERS:
**   unsigned char: Front left  drive motor port
**   unsigned char: Back  left  drive motor port
**   unsigned char: Front right drive motor port
**   unsigned char: Back  right drive motor port
*/
void driveInit(unsigned char _driveFL, unsigned char _driveBL,
  unsigned char _driveFR, unsigned char _driveBR) {
    print("[ELib] Initialized drive motors\n");
    driveFL = _driveFL; // Set our front left motor
    driveBL = _driveBL; // Set our back  left motor
    driveFR = _driveFR; // Set our front right motor
    driveBR = _driveBR; // Set our back  right motor
}

/*
** Sets whether each drive motor should be reverse or not
**
** PARAMETERS:
**   bool: Front left  drive motor reverse
**   bool: Back  left  drive motor reverse
**   bool: Front right drive motor reverse
**   bool: Back  right drive motor reverse
*/
void driveSetReverse(bool _driveFLReverse, bool _driveBLReverse,
  bool _driveFRReverse, bool _driveBRReverse) {
    driveFLReverse = _driveFLReverse;
    driveBLReverse = _driveBLReverse;
    driveFRReverse = _driveFRReverse;
    driveBRReverse = _driveBRReverse;
  }

/*
** Sets the given motor's speed, assuming the motor is valid. Otherwise, an
** error is output to the console.
**
** PARAMETERS:
**   unsigned char: Motor port to be set
**   int: The speed of the motor, -127 to 127
**
** OPTIONAL PARAMETERS:
**   const char *: Text representation of motor, used for error message
**
** NOTES:
**   Calls to printf can be taxing, so try to not have any erroneous calls
*/
void driveIfValid(unsigned char motor, int speed,
  const char* string) { // Eventually want default for this, but causing errors
  if (motor != NULL)
    motorSet(motor, speed);
  else
    printf("[ELib] Tried to set motor speed, but it wasn't initialized! [%s]\n",
      string);
}

/*
** Directly sets the motor speeds, checking to make sure the motors have been
** set. Base method for most other drive methods.
**
** PARAMETERS:
**   int: The speed of the front left  motor, -127 to 127
**   int: The speed of the back  left  motor, -127 to 127
**   int: The speed of the front right motor, -127 to 127
**   int: The speed of the back  right motor, -127 to 127
*/
void driveRaw(int speedFL, int speedBL, int speedFR, int speedBR) {
  // Set drive front left
  driveIfValid(driveFL, speedFL * (driveFLReverse ? -1 : 1), "driveFL");
  // Set drive back  left
  driveIfValid(driveBL, speedBL * (driveBLReverse ? -1 : 1), "driveBL");
  // Set drive front right
  driveIfValid(driveFR, speedFR * (driveFRReverse ? -1 : 1), "driveFR");
  // Set drive back  right
  driveIfValid(driveBR, speedBR * (driveBRReverse ? -1 : 1), "driveBR");
}


/*
** Sets the drive motors based on forward and turn speed. This is for tank
** style drives. Use driveHolonomic() for x-drive and mecanum style drives.
**
** PARAMETERS:
**   int: The speed/rate to move, -127 (backward) to 127 (forward)
**   int: The speed/rate to turn, -127 (left)     to 127 (right)
*/
void drive(int speedForward, int speedTurn) {
  int speedFL = speedForward - speedTurn;
  int speedBL = speedForward - speedTurn;
  int speedFR = speedForward + speedTurn;
  int speedBR = speedForward + speedTurn;
  driveRaw(speedFL, speedBL, speedFR, speedBR);
}

/*
** Sets left and right side drive speeds. Very basic drive style, and it's
** suggested to use drive() for normal driving. Made for tank style drives.

** PARAMETERS:
**   int: The speed for left  drive, -127 (backward) to 127 (forward)
**   int: The speed for right drive, -127 (backward) to 127 (forward)
*/
void driveTank(int speedLeft, int speedRight){
  driveRaw(speedLeft, speedLeft, speedRight, speedRight);
}

/*
** Sets the drive motors based on forward, turn, and strafe speed. This is for
** holonomic drives like x-drives and mecanum. Use drive() for tank drives.
**
** PARAMETERS:
**   int: The speed/rate to move,   -127 (backward) to 127 (forward)
**   int: The speed/rate to turn,   -127 (left)     to 127 (right)
**   int: The speed/rate to strafe, -127 (left)     to 127 (right)
*/
void driveHolonomic(int speedForward, int speedTurn, int speedStrafe) {
  int speedFL = speedForward + speedTurn + speedStrafe;
	int speedBL = speedForward + speedTurn - speedStrafe;
	int speedFR = speedForward - speedTurn - speedStrafe;
	int speedBR = speedForward - speedTurn + speedStrafe;
  driveRaw(speedFL, speedBL, speedFR, speedBR);
}

// Drive with checks to rule out errors with joystick controls
// NOTES:
// 	Multipliers should always be (0 <= x <= 1.0). This avoids going too fast or slow and going out of proportion
// PARAMETERS:
//	int: -127 to 127, speed to drive forward or backward respectively
//	int: -127 to 127, speed to turn left or right respectively
//	int: -127 to 127, speed to strafe left or right respectively
//	float: What to reduce forward/backward speed to (0.7 -> 70% of input)
//	float: What to reduce left/right turn speed to (0.7 -> 70% of input)
//	float: What to reduce left/right strafe speed to (0.7 -> 70% of input)
void driveHolonomicWithLogic(int speedForward, int speedTurn, int speedStrafe) {
	int multipliedSpeedForward = speedForward; // ((float) speedForward)*forwardMultiplier;
	int multipliedSpeedTurn = speedTurn; //((float) speedTurn)*turnMultiplier;
	int multipliedSpeedStrafe = speedStrafe; //((float) speedStrafe)*strafeMultiplier;

	char forwardMult = (multipliedSpeedForward < 0) ? -1 : 1;
	char turnMult = (multipliedSpeedTurn < 0) ? -1 : 1;
	char strafeMult = (multipliedSpeedStrafe < 0) ? -1 : 1;

	multipliedSpeedForward = abs(multipliedSpeedForward);
	multipliedSpeedTurn = abs(multipliedSpeedTurn);
	multipliedSpeedStrafe = abs(multipliedSpeedStrafe);

	if (abs(multipliedSpeedForward) <= DRIVE_THRESHOLD_FORWARD)
    multipliedSpeedForward = 0;
	if (abs(multipliedSpeedTurn) <= DRIVE_THRESHOLD_TURN)
    multipliedSpeedTurn = 0;
	if (abs(multipliedSpeedStrafe) <= DRIVE_THRESHOLD_STRAFE)
    multipliedSpeedStrafe = 0;

	if(abs(multipliedSpeedForward) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedForward = 0;
	if(abs(multipliedSpeedTurn) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedTurn = 0;
	if(abs(multipliedSpeedStrafe) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedStrafe = 0;

	// Uses linear interpolation/lerp to fix the logarithmic nature of a motor's
  //  RPM to motor speed ratio into linear growth
	multipliedSpeedForward = getLerpedSpeed(multipliedSpeedForward,
    INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_FORWARD);
	multipliedSpeedTurn = getLerpedSpeed(multipliedSpeedTurn,
    INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_TURN);
	multipliedSpeedStrafe = getLerpedSpeed(multipliedSpeedStrafe,
    INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_STRAFE);

	if (abs(speedForward) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedForward = 0;
	if (abs(speedTurn) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedTurn = 0;
	if (abs(speedStrafe) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedStrafe = 0;

	multipliedSpeedForward *= forwardMult;
	multipliedSpeedTurn *= turnMult;
	multipliedSpeedStrafe *= strafeMult;
	//Double strafe speeds
	multipliedSpeedStrafe *= 2;
	multipliedSpeedStrafe = (multipliedSpeedStrafe > 127) ?
   127 : multipliedSpeedStrafe;

	driveHolonomic(multipliedSpeedForward, multipliedSpeedTurn,
    multipliedSpeedStrafe); // Pass off the checked values to drive
}
