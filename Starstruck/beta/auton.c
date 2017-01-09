bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 5;
int currentMode = minAutonomous;

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

void tempEncoderForward(int speed, int ticks){
	int tickTarget = (speed < 0) ? ((ticks*-1)
	+ nMotorEncoder[driveFL]) : (ticks + nMotorEncoder[driveFL]);
	driveRaw(speed,speed,speed,speed);
	while((speed<0) ? nMotorEncoder[driveFL] > tickTarget : nMotorEncoder[driveFL] < tickTarget){
		wait1Msec(10);
	}
	driveRaw(0,0,0,0);
}

// Positive speed for right, negative for left
void tempEncoderPoint(int speed, int ticks){
	int tickTarget = (speed < 0) ? ((ticks*-1)
	+ nMotorEncoder[driveFL]) : (ticks + nMotorEncoder[driveFL]);
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
	while((speed<0) ? nMotorEncoder[driveFL] > tickTarget : nMotorEncoder[driveFL] < tickTarget){
		wait1Msec(10);
	}
	driveRaw(0,0,0,0);
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

// Auton modes should handle team color and positon
// (Thanks vex for making the pole opposite for each side)
void runAuton() {
	string debug = "";
	sprintf(debug,"Running Auton Mode: %i",currentMode);
	writeDebugStreamLine(debug);
	int sideMult = getAutonPosition() ? 1 : -1;
	// Mode 1 [Default, Do nothing] ---------------------------------------------

	// Mode 2 [Max Points] ------------------------------------------------------
	if (currentMode == 2) {
		//turnToAngle(650 * sideMult, -60 * sideMult);
		//driveStraightPID(-1200,5000);
		clearDriveEncoders();
	}

	// Mode 3 [Compat] ----------------------------------------------------------
	if (currentMode == 3) {
		driveRaw(-80,-80,-80,-80);
		wait1Msec(3000);
		driveRaw(0,0,0,0);
	}

	// Mode 4 [C Compat] --------------------------------------------------------
	if (currentMode == 4) {
		driveRaw(-80,-80,-80,-80);
		wait1Msec(2000);
		driveRaw(0,0,0,0);
	}

	// Mode 5 [Skills] ----------------------------------------------------------
	if (currentMode == 5) {
		int cooldownTime = 200; // Time so motors don't change too quick
		// TODO Figure out how long it takes to place stuff
		int placementTimeStars = 3000;
		int placementTimeCubes = 3000; // Might be 0, since robot goes and comes

		/*
		** Green: Back up for stars and wait for placement
		*/
		pidDriveStraightLimit(-600,3000);
		// TODO Deploy claw
		wait1Msec(placementTimeStars); // Wait to place stars

		/*
		** Red: Go to fence and score stars
		** (TODO Replace with gyro PID)
		*/
		/* TODO Change values */ int fenceDistance = 600;
		/* TODO Change values */ int fenceTurn = 600;
		// Drive, dump, repeat
		for (int i=0; i < 3; i++) { // Stars, then 2 cubes
			// Drive up to fence
			pidDriveStraightLimit(-1*fenceDistance,10000);
			wait1Msec(cooldownTime); // Motor cooldown

			// Turn towards fence
			pidDrivePointLimit(-1*fenceTurn*sideMult,5000);

			// TODO Lift and then drop stars
			// TODO start lowering lift

			if(i < 2) { // If we want to go back to starting tile
				// Turn away from fence
				pidDrivePointLimit(fenceTurn*sideMult,5000);
				wait1Msec(cooldownTime); // Motor cooldown
				pidDriveStraight(fenceDistance); // Drive back to tile
				// TODO Check and adjust claw values
				waitForPidLimit(8000); // Make sure we wait for PID
			}
		}

		/*
		** Yellow: Get 3 fence stars and dump
		** (TODO Replace with gyro PID)
		*/
		// TODO Check and adjust claw values
		// After dumping, turn towards yellow stars
		/* TODO Change values */ pidDrivePointLimit(90*sideMult,5000);

		// Drive into the stars
		/* TODO Change values */ pidDriveStraightLimit(-1*600,10000);

		// TODO Close claw

		// Turn to fence to dump
		/* TODO Change values */ pidDrivePointLimit(-90*sideMult,5000);

		// TODO Lift

		// TODO Release and dump

		// TODO Lower lift


		/*
		** Orange: Grab center cube
		*/

		// TODO Finish auton
	}
}
