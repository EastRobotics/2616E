bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 6;
int currentMode = minAutonomous;

void waitForLift(int speed); //

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Setting methods
//
// All of our methods for getting and setting autonomous factors
//
/////////////////////////////////////////////////////////////////////////////////////////


bool setAutonMode(int mode) {
	if ((mode < minAutonomous) || (mode > maxAutonomous)) { // Number is invalid
		return false;
		} else { // Number is valid
		currentMode = mode;
		return true;
	}
}

int getAutonMode() {
	return currentMode;
}

int getAutonModeMin() {
	return minAutonomous;
}

int getAutonModeMax() {
	return maxAutonomous;
}

// False for left, true for right
void setAutonPosition(bool setPosition) {
	position = setPosition;
}

bool getAutonPosition() {
	return position;
}

// False for red, true for blue
void setAutonColor(bool setColor) {
	color = setColor;
}

bool getAutonColor() {
	return color;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Bot methods
//
// Methods for doing different actions on the robot, like launching, intaking, etc using
// our robot's sensor array. Most methods forwarded from main.
//
/////////////////////////////////////////////////////////////////////////////////////////

void driveForTime(int powerFL, int powerBL, int powerFR, int powerBR, int time){
	driveRaw(powerFL,powerBL,powerFR,powerBR);
	wait1Msec(time);
	driveRaw(0,0,0,0);
}

// Now with Quadrature encoders
void tempEncoderForward(int speed, int ticks){
	int tickTarget = (speed < 0) ? ((ticks*-1)
	+nMotorEncoder[driveBL]) : (ticks + nMotorEncoder[driveBL]);
	driveRaw(speed,speed,speed,speed);
	while((speed<0) ? nMotorEncoder[driveBL] > tickTarget : nMotorEncoder[driveBL] < tickTarget){
		wait1Msec(10);
	}
	driveRaw(0,0,0,0);
}

// Positive speed for right, negative for left
void tempEncoderPoint(int speed, int ticks){
	int tickTarget = (speed < 0) ? ((ticks*-1)
	+ nMotorEncoder[driveBL]) : (ticks + nMotorEncoder[driveBL]);
	int speedLeft = 0;
	int speedRight = 0;
	if(speed>0){
		speedRight = (speed < 0) ? speed : speed*-1;
		speedLeft = (speed < 0) ? speed*-1 : speed;
		} else {
		speedRight = (speed < 0) ? speed*-1 : speed;
		speedLeft = (speed < 0) ? speed : speed*-1;
	}
	driveRaw(speedLeft,speedLeft,speedRight,speedRight);
	while((speed<0) ? nMotorEncoder[driveBL] > tickTarget : nMotorEncoder[driveBL] < tickTarget){
		wait1Msec(10);
	}
	driveRaw(0,0,0,0);
}
int clawTargetLeft;
int clawTargetRight;
bool clawRunning = false;
int clawSpeed;

task adjustClawPosition() {
	int clawPosRight;
	int clawPosLeft;
	const int misal = 30;
	do { // Align the claws
		const int misal = 20;
		int clawPosLeft = getMotorEncoder(clawL);
		int clawPosRight = getMotorEncoder(clawR);
		//clawPosRight *= -1; // so they both count up as they go out
		clawPosLeft *= -1;
		// If we're misaligning, counteract
		// Left claw
		if (abs(clawPosLeft-clawTargetLeft) > misal) {
			motor[clawL] = clawPosLeft-clawTargetLeft > 0 ? -1 * clawSpeed : clawSpeed;
			} else {
			motor[clawL] = 0;
		}
		// Right claw
		if (abs(clawPosRight-clawTargetRight) > misal) {
				motor[clawR] = clawPosRight-clawTargetRight > 0 ? clawSpeed : -1 * clawSpeed;
			} else {
			motor[clawR] = 0;
		}
		wait1Msec(20);
		if (clawRunning)
			clawRunning = (abs(clawPosRight-clawTargetRight) > misal) || (abs(clawPosLeft-clawTargetLeft) > misal);
	} while // They aren't aliged
		(true); // Left claw not aligned
}


void stopClaw() {
	clawTargetRight = getMotorEncoder(clawL);
	clawTargetLeft = getMotorEncoder(clawR);
}

void stopClawTask(){
	stopTask(	adjustClawPosition );
}

void startClawTask() {
	startTask( adjustClawPosition );
}

// Should really only be used for opening, see clawClose(int ms)
void setClaw(int target, int speed) {
	clawRunning = true;
	clawSpeed = speed;
	clawTargetLeft = target;
	clawTargetRight = target;
}

// Used for when we want to clamp an unknown orientation, and can't guarentee a certain close value
void clawClose(int ms, int speed) {
	setClaw(-100, speed); // Try and close the claw
	int timeGone = 0;
	while (timeGone < ms || pidRunning) { // While we still have time or haven't reached 0ms
		wait1Msec(10);
		timeGone += 10;
	}
	stopClaw();
}

// Wait for the claw to reach it's target
void waitForClaw() {
	while (clawRunning)
		wait1Msec(20);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Debug methods
//
// Used for finding errors in the autonomous codes
//
/////////////////////////////////////////////////////////////////////////////////////////

//wait until button press to continue
void breakpoint() {
	while(!SensorValue[autonCont]){
		wait1Msec(10);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Auton Modes
//
// All the different modes to handle autonomous function. Should use all the aquired
// auton settings to be as successful as possible
//
/////////////////////////////////////////////////////////////////////////////////////////

task manageClaw();
void moveLiftWithLogic(int speed, bool overrideHold, bool dampenSpeed, bool overrideBounds);

// Auton modes should handle team color and positon
// (Thanks vex for making the pole opposite for each side)
void runAuton() {
	string debug = "";
	sprintf(debug,"Running Auton Mode: %i",currentMode);
	writeDebugStreamLine(debug);
	int sideMult = getAutonPosition() ? 1 : -1; // 1 for right, -1 for left
	stopTask(manageClaw);
	stopClaw();
	//startClawTask();
	// Mode 1 [Default, Do nothing] ---------------------------------------------

	// Mode 2 [Max Points] ------------------------------------------------------
	if (currentMode == 2)  {
		clearDriveEncoders();
		moveLiftWithLogic(0,false,false,false);
		driveForTime(70,70,70,70,750); // Drive at stuff
		wait1Msec(100);
		driveForTime(-70,-70,-70,-70,500); // Back up
		wait1Msec(100);
		motor[clawL] = -127;
		motor[clawR] = 127;
		wait1Msec(500);
		motor[clawL] = motor[clawR] = 0;
		wait1Msec(100);
		driveForTime(70,70,70,70,1250); // Back towards
		motor[clawL] = 127;
		motor[clawR] = -127;
		wait1Msec(500);
		motor[clawL] = 30;
		motor[clawR] = -30;
		wait1Msec(100);
		turnToAngle(((sideMult < 0) ? -1350 : 1350),-100*sideMult);
		wait1Msec(250);
		driveForTime(-70,-70,-70,-70,1250); // Drive at stuff
		wait1Msec(500);
		waitForLift(100);
		motor[clawL] = -127;
		motor[clawR] = 127;
		wait1Msec(500);
		motor[clawL] = motor[clawR] = 0;

		/*turnToAngle(((sideMult > 0) ? 3334 : 266) ,-100*sideMult);
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		driveForTime(-100,-100,-100,-100,1000); //TODO: Switch to encoders
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		turnToAngle(((sideMult > 0) ? 804 : 2796),-100*sideMult);
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		setClaw(500,-127);
		waitForClaw();
		setClaw(0,127);
		waitForClaw();
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		turnToAngle(10,-100*sideMult);
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		driveForTime(-100,-100,-100,-100,500); //TODO: Switch to encoders
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		waitForLift(100);
		setClaw(500,-127);
		waitForClaw();
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		waitForLift(-100);
		waitForPid();
		waitForClaw();*/
	}

	// Mode 3 [Compat] ----------------------------------------------------------
	if (currentMode == 3) {
		wait1Msec(5000);
		driveForTime(70,70,70,70,750); // Drive at stuff
		wait1Msec(100);
		driveForTime(-70,-70,-70,-70,500); // Back up
		wait1Msec(100);
		motor[clawL] = -127;
		motor[clawR] = 127;
		wait1Msec(500);
		motor[clawL] = motor[clawR] = 0;
		wait1Msec(100);
		driveRaw(127,127,127,127);
		wait1Msec(1500);
		driveRaw(0,0,0,0);
	}

	// Mode 4 [C Compat] --------------------------------------------------------
	if (currentMode == 4) {
		driveRaw(127,127,127,127);
		wait1Msec(3000);
		driveRaw(0,0,0,0);
	}

	// Mode 5 [Cube Toss] ------------------------------------------------------
	if(currentMode == 5) {

	}

	// Mode 5 [Skills] ----------------------------------------------------------
	if (currentMode == 6) {
		int cooldownTime = 200; // Time so motors don't change too quick
		// TODO Figure out how long it takes to place stuff
		int placementTimeStars = 3000;
		int placementTimeCubes = 3000; // Might be 0, since robot goes and comes
		int dumpValue = 300;

		/*
		** Green: Back up for stars and wait for placement
		*/
		pidDriveStraightLimit(-600,3000);
		/* TODO Change values */ setClaw(500,100); // Deploy claw
		/* TODO Delete */ breakpoint();

		/*
		** Red: Go to fence and score stars
		** (TODO Replace with gyro PID)
		*/
		/* TODO Change values */ int fenceDistance = 600;
		/* TODO Change values */ int fenceTurn = 600;
		// Drive, dump, repeat
		for (int i=0; i < 3; i++) { // Stars, then 2 cubes
			wait1Msec(i == 0 ? placementTimeStars : placementTimeCubes); // Wait to place load
			waitForClaw(); // Make sure claw is ready
			/* TODO Change values */ clawClose(1000,100); // Clamp load for 1 second
			/* TODO Delete */ breakpoint();
			// Drive up to fence
			pidDriveStraightLimit(-1*fenceDistance,10000);
			wait1Msec(cooldownTime); // Motor cooldown
			/* TODO Delete */ breakpoint();

			// TODO Lift load so it doesn't drag?

			// Turn towards fence
			pidDrivePointLimit(-1*fenceTurn*sideMult,5000);
			/* TODO Delete */ breakpoint();

			// TODO Lift
			/* TODO Change values */ setClaw(dumpValue,127); // Drop the load, open just enough to drop
			waitForClaw();
			/* TODO Delete */ breakpoint();
			// TODO start lowering lift

			if(i < 2) { // If we want to go back to starting tile
				/* TODO Change values */ setClaw(500,127); // Set claw at whatever we want for pickup
				// Turn away from fence
				/* TODO Delete */ breakpoint();
				pidDrivePointLimit(fenceTurn*sideMult,5000);
				wait1Msec(cooldownTime); // Motor cooldown
				/* TODO Delete */ breakpoint();
				pidDriveStraight(fenceDistance); // Drive back to tile
				// TODO Check and adjust claw values
				waitForPidLimit(8000); // Make sure we wait for PID
			}
		}

		/*
		** Yellow: Get 3 fence stars and dump
		** (TODO Replace with gyro PID)
		*/
		/* TODO Change values */ setClaw(400,127); // Set claw at grabbing pos
		/* TODO Delete */ breakpoint();
		// After dumping, turn towards yellow stars
		/* TODO Change values */ pidDrivePointLimit(90*sideMult,5000);
		/* TODO Delete */ breakpoint();

		// Drive into the stars
		/* TODO Change values */ pidDriveStraightLimit(-1*600,10000);
		/* TODO Delete */ breakpoint();

		/* TODO Change values */ clawClose(1000,100); // Clamp load for 1 second
		/* TODO Delete */ breakpoint();

		// Turn to fence to dump
		/* TODO Change values */ pidDrivePointLimit(-90*sideMult,5000);
		/* TODO Delete */ breakpoint();

		// TODO Lift

		/* TODO Change values */ setClaw(dumpValue,127); // Drop the load, open just enough to drop
		waitForClaw();
		/* TODO Delete */ breakpoint();

		// TODO Lower lift


		/*
		** Orange: Grab center cube
		*/

		// TODO Finish auton
	}
	stopClawTask();
	startTask(manageClaw);
}
