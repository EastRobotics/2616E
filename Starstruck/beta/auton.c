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
	motor[launcherLI] = motor[launcherLO] = motor[launcherRI] = motor[launcherRO] = -127;
	wait1Msec(3650);
	motor[launcherLI] = motor[launcherLO] = motor[launcherRI] = motor[launcherRO] = 0;
}

//wait until button press to continue
void breakpoint() {
	while(!SensorValue[autonCont]){
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
		breakpoint();
		for(int i = 0; i < 4; i ++){
			shoot();
			breakpoint();
		}

	}
	if (currentMode == 3) { // Mode 3
		// TODO: Stuff
		//DEBUG FOR NOW
		string debugLine = "";
		sprintf(debugLine,"nMotorEncoder[1]--%i",nMotorEncoder[1]);
		writeDebugStreamLine(debugLine);
		debugLine = "";
		sprintf(debugLine,"driveFL = %i",driveFL);
		writeDebugStreamLine(debugLine);
		setupMotorTicks(driveFL,1000);
		startTask( driveMotorToTargetPID, kHighPriority );
	}
}
