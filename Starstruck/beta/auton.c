bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 3;
int currentMode = minAutonomous;
int ticks = 250;
int intakeLoadTime = 1000;

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

//Fire the launcher
void shoot() {

}

//wait until button press to continue
void breakpoint() {
	while(!vexRT[Btn8D]){
		wait1Msec(10);
	}
}

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

// Auton modes should handle team color and positon
// (Thanks vex for making the pole opposite for each side)
void runAuton() {
	// Mode 1 is do nothing
	if (currentMode == 2) { // Mode 2
		clearDriveEncoders();
		//313.6 Ticks per revolution for the wheels
		//0.04inches per tick
		// ticks to travel = distance/distance per tick
		//Shoot a star
		shoot();
		//Load and shoot 3 stars
		for(int i = 0; i < 3; i ++){
			startIntake();
			shoot();
		}
		//Pivot turn right 90 degrees
		drivePointTurnPID(ticks);
		driveRaw(0,0,0,0);
		//Load the last star ... asynchronously
		startTask(intake);
		//Drive forward for half a second
		drive(60,0,0);
		wait1Msec(500);
		driveRaw(0,0,0,0);
		//wait for other star to finish loading
		if((intakeLoadTime-497)>0){
			wait1Msec(intakeLoadTime-497);
		}
		//Make star go half of the way up intake
		intakeLoadTime/=2;
		//Grab star
		startTask(intake);
		//Wait for intake and star to get to know eachother
		wait1Msec(round(intakeLoadTime/2));
		//Time to go
		drive(-60,0,0);
		wait1Msec(500);
		driveRaw(0,0,0,0);
		//Pivot turn left 90 degrees
		drivePointTurnPID(ticks);
		//wait for star to finish loading
		if(((intakeLoadTime/2)-500)>0){
			wait1Msec((intakeLoadTime/2)-500);
		}
		//fire star
		shoot();
		//Finish loading other star
		startIntake();
		//reset intake load time
		intakeLoadTime*=2;
		//Fire
		shoot();
	}
	if (currentMode == 3) { // Mode 3
		// TODO: Stuff
	}
}
