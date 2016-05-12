#define DRIVE_THRESHOLD_FORWARD 20 // Joystick forward threshold
#define DRIVE_THRESHOLD_TURN 20 // Joystick turn threshold
#define DRIVE_THRESHOLD_STRAFE 20 // Joystick strafe threshold

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
	// Not using driveRaw, easier to read this in this case
	motor[driveFL] = speedForward + speedTurn + speedStrafe;
	motor[driveBL] = speedForward + speedTurn - speedStrafe;
	motor[driveFR] = speedForward - speedTurn - speedStrafe;
	motor[driveBR] = speedForward - speedTurn + speedStrafe;
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
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe, float forwardMultiplier, float speedMultiplier, float strafeMultiplier) {
	if (speedForward <= DRIVE_THRESHOLD_FORWARD) speedForward = 0;
	if (speedTurn <= DRIVE_THRESHOLD_TURN) speedTurn = 0;
	if (speedStrafe <= DRIVE_THRESHOLD_STRAFE) speedStrafe = 0;

	// TODO Cooler stuff than just thresholds :P

	drive(speedForward, speedTurn, speedStrafe); // Pass off the checked values to drive
}

// Overriding method. See driveWithLogic(int, int, int, float, float, float).
// NOTE:
// 	-> Sets multipliers to 1.0
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe) {
	driveWithLogic(speedForward, speedTurn, speedStrafe, 1.0, 1.0, 1.0);
}

// Considers the input as if the front of the robot was at a different angle (For example, considering
// -> the right side of the robot to be the front, or the front right corner to be the front) all while
// -> facing the same direction so you can move towards a scoring object and launch stuff over the fence
// CRS: Correction of Rotation Stick (or crazy rotational stuff)
// Beta, will add documentation when complete
void driveWithCRS(int speedForward, int speedStrafe, float startDegree, float currentDegree) {
	if (speedForward <= DRIVE_THRESHOLD_FORWARD) speedForward = 0;
	if (speedStrafe <= DRIVE_THRESHOLD_STRAFE) speedStrafe = 0;
	float degree = degreeToRad(currentDegree-startDegree);
	// Considering speed as x, x' = y*sin(a) + x*cos(a)
	speedForward = speedStrafe*sin(degree) + speedForward*cos(degree);
	// Considering strafe as y, y' = y*cos(a) - x*sin(a)
	speedStrafe = speedStrafe*cos(degree) - speedForward*sin(degree);
}
