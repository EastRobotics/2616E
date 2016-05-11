#define DRIVE_THRESHOLD_FORWARD 20 // Joystick forward threshold
#define DRIVE_THRESHOLD_TURN 20 // Joystick turn threshold
#define DRIVE_THRESHOLD_STRAFE 20 // Joystick strafe threshold

// Cleaner way of setting all four drive motor speeds.
void driveRaw(int speedFL, int speedBL, int speedFR, int speedBR) {
	motor[driveFL] = speedFL; // Front left
	motor[driveBL] = speedBL; // Back left
	motor[driveFR] = speedFR; // Front right
	motor[driveBR] = speedBR; // Back right
}

void drive(int speedForward, int speedTurn, int speedStrafe) {
	// Not using driveRaw, easier to read this in this case
	motor[driveFL] = speedForward + speedTurn + speedStrafe;
	motor[driveBL] = speedForward + speedTurn - speedStrafe;
	motor[driveFR] = speedForward - speedTurn - speedStrafe;
	motor[driveBR] = speedForward - speedTurn + speedStrafe;
}

// Multipliers should always be 1.0 or under. It won't pass otherwise
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe, float forwardMultiplier, float speedMultiplier, float strafeMultiplier) {
	if (speedForward <= DRIVE_THRESHOLD_FORWARD) speedForward = 0;
	if (speedTurn <= DRIVE_THRESHOLD_TURN) speedTurn = 0;
	if (speedStrafe <= DRIVE_THRESHOLD_STRAFE) speedStrafe = 0;

	drive(speedForward, speedTurn, speedStrafe); // Pass off the checked values to drive
}

void driveWithCRS(int speedForward, int speedStrafe, float startDegree, float currentDegree) {
	if (speedForward <= DRIVE_THRESHOLD_FORWARD) speedForward = 0;
	if (speedStrafe <= DRIVE_THRESHOLD_STRAFE) speedStrafe = 0;
	float degree = currentDegree-startDegree;
	// Considering speed as x, x' = y*sin(a) + x*cos(a)
	speedForward = speedStrafe*sin(degree) + speedForward*cos(degree);
	// Considering strafe as y, y' = y*cos(a) - x*sin(a)
	speedStrafe = speedStrafe*cos(degree) - speedForward*sin(degree);
}
