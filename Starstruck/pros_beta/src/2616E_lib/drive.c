#include "main.h"

unsigned char driveFL, driveBL, driveFR, driveBR;

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
    driveFL = _driveFL; // Set our front left motor
    driveBL = _driveBL; // Set our back  left motor
    driveFR = _driveFR; // Set our front right motor
    driveBR = _driveBR; // Set our back  right motor
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
  const char * string = "Unknown") {
  if (motor != NULL)
    motorSet(motor, speed);
  else
    printf("Tried to set motor speed, but it wasn't initialized! [%s]", string);
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
  driveIfValid(driveFL, speedFL, "driveFL"); // Set drive front left
  driveIfValid(driveBL, speedBL, "driveBL"); // Set drive back  left
  driveIfValid(driveFR, speedFR, "driveFR"); // Set drive front right
  driveIfValid(driveBR, speedBR, "driveBR"); // Set drive back  right
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
