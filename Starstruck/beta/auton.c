bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 4;
int currentMode = minAutonomous;
int intakeLoadTime = 800;

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

//Run the intake for one second and reset
task intake() {
	motor[intakeL] = -127;
	motor[intakeR] = 127;
	wait1Msec(intakeLoadTime);
	motor[intakeL] = motor[intakeR] = 0;
	wait1Msec(300);
	//reset the intake
	motor[intakeL] = 127;
	motor[intakeR] = -127;
	wait1Msec(intakeLoadTime);
	motor[intakeL] = motor[intakeR] = 0;
}

//synchronous version of startTask(intake);
void startIntake() {
	startTask( intake );
	wait1Msec(intakeLoadTime+300);
}

// Blocking method to wait for the launcher when we're in position
void waitForLauncherReady() {
	while (!getCanLaunch()) {
		wait1Msec(5);
	}
}

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
	int speedRight = (speed < 0) ? speed*-1 : speed;
	int speedLeft = (speed < 0) ? speed : speed*-1;
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

/*task logTicks() {
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
*/

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
	// Mode 1 [Do nothing]

	// Mode 2 [Default mode/Max Points]
	if (currentMode == 2) {
		motor[intakeL] = 127;
		motor[intakeR] = -127;
		wait1Msec(500);
		launch();
		//driveForTime(-80,-80,-80,-80,400); //backwards
		resetMotorEncoder(driveFL);
		tempEncoderForward(-60,300);
		wait1Msec(500);
		driveForTime(80*sideMult,80*sideMult,-80*sideMult,-80*sideMult,500);//turn left towards star
		wait1Msec(500);
		driveForTime(80,80,80,80,400);//forwards
		driveForTime(40,40,40,40,200);//coast slowly to star
		wait1Msec(500);
		waitForLauncherReady();
		startIntake(); //pick up star
		wait1Msec(300);
		driveForTime(-80*sideMult,-80*sideMult,80*sideMult,80*sideMult,500);//turn right towards fence
		wait1Msec(500);
		launch();
		waitForLauncherReady();
	}

	// Mode 3 [Launch + Sit still]
	if (currentMode == 3) {
		motor[intakeL] = 127;
		motor[intakeR] = -127;
		wait1Msec(500);
		launch();
		waitForLauncherReady();
	}

	// Mode 4 [Robot Skills]
	if (currentMode == 4) {
		motor[intakeL] = 127;
		motor[intakeR] = -127;
		wait1Msec(500);
		launch();
		// Launch gameloads
		for (int i=0; i<3; i++) {
			waitForLauncherReady();
			wait1Msec(5000);
			launch();
			wait1Msec(200);
		}

		// From here on it's just mode 2
		//driveForTime(-80,-80,-80,-80,400); //backwards
		resetMotorEncoder(driveFL);
		tempEncoderForward(-60,300);
		wait1Msec(500);
		driveForTime(80*sideMult,80*sideMult,-80*sideMult,-80*sideMult,500);//turn left towards star
		wait1Msec(500);
		driveForTime(80,80,80,80,400);//forwards
		driveForTime(40,40,40,40,200);//coast slowly to star
		wait1Msec(500);
		waitForLauncherReady();
		startIntake(); //pick up star
		wait1Msec(300);
		driveForTime(-80*sideMult,-80*sideMult,80*sideMult,80*sideMult,500);//turn right towards fence
		wait1Msec(500);
		launch();
		waitForLauncherReady();
	}
}
