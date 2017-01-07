bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 5;
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

// This probably could be so much better, and it probably doesn't work, but I wrote
// it at 2:30 AM and I was tired -Michael
void turnToAngle(int degrees, int speed) {
	degrees = tempGyroFix(degrees);
	int currentGyroVal = tempGyroFix(SensorValue[gyroMain]);
	if (speed < 0) {
		writeDebugStreamLine("turning right");
		writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
		if(degrees > currentGyroVal) {
			while(degrees > currentGyroVal) {
				driveRaw(speed, speed, speed*-1, speed*-1);
				wait1Msec(20);
				currentGyroVal = tempGyroFix(SensorValue[gyroMain]);
				writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
			}
		} else {
			int lastDegrees = currentGyroVal;
			while(lastDegrees <= currentGyroVal) {
				driveRaw(speed, speed, speed*-1, speed*-1);
				lastDegrees = currentGyroVal;
				wait1Msec(20);
				currentGyroVal = tempGyroFix(SensorValue[gyroMain]);
				writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
			}
			writeDebugStreamLine("TRANSITION");
			while(degrees > currentGyroVal) {
				driveRaw(speed, speed, speed*-1, speed*-1);
				wait1Msec(20);
				currentGyroVal = tempGyroFix(SensorValue[gyroMain]);
				writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
			}
		}
		driveRaw(25,25,-25,-25);
		wait1Msec(250);
	} else {
		writeDebugStreamLine("turning left");
		writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
		if(degrees < currentGyroVal) {
			while(degrees < currentGyroVal) {
				driveRaw(speed,speed, speed*-1, speed*-1);
				wait1Msec(20);
				currentGyroVal = tempGyroFix(SensorValue[gyroMain]);
				writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
				writeDebugStreamLine("Exact:%i",SensorValue[gyroMain]);
			}
		} else {
		int lastDegrees = currentGyroVal;
				while(lastDegrees >= currentGyroVal) {
					driveRaw(speed,speed, speed*-1, speed*-1);
					lastDegrees = currentGyroVal;
					wait1Msec(20);
					currentGyroVal = tempGyroFix(SensorValue[gyroMain]);
					writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
					writeDebugStreamLine("Exact:%i",SensorValue[gyroMain]);
				}
				writeDebugStreamLine("TRANSITION");
				while(degrees < currentGyroVal) {
					driveRaw(speed,speed,speed*-1,speed*-1);
					wait1Msec(20);
					currentGyroVal = tempGyroFix(SensorValue[gyroMain]);
					writeDebugStreamLine("Deg:%i CurGy: %i",degrees,currentGyroVal);
					writeDebugStreamLine("Exact:%i",SensorValue[gyroMain]);
				}
		}
		driveRaw(-25,-25,25,25);
		wait1Msec(250);
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
		turnToAngle(2700, 60);
		wait1Msec(500);
		turnToAngle(450, -60);
	}

	// Mode 3 [Launch + Sit still]
	if (currentMode == 3) {

	}

	// Mode 4 [Robot Skills]
	if (currentMode == 4) {

	}

	if (currentMode == 5) {
		pidRequestedValue = 800;

		// start the PID task
		startTask( drivePID );


	}
}
