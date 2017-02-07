bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 10;
int currentMode = minAutonomous;

void waitForLiftOld(int speed);
void moveLiftWithLogic(int speed, bool overrideHold, bool dampenSpeed, bool overrideBounds);
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
	const int misal = 15;
	do { // Align the claws
		const int misal = 15;
		int localClawSpeed = clawRunning ? clawSpeed : 30;
		int clawPosLeft = getMotorEncoder(clawL);
		int clawPosRight = getMotorEncoder(clawR);
		//clawPosRight *= -1; // so they both count up as they go out
		clawPosRight *= -1;
		// If we're misaligning, counteract
		// Left claw
		if (abs(clawPosLeft-clawTargetLeft) > misal) {
			motor[clawL] = clawPosLeft-clawTargetLeft > 0 ? localClawSpeed : localClawSpeed * -1;
			} else {
			motor[clawL] = 0;
		}
		// Right claw
		if (abs(clawPosRight-clawTargetRight) > misal) {
			motor[clawR] = clawPosRight-clawTargetRight > 0 ? -1 * localClawSpeed : localClawSpeed;
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

void setClawSpeed(int speed) {
	clawSpeed = speed;
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
	setClawSpeed(30);
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

int liftTarget;
bool liftRunning = false;
int liftSpeed;
bool holdUpLift = false;

task adjustLiftPosition() {
	liftRunning = true;
	int liftPos;
	const int misal = 25;
	do { // align the lift with the target
		liftPos = SensorValue[potArm];
		int localLiftSpeed = liftRunning ? liftSpeed : 0;
		if(abs(liftPos-liftTarget) > misal) {
			moveLiftWithLogic(((liftPos-liftTarget > 0) ? localLiftSpeed * -1 : localLiftSpeed), true, true, false);
			} else {
			setLiftMotors(holdUpLift ? 30 : 0);
		}
		wait1Msec(20);
		if(liftRunning)
			liftRunning = abs(liftPos-liftTarget) > misal;
	} while (true);
}

void setLift(int target, int speed) {
	liftRunning = true;
	liftSpeed = speed;
	liftTarget = target;
}

// Wait for the lift to reach it's target
void waitForLift() {
	while (liftRunning)
		wait1Msec(20);
}

void stopLift() {
	setLiftMotors(0);
}

void stopLiftTask(){
	stopTask(	adjustLiftPosition );
}

void startLiftTask() {
	startTask( adjustLiftPosition );
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

// Auton modes should handle team color and positon
// (Thanks vex for making the pole opposite for each side)
void runAuton() {
	string debug = "";
	sprintf(debug,"Running Auton Mode: %i",currentMode);
	writeDebugStreamLine(debug);
	int sideMult = getAutonPosition() ? 1 : -1; // 1 for right, -1 for left
	stopTask(manageClaw);
	stopSlew();
	stopClaw();
	stopLift();
	startClawTask();
	startLiftTask();
	startSlewTask();
	// Mode 1 [Default, Do nothing] ---------------------------------------------

	// Mode 2 [Max Points] ------------------------------------------------------
	if (currentMode == 2)  {
		pidDriveStraight(-1400); // Drive to cube
		waitForPID(); // Wait for claw
		pidDrivePoint(100 * sideMult); // Turn to cube
		waitForPID(); // Wait for turn
		setClaw(300,127); // Deploy claw
		waitForClaw(); // Wait for claw
		clawClose(500,127); // Close claw
		setLift(1600,127); // Lift
		pidDrivePoint(-120 * sideMult); // Turn to wall
		waitForLift(); // Wait for lift
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
		waitForPID(); // Wait for drive

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
		waitForLiftOld(100);
		setClaw(500,-127);
		waitForClaw();
		breakpoint(); // TODO: REMOVE BEFORE MATCH
		waitForLiftOld(-100);
		waitForPid();
		waitForClaw();*/
	}

	// Mode 3 [Compat] ----------------------------------------------------------
	if (currentMode == 3) {
		wait1Msec(5000);
		driveForTime(70,70,70,70,+750); // Drive at stuff
		wait1Msec(100);
		driveForTime(-70,-70,-70,-70,500); // Back up
		wait1Msec(100);
		setClaw(300,127);
		waitForClaw();
		wait1Msec(100);
		driveRaw(127,127,127,127);
		wait1Msec(1250);
		driveRaw(0,0,0,0);
	}

	// Mode 4 [C Compat] --------------------------------------------------------
	if (currentMode == 4) {
		driveRaw(127,127,127,127);
		wait1Msec(2750);
		driveRaw(0,0,0,0);
	}

	// Mode 5 [Skills] ----------------------------------------------------------
	if (currentMode == 5) {
		clearDriveEncoders();
		moveLiftWithLogic(0,false,false,false);
		driveForTime(-70,-70,-70,-70,750); // Drive at fence
		wait1Msec(100);
		setClaw(300,127); // Open the claw
		waitForClaw();
		wait1Msec(2000);
		driveForTime(70,70,70,70,750); // Back up
		clawClose(500,127); // Close the claw
		driveForTime(-70,-70,-70,-70,1250); // Drive at fence
		wait1Msec(100);
		setLift(1900,127);
		waitForLift();
		setClaw(300,127);
		waitForClaw();
		for(int i = 0; i < 2; i ++) {
			setLift(600,127);
			waitForLift();
			wait1Msec(1000);
			driveForTime(70,70,70,70,900); // Back up
			wait1Msec(100);
			clawClose(500,127); // Close the claw
			driveForTime(-70,-70,-70,-70,1250); // Drive at fence
			wait1Msec(100);
			setLift(1900,127);
			waitForLift();
			setClaw(300,127);
			waitForClaw();
		}
		wait1Msec(100);
		breakpoint(); //////////
		/*driveForTime(70,70,70,70,250); // Back up
		turnToAngle(450,-127);
		breakpoint();
		driveForTime(70,70,70,70,1000);
		clawClose(500,127);
		breakpoint();
		setLift(1600,127);
		turnToAngle(0,127);
		breakpoint();
		driveForTime(-70,-70,-70,-70,250);
		waitForLift();
		setLift(1900,127);
		waitForLift();
		setClaw(300,127);
		waitForClaw();
		breakpoint();
		driveForTime(70,70,70,70,250);*/
		pidDriveStraight(-3000); // Drive to fence
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		setClaw(200,127); // Unnamed action 15`
		waitForClaw(); // Wait for claw
		breakpoint(); /////////////////////////////////
		setLift(600,127); // Lower lift
		setClaw(250,127); // Reset claw
		waitForLift(); // Wait for lift
		waitForClaw(); // Wait for claw
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		pidDriveStraight(500); // Back up
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		turnToAngle(-900 * sideMult, 127 * sideMult); // Turn to stars
		breakpoint(); /////////////////////////////////
		pidDriveStraight(2500); // Back towards stars
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		clawClose(500,127); // Close claw
		setLift(700,127); // Hover lift
		waitForLift(); // Wait for lift
		setLift(700,30); // Lock lift
		breakpoint(); /////////////////////////////////
		pidDrivePoint(-469 * sideMult); // Turn to wall
	}

	// Mode 6 [PID Tuning] -----------------------------------------------------
	if(currentMode == 6) {
		pidDriveStraight(-1150); // Drive to cube
		waitForPID(); // Wait for drive
		turnToAngle(-600 * sideMult ,127 * sideMult); // Turn to cube
		waitForPID(); // Wait for turn
		setClaw(300,127); // Deploy claw
		waitForClaw(); // Wait for claw
		clawClose(500,127); // Close claw
		setLift(1900,127); // Lift
		turnToAngle(-300 * sideMult, -127 * sideMult) // Turn to fence
		waitForLift(); // Wait for lift
		setClaw(300,127); // Open claw
		waitForPID(); // Wait for turn
		waitForClaw(); // Wait for claw
		setLift(600,127); // Lower Lift
		setClaw(360,127); // Widen claw
		pidDriveStraight(700); // Drive to stars
		waitForLift(); // Wait for lift
		waitForClaw(); // Wait for claw
		waitForPID(); // Wait for drive
		clawClose(750,127); // Close claw
		pidDriveStraight(-700); // Drive to fence
		wait1Msec(1000);
		setLift(1600,127); // Raise lift
		waitForPID(); // Wait for drive
		setLift(1900,127); // Finish lift movement
		waitForLift(); // Wait for lift
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
		waitForLift(); // Wait for lift
		waitForPID(); // Wait for drive
	}

	if(currentMode == 7) {
		pidDriveStraight(-700); // Drive forward
		waitForPID(); // Wait for drive
		//pidDrivePoint(-168); // Turn to star
		turnToAngle(200 * sideMult, -127 * sideMult); // Turn to star
		waitForPID(); // Wait for drive
		setClaw(250,127); // Deploy claw
		waitForClaw(); // Wait for claw
		pidDriveStraight(300); // Back towards star
		waitForPID(); // Wait for drive
		wait1Msec(500);
		clawClose(500,127); // Close claw
		wait1Msec(500);
		pidDriveStraight(-300); // Drive forward
		waitForPID(); // Wait for drive
		wait1Msec(500);
		turnToAngle(-100 * sideMult, 127 * sideMult); // Turn to fence
		waitForPID(); // Wait for drive
		// TODO Uncomment -----------------------------------------------------------------------------------------------------
		//pidDriveStraight(-700); // Drive to fence
		//waitForPID(); //  for drive
		//setLift(1900,127); // Lift
		//waitForLift(); // Wait for lift
		//setClaw(200,127); // Open claw
		//waitForClaw(); // Wait for claw
	}

	if(currentMode == 8) {
		pidDriveStraight(-1400); // Back up
		waitForPID(); // Wait for drive
		pidDrivePoint(-250 * sideMult); // Turn to the stars
		waitForPID(); // Wait for drive
		setClaw(250,127); // Deploy claw
		waitForClaw(); // Wait for claw
		wait1Msec(200);
		driveRaw(100,100,100,100);
		wait1Msec(400);
		driveRaw(0,0,0,0);
		clawClose(500,127); // Unnamed action 9
		setLift(750,90); // Hover lift
		waitForLift(); // Wait for lift
		setLift(750,30); // Stall lift
		turnToAngle(100 * sideMult, -127 * sideMult);
		pidDriveStraight(2000);
		waitForPid();
		setLift(1900,127);
		waitForLift();
		setClaw(300,127);
		waitForClaw();
	}

	if(currentMode == 9) {
		pidDriveStraight(-1560); // Drive forward
		waitForPID(); // Wait for drive
		setClaw(300,127); // Open claw
		breakpoint(); ///////////////////////////////
		waitForClaw(); // Wait for claw
		pidDriveStraight(450); // Drive to stars
		wait1MSec(200); // Delay claw close
		clawClose(500,127); // Close claw
		waitForPID(); // Wait for drive
		breakpoint(); ///////////////////////////////
		pidDriveStraight(-1550); // Drive to fence
		waitForPID(); // Wait for drive
		setLift(1900,127); // Lift stars
		waitForLift(); // Wait for lift
		breakpoint(); /////////////////////////////////
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
		setLift(600,127); // Lower Lift
		waitForLift(); // Wait for lift
		breakpoint(); /////////////////////////////////
		turnToAngle(-900 * sideMult, 100 * sideMult); // Turn to wall
		setClaw(200,127); // Narrow claw
		pidDriveStraight(4800); // Back towards star group
		waitForPID(); // Wait for drive
		clawClose(500,127); // Close claw
		breakpoint(); /////////////////////////////////
		setLift(1500,127); // Lift stars
		turnToAngle(0 * sideMult, -127 * sideMult); // Turn to fence
		setLift(1900,127); // Finish lift motion
		waitForLift(); // Wait for lift
		breakpoint(); /////////////////////////////////
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
		setLift(600,127); // Lower lift
		turnToAngle(120 * sideMult, -100 * sideMult); // Turn to cube
		breakpoint(); /////////////////////////////////
		pidDriveStraight(1700); // Drive to cube
		clawClose(500,127); // Close claw
		setLift(1500,127); // Lift cube
		turnToAngle(0 * sideMult, 0 * sideMult); // Turn to fence
		breakpoint(); /////////////////////////////////
		pidDriveStraight(-1640); // Drive to fence
		waitForLift(); // Wait for lift
		waitForPID(); // Wait for drive
		setLift(1900,127); // Finish lift movement
		setClaw(250,127); // Open claw
		waitForClaw(); // Wait for claw
		breakpoint(); /////////////////////////////////
		setLift(600,127); // Lower lift
		pidDriveStraight(2400); // Drive to wall
		waitForLift(); // Wait for lift
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		turnToAngle(900 * sideMult, -127 * sideMult); // Turn to stars
		pidDriveStraight(5000); // Drive through stars
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		clawClose(750,0); // Close claw
		wait1MSec(500); // Hold claw
		turnToAngle(550 * sideMult, 127 * sideMult); // Turn to corner
		breakpoint(); /////////////////////////////////
		pidDriveStraight(-640); // Back up
		waitForPID(); // Wait for drive
		turnToAngle(0 * sideMult, 0 * sideMult); // Turn to fence
		breakpoint(); /////////////////////////////////
		pidDriveStraight(-2050); // Drive to fence
		waitForPID(); // Wait for drive
		setLift(1900,127); // Lift stars
		waitForLift(); // Wait for lift
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
		breakpoint(); /////////////////////////////////
		setLift(600,127); // Lower lift
		pidDriveStraight(1700); // Drive to start
		waitForLift(); // Wait for lift
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		clawClose(500,0); // Close claw
		pidDriveStraight(-1700); // Drive to fence
		waitForPID(); // Wait for drive
		setLift(1900,127); // Lift cube
		waitForLift(); // Wait for lift
		breakpoint(); /////////////////////////////////
		pidDriveStraight(0); // Open claw
		waitForClaw(); // Wait for claw
		setLift(600,127); // Lower lift
		breakpoint(); /////////////////////////////////
		pidDriveStraight(1700); // Drive to cube
		waitForLift(); // Wait for lift
		waitForPID(); // Wait for drive
		clawClose(500,0); // Close claw
		breakpoint(); /////////////////////////////////
		setLift(1200,127); // Partially lift cube
		turnToAngle(350 * sideMult, 0 * sideMult); // Turn to center
		breakpoint(); /////////////////////////////////
		pidDriveStraight(-1800); // Drive to center
		waitForLift(); // Wait for lift
		waitForPID(); // Wait for drive
		breakpoint(); /////////////////////////////////
		setLift(1900,127); // Lift cube
		waitForLift(); // Wait for lift
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
		breakpoint(); /////////////////////////////////
		setLift(1400,95); // Lower lift (slightly)
		turnToAngle(-450 * sideMult, 0 * sideMult); // Turn to center cube
		breakpoint(); /////////////////////////////////
		setLift(600,127); // Lower lift (fully)
		waitForLift(); // Wait for lift
		clawClose(500,0); // Close claw
		setLift(1500,127); // Raise lift
		breakpoint(); /////////////////////////////////
		wait1MSec(200); // Wait for some lift movement
		turnToAngle(0 * sideMult, -127 * sideMult); // Turn to fence
		breakpoint(); /////////////////////////////////
		setLift(1900,127); // Lift cube over
		waitForLift(); // Wait for lift
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
	}

	if(currentMode == 10) {
		pidDriveStraight(-2200); // Drive to cube
		waitForPID(); // Wait for drive
		pidDrivePoint(500 * sideMult); // Turn to cube
		waitForPid(); // Wait for drive
		wait1Msec(600); // Delay Deploy
		setClaw(300,127); // Deploy claw
		waitForClaw(); // Wait for claw
		wait1MSec(100); // Delay close claw
		clawClose(500,127); // Close claw
		setLift(1000,127); // Hover lift
		waitForLift(); // Wait for lift
		turnToAngle(-350 * sideMult, -127 * sideMult); // Turn to fence
		setLift(1900,127); // Lift cube
		waitForLift(); // Wait for lift
		setClaw(300,127); // Open claw
		waitForClaw(); // Wait for claw
		breakpoint(); //////////////////////////
		setLift(1000,127); // Lower lift
		wait1MSec(200); // Delay turn
		pidDrivePoint(234 * sideMult); // Turn to stars... sort  of
		waitForPid(); // Wait for drive
		/*breakpoint(); //////////////////////////
		waitForLift(); // Wait for lift
		pidDriveStraight(1400); // Back up
		breakpoint(); //////////////////////////
		pidDrivePoint(-234 * sideMult); // Turn to stars
		waitForPID(); // Wait for drive
		*/
		breakpoint(); //////////////////////////
		driveForTime(127,127,127,127,500); // Backup
		waitForPID(); // Wait for drive
		clawClose(500,127); // Close claw
		breakpoint(); //////////////////////////
		pidDriveStraight(-1500); // Drive to fence
		waitForPID(); // Wait for drive
		setLift(1900,127); // Raise lift
		waitForLift(); // Wait for lift
		setClaw(500,127); // Open claw (wide)
		breakpoint(); //////////////////////////
		waitForClaw(); // Wait for claw
		waitForLift(); // Wait for lift
		waitForPID(); // Wait for drive
	}
	stopClawTask();
	stopSlewTask();
	startTask(manageClaw);

}
