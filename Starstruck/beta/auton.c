bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 8;
int currentMode = minAutonomous;
int intakeLoadTime = 1000;

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

void launch(); // Forward from main
bool getCanLaunch(); // Forward from main

//Run the intake for one second
task intake() {
	motor[intakeL] = motor[intakeR] = 127;
	wait1Msec(intakeLoadTime);
	motor[intakeL] = motor[intakeR] = 0;
}

//synchronous version of startTask(intake);
void startIntake() {
	startTask(intake);
	wait1Msec(round(intakeLoadTime)+3);
}

// Blocking method to wait for the launcher when we're in position
void waitForLauncherReady() {
		while (!getCanLaunch()) {
			wait1Msec(5);
		}
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

task logTicks() {
	while (true) {
		datalogDataGroupStart();
		datalogAddValue(0,nMotorEncoder[driveFL]);
		datalogAddValue(1,nMotorEncoder[driveBL]);
		datalogAddValue(2,nMotorEncoder[driveFR]);
		datalogAddValue(3,nMotorEncoder[driveBR]);
		datalogAddValue(4,2500);
		datalogDataGroupEnd();
		wait1Msec(20);
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
	// Mode 1 [Do nothing]

	// Mode 2 [Default mode/Max Points]
	if (currentMode == 2) {
		// TODO Deploy intake
		launch(); // Launch preload

		/*
		// Preload
		*/
		// Drive away from wall to line up with star, turn intake towards star, then drive to it
		driveEncoderNormal(100, false); // Drive away from wall (ENCODER VALUE NOT SET)
		breakpoint();
		/*
		// First star
		*/
		driveEncoderPointTurn(100,position); // Turn to the middle (ENCODER VALUE NOT SET)
		breakpoint();
		driveEncoderNormal(100, true); // Drive to first star (ENCODER VALUE NOT SET)
		breakpoint();
		waitForLauncherReady(); // Make sure we're ready to shoot
		// TODO Intake star
		driveEncoderPointTurn(100,!position); // Turn to the middle (ENCODER VALUE NOT SET)
		launch(); // Launch picked up star
		breakpoint();
		/*
		// Second star
		*/
		driveEncoderNormal(100, false); // Align with second star
		breakpoint();
		driveEncoderPointTurn(100,position); // Turn to second star (ENCODER VALUE NOT SET)
		breakpoint();
		driveEncoderNormal(100, true); // Drive to second star (ENCODER VALUE NOT SET)
		breakpoint();
		waitForLauncherReady(); // Make sure we're ready to shoot
		// TODO Intake star
		driveEncoderPointTurn(100,!position); // Turn to the middle (ENCODER VALUE NOT SET)
		launch(); // Launch picked up star
		breakpoint();
		/*
		// Third star
		*/
		driveEncoderNormal(100, false); // Align with second star
		breakpoint();
		driveEncoderPointTurn(100,position); // Turn to second star (ENCODER VALUE NOT SET)
		breakpoint();
		driveEncoderNormal(100, true); // Drive to second star (ENCODER VALUE NOT SET)
		breakpoint();
		waitForLauncherReady(); // Make sure we're ready to shoot
		// TODO Intake star
		driveEncoderPointTurn(100,!position); // Turn to the middle (ENCODER VALUE NOT SET)
		launch(); // Launch picked up star
		breakpoint();

		// TODO If drive is ever powerful enough, push cube
	}

	// Mode 3 [Launch + Push Cube]
	if (currentMode == 3) {
		// TODO Deploy intake
		launch(); // Launch preload

		// TODO If drive is ever powerful enough, push cube
	}

	// Mode 4 [Launch in place]
	if (currentMode == 4) {
		// TODO Deploy intake
		launch(); // Launch preload
	}

	// Mode 5 [Launch + Corner]
	if (currentMode == 5) {
		// TODO Deploy intake
		launch(); // Launch preload

		/*
		// Preload
		*/
		// Drive away from wall to line up with star, turn intake towards star, then drive to it
		driveEncoderNormal(100, false); // Drive away from wall (ENCODER VALUE NOT SET)
		breakpoint();
		/*
		// Corner star
		*/
		driveEncoderPointTurn(100,!position); // Turn to the middle (ENCODER VALUE NOT SET)
		breakpoint();
		driveEncoderNormal(100, true); // Drive to first star (ENCODER VALUE NOT SET)
		breakpoint();
		waitForLauncherReady(); // Make sure we're ready to shoot
		// TODO Intake star
		driveEncoderPointTurn(100,position); // Turn to the middle (ENCODER VALUE NOT SET)
		launch(); // Launch picked up star
		breakpoint();
	}

	// Mode 6 [Launch + Corner + Cube]
	if (currentMode == 6) {
		// TODO Deploy intake
		launch(); // Launch preload

		/*
		// Preload
		*/
		// Drive away from wall to line up with star, turn intake towards star, then drive to it
		driveEncoderNormal(100, false); // Drive away from wall (ENCODER VALUE NOT SET)
		breakpoint();
		/*
		// Corner star
		*/
		driveEncoderPointTurn(100,!position); // Turn to the middle (ENCODER VALUE NOT SET)
		breakpoint();
		driveEncoderNormal(100, true); // Drive to first star (ENCODER VALUE NOT SET)
		breakpoint();
		waitForLauncherReady(); // Make sure we're ready to shoot
		// TODO Intake star
		driveEncoderPointTurn(100,position); // Turn to the middle (ENCODER VALUE NOT SET)
		launch(); // Launch picked up star
		breakpoint();

		// TODO If drive is ever powerful enough, push cube
	}

	// Mode 7 [Unnamed atm]
	if (currentMode == 7) {
		clearDriveEncoders();
		//313.6 Ticks per revolution for the wheels
		//0.04inches per tick
		// ticks to travel = distance/distance per tick
		breakpoint();
		for(int i = 0; i < 4; i ++){
			launch();
			breakpoint();
		}
	}

	// Mode 8 [Unnamed atm]
	if (currentMode == 8) {
		startTask(logTicks);
		while(true) wait1Msec(25);
	}
}
