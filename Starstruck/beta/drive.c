#define DRIVE_THRESHOLD_FORWARD 20 // Joystick forward threshold
#define DRIVE_THRESHOLD_TURN 20 // Joystick turn threshold
#define DRIVE_THRESHOLD_STRAFE 20 // Joystick strafe threshold

/////////////////////////////////////////////////////////////////////////////////////////
//
//                        Normal drive methods (No sensors)
//
/////////////////////////////////////////////////////////////////////////////////////////

// Cleaner way of setting all four drive motor speeds.
// Drive with checks to rule out errors with joystick controls
// PARAMETERS:
//	int: -127 to 127, speed for the front left motor
//	int: -127 to 127, speed for the back left motor
//	int: -127 to 127, speed for the front right motor
//	int: -127 to 127, speed for the back right motor
void driveRaw(int speedFL, int speedBL, int speedFR, int speedBR) {
	motor[driveFL] = speedFL; // Front left
	motor[driveBL] = speedBL; // Back left
	motor[driveFR] = speedFR; // Front right
	motor[driveBR] = speedBR; // Back right
}

// Set mecanum wheel motor speeds based on forward, turn, and strafe speeds
// PARAMETERS:
//	int: -127 to 127, speed to drive forward or backward respectively
//	int: -127 to 127, speed to turn left or right respectively
//	int: -127 to 127, speed to strafe left or right respectively
void drive(int speedForward, int speedTurn, int speedStrafe) {
	int speedFL = speedForward + speedTurn + speedStrafe;
	int speedBL = speedForward + speedTurn - speedStrafe;
	int speedFR = speedForward - speedTurn - speedStrafe;
	int speedBR = speedForward - speedTurn + speedStrafe;
	driveRaw(speedFL, speedBL, speedFR, speedBR);
}

// Drive with checks to rule out errors with joystick controls
// NOTE:
// 	Multipliers should always be (0 <= x <= 1.0). This avoids going too fast or slow and going out of proportion
// PARAMETERS:
//	int: -127 to 127, speed to drive forward or backward respectively
//	int: -127 to 127, speed to turn left or right respectively
//	int: -127 to 127, speed to strafe left or right respectively
//	float: What to reduce forward/backward speed to (0.7 -> 70% of input)
//	float: What to reduce left/right turn speed to (0.7 -> 70% of input)
//	float: What to reduce left/right strafe speed to (0.7 -> 70% of input)
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe, float forwardMultiplier, float turnMultiplier, float strafeMultiplier) {
	int multipliedSpeedForward = speedForward; // ((float) speedForward)*forwardMultiplier;
	int multipliedSpeedTurn = speedTurn; //((float) speedTurn)*turnMultiplier;
	int multipliedSpeedStrafe = speedStrafe; //((float) speedStrafe)*strafeMultiplier;

	if (abs(multipliedSpeedForward) <= DRIVE_THRESHOLD_FORWARD) multipliedSpeedForward = 0;
	if (abs(multipliedSpeedTurn) <= DRIVE_THRESHOLD_TURN) multipliedSpeedTurn = 0;
	if (abs(multipliedSpeedStrafe) <= DRIVE_THRESHOLD_STRAFE) multipliedSpeedStrafe = 0;

	// TODO Cooler stuff than just thresholds :P

	drive(multipliedSpeedForward, multipliedSpeedTurn, multipliedSpeedStrafe); // Pass off the checked values to drive
}

// Overriding method. See driveWithLogic(int, int, int, float, float, float).
// NOTE:
// 	-> Sets multipliers to 1.0
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe) {
	driveWithLogic(speedForward, speedTurn, speedStrafe, 1.0, 1.0, 1.0);
}

// Set each side of the drive to a certain speed.
// PARAMETERS:
//  int: How fast to move the left side (-127 to 127)
//  int: How fast to move the right side (-127 to 127)
void driveTank(int speedLeft, int speedRight){
	driveRaw(speedLeft,speedLeft,speedRight,speedRight);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                        Smart drive methods (Encoders & PID)
//
/////////////////////////////////////////////////////////////////////////////////////////

// Sets the encoders on all of our drive motors back to 0.
void clearDriveEncoders() {
	nMotorEncoder[driveFR] = 0;
	nMotorEncoder[driveFL] = 0;
	nMotorEncoder[driveBL]= 0;
	nMotorEncoder[driveBR] = 0;
}

// Used to set the tick targets, because tasks can't take parameters
long ticksFrontLeft, ticksBackLeft, ticksFrontRight, ticksBackRight = 0;

// Clean way to set above motor targets
// PARAMETERS:
//  int: Target ticks to change for front left motor
//  int: Target ticks to change for back left motor
//  int: Target ticks to change for front right motor
//  int: Target ticks to change for back right motor
// NOTE:
//  - You should enter the delta, the method will figure out the total
//  - For example, if you want it to go forward 300 and the encoder is already
//    at 1000, you should still only put in 300.
//  - Negative = backwards wheel movement.
void setupMotorTicks(long frontl, long backl, long frontr, long backr) {
	ticksFrontLeft = frontl + nMotorEncoder[driveFL];
	ticksBackLeft = backl + nMotorEncoder[driveBL];
	ticksFrontRight = frontr + nMotorEncoder[driveFR];
	ticksBackRight = backr + nMotorEncoder[driveBR];
}

// Drive tick method. Controls left and right side useful for point turns.
// Reads encoder valte of the front motors for each side.
// PARAMETERS:
//  int: How fast to move the left side drive (-127 to 127)
//  long: How far to spin the left drive, in ticks. If above speed is negative, so should this number.
//  int: How fast to move the right side drive (-127 to 127)
//  long: How far to spin the right drive, in ticks. If above speed is negative, so should this number.
void driveTilRightLeft(int speedLeft, long ticksLeft, int speedRight, long ticksRight) {
	setupMotorTicks(ticksLeft, ticksLeft, ticksRight, ticksRight);
	// While we're not where we want to be (Ternary allows for both forward and backward input)
	while ((ticksLeft > 0 ? nMotorEncoder[driveFL] < ticksFrontLeft : nMotorEncoder[driveFL] > ticksFrontLeft) ||
		(ticksRight > 0 ? nMotorEncoder[driveFR] < ticksFrontRight : nMotorEncoder[driveFR] > ticksFrontRight)) {
		driveRaw(speedLeft, speedLeft, speedRight, speedRight);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                        CRS (Currently on the backburner)
//
/////////////////////////////////////////////////////////////////////////////////////////

// Considers the input as if the front of the robot was at a different angle (For example, considering
// -> the right side of the robot to be the front, or the front right corner to be the front) all while
// -> facing the same direction so you can move towards a scoring object and launch stuff over the fence
// CRS: Correction of Rotation Stick (or crazy rotational stuff)
// Beta, will add documentation when complete
/*
void driveWithCRS(int speedForward, int speedStrafe, float startDegree, float currentDegree) {
	if (speedForward <= DRIVE_THRESHOLD_FORWARD) speedForward = 0;
	if (speedStrafe <= DRIVE_THRESHOLD_STRAFE) speedStrafe = 0;
	float degree = degreeToRad(currentDegree-startDegree);
	// Considering speed as x, x' = y*sin(a) + x*cos(a)
	speedForward = speedStrafe*sin(degree) + speedForward*cos(degree);
	// Considering strafe as y, y' = y*cos(a) - x*sin(a)
	speedStrafe = speedStrafe*cos(degree) - speedForward*sin(degree);
}
*/
