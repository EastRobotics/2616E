bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 6;
int currentMode = minAutonomous;

void waitForLift(int speed); //
void setLiftMotors(int speed);
void lockLift();

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

//Open claw based on... time *SHUDDER*
void tempOpenClaw(bool lock) {
	//Open Claw
	motor[clawL] = -127;
	motor[clawR] = 127;
	wait1Msec(500);
	motor[clawL] = (lock ? 30 : 0);
	motor[clawR] = (lock ? -30 : 0);
	wait1Msec(100);
}

//Close claw based on... time *SHUDDER*
void tempCloseClaw(bool lock) {
	//Close Claw
	motor[clawL] = 127;
	motor[clawR] = -127;
	wait1Msec(500);
	motor[clawL] = (lock ? 30 : 0);
	motor[clawR] = (lock ? -30 : 0);
	wait1Msec(100);
}

void moveLiftWithTime(int speed, int time) {
	time = abs(time);
	setLiftMotors(speed);
	wait1Msec(time);
	setLiftMotors(0);
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
		driveForTime(70,70,70,70,1000); // Back towards
		motor[clawL] = 127;
		motor[clawR] = -127;
		wait1Msec(500);
		motor[clawL] = 30;
		motor[clawR] = -30;
		wait1Msec(100);
		//waitForLift(100);
		moveLiftWithTime(100,700);
		lockLift();
		setLiftMotors(30);
		wait1Msec(500);
		turnToAngle(((sideMult < 0) ? -1350 : 1350),-100*sideMult);
		wait1Msec(250);
		driveForTime(-70,-70,-70,-70,1250); // Drive at stuff
		wait1Msec(100);
		moveLiftWithTime(127,1000);
		lockLift();
		setLiftMotors(30);
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

	// Mode 5 [Skills] ----------------------------------------------------------
	if (currentMode == 5) {
		clearDriveEncoders();
		moveLiftWithLogic(0,false,false,false);
		driveForTime(-70,-70,-70,-70,750); // Drive at fence
		wait1Msec(100);
		tempOpenClaw(false); // Open the claw
		wait1Msec(2000);
		driveForTime(70,70,70,70,750); // Back up
		tempCloseClaw(true); // Close the claw
		driveForTime(-70,-70,-70,-70,1250); // Drive at fence
		wait1Msec(100);
		waitForLift(100);
		tempOpenClaw(false);
		for(int i = 0; i < 2; i ++) {
			waitForLift(-80);
			wait1Msec(1000);
			driveForTime(70,70,70,70,750); // Back up
			wait1Msec(100);
			tempCloseClaw(true); // Close the claw
			driveForTime(-70,-70,-70,-70,1250); // Drive at fence
			wait1Msec(100);
			waitForLift(100);
			tempOpenClaw(false);
		}
		wait1Msec(100);
		driveForTime(70,70,70,70,250); // Back up
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		turnToAngle(((sideMult < 0) ? 900 : -900),80*sideMult);
		wait1Msec(250);
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		waitForLift(-80);
		wait1Msec(100);
		driveForTime(-70,-70,-70,-70,750); // Drive at cube
		wait1Msec(100);
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		tempCloseClaw(true);
		wait1Msec(100);
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		turnToAngle(0,-80*sideMult);
		wait1Msec(250);
		breakpoint();
		driveForTime(-70,-70,-70,-70,1250); // Drive at fence
		tempOpenClaw(false);
	}
	stopClawTask();
	startTask(manageClaw);
}
