#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    sensorFrontRight, sensorLineFollower)
#pragma config(Sensor, in2,    sensorFrontLeft, sensorLineFollower)
#pragma config(Sensor, in3,    leftLineSensor, sensorLineFollower)
#pragma config(Sensor, in4,    rightLineSensor, sensorLineFollower)
#pragma config(Sensor, in5,    middleLineSensor, sensorLineFollower)
#pragma config(Sensor, in7,    armPot,         sensorPotentiometer)
#pragma config(Sensor, in8,    powerExpander,  sensorNone)
#pragma config(Sensor, dgtl1,  barLimit,       sensorDigitalIn)
#pragma config(Sensor, dgtl2,  ledCanFire,     sensorDigitalOut)
#pragma config(Sensor, dgtl3,  ledCannotFire,  sensorDigitalOut)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           motorBar,      tmotorVex393_HBridge, openLoop, driveLeft, encoderPort, I2C_1)
#pragma config(Motor,  port2,           motorFrontLeft, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           motorBackLeft, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           motorFrontRight, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           motorBackRight, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           motorLauncherOne, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           motorLauncherTwo, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           motorLauncherThree, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           motorLauncherFour, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          motorIntake,   tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   // Main competition background code
#include "bar.c" // Launcher bar control
#include "sensors.c" // Sensor methods
#include "autonomous.c" // Autonomous handler
#include "lcd.c" // LCD methods
#include "controller.c" // Controller methods

int firePosition;
bool isFiring;
short lineL,lineR,lineM,lineFL,lineFR;
bool detectedLeft,detectedRight = false;
int lastTurn = -1;
int moving = -1;

bool detectedL() {
	return detectedLeft;
}

bool detectedR() {
	return detectedRight;
}

int movingWhere() {
	return moving;
}

bool canFire() {
	return (!isFiring && isBarReady());
}

// ##################
// # Pre Autonomous #
// ##################

void pre_auton()
{
	bLCDBacklight = true;
	resetBar();
	//startTask (	barHookPos ); // TODO Uncomment

	// Goodbye, errors
	if (false) {
		UserControlCodePlaceholderForTesting();
		AutonomousCodePlaceholderForTesting();
		startTask( countHoldTime );
	}
}

// ##############
// # Autonomous #
// ##############

task autonomous()
{
	runAutonomous();
}

// Should really be 'isLine' in final production
bool isWhite(int value){
	// return value<850; // For light line on dark surface
	return value>1200; // For dark line on light surface
}

void drive(int forwardDirection, int strafeDirection, int turnDirection) {
	motor[motorFrontLeft] = forwardDirection + turnDirection + strafeDirection;
	motor[motorBackLeft] =  forwardDirection + turnDirection - strafeDirection;
	motor[motorFrontRight] = forwardDirection - turnDirection - strafeDirection;
	motor[motorBackRight] =  forwardDirection - turnDirection + strafeDirection;
}

task lineFollow()
{
	int lastDirection = -1;
	detectedLeft = detectedRight = false;
	bool foundLine = true;
	bool droppedLine = false;
	while(true){
		if (foundLine && droppedLine) {
			detectedLeft = false;
			detectedRight = false;
		}
		if(isWhite(lineFL)){
			detectedLeft = true;
		}
		if(isWhite(lineFR)){
			detectedRight = true;
		}
		lineL = SensorValue[leftLineSensor];
		lineR = SensorValue[rightLineSensor];
		lineM = SensorValue[middleLineSensor];
		lineFL = SensorValue[sensorFrontLeft];
		lineFR = SensorValue[sensorFrontRight];
		string lineValue;

		// Console output
		writeDebugStreamLine("--------------- Line update ---------------");
		string lineString = "Front Left: ";
		long line = lineFL;
		sprintf(lineString, "%i", line); strcat(lineString, "(");
		sprintf(lineString, "%d", isWhite(line)); strcat(lineString,")");
		writeDebugStreamLine(lineString);
		lineString = "Front right: ";
		line = lineFR;
		sprintf(lineString, "%i", line); strcat(lineString, "(");
		sprintf(lineString, "%d", isWhite(line)); strcat(lineString,")");
		writeDebugStreamLine(lineString);

		lineString = "Left: ";
		line = lineL;
		sprintf(lineString, "%i", line); strcat(lineString, "(");
		sprintf(lineString, "%d", isWhite(line)); strcat(lineString,")");
		writeDebugStreamLine(lineString);
		lineString = "Center: ";
		line = lineM;
		sprintf(lineString, "%i", line); strcat(lineString, "(");
		sprintf(lineString, "%d", isWhite(line)); strcat(lineString,")");
		writeDebugStreamLine(lineString);
		lineString = "Right: ";
		line = lineR;
		sprintf(lineString, "%i", line); strcat(lineString, "(");
		sprintf(lineString, "%d", isWhite(line)); strcat(lineString,")");
		writeDebugStreamLine(lineString);
		// END Console output

		// RIGHT sensor sees line:
		if(isWhite(lineR))
		{
			drive(25,0,50);
			// counter-steer right:
			//motor[motorFrontLeft]  = motor[motorBackLeft] = 50;
			//motor[motorFrontRight] = motor[motorFrontRight]  = 0;
			lastDirection = lastTurn = 0;
			moving = 0;
			//continue;
		}
		// CENTER sensor sees line:
		if(isWhite(lineM))
		{
			drive(50,0,0);
			if (!(isWhite(lineL) && isWhite(lineR))) { // If not both left and right see line
				if (isWhite(lineL)) {
					// Go slightly right and straight
					drive(50,10,-10);
				}	else
					if (isWhite(lineR)) {
					// Go slightly left and straight
					drive(50,-10,10);
				}
			}
			moving = 1;
			lastDirection = 2;
			//continue;
		}
		// LEFT sensor sees line:
		if(isWhite(lineL))
		{
			drive(25,0,-50);
			// counter-steer left:
			//motor[motorFrontLeft] = motor[motorBackLeft] = 0;
			//motor[motorFrontRight] = motor[motorBackRight] = 50;
			lastDirection = lastTurn = 1;
			moving = 2;
			//continue;
		}
		int strafePower = 0;
		int turnPower = 0;
		int forwardPower = 0;
		if(!(isWhite(lineL)||isWhite(lineM)||isWhite(lineR))){
			foundLine = false;
			droppedLine = true;
			moving = 5;
			//TODO We probably reached the end of the line, handle a turn.
			// At this point, it's really up to the challenge and programmer
			// to decide how to take our action. It's really purpose specific.
			// For our test robot, it just turns in the direction of the line.
			// NOTE: To handle a T- intersection, check if both are true before checking
			// them individually.
			if(detectedLeft){
				turnPower = 70;
				} else if(detectedRight){
				turnPower = -70;
			}
			// We lost the line, find it again
			else if (lastTurn ==1) {
				//RIGHT
				strafePower =	30;
				forwardPower = 10;
				turnPower = -20;
				moving = 3;
				} else if(lastTurn==0){
				//LEFT
				strafePower = -30;
				forwardPower = 10;
				turnPower = 20;
				moving = 4;
				} else {
				// If all else fails, pivot turn right to victory
				turnPower = -20;
				moving = 6+lastDirection;
			}

			drive(forwardPower, strafePower, turnPower);
		} else
			foundLine = true;
	}
}

// ################
// # User Control #
// ################

task usercontrol()
{
	// Hold time currently not used
	//startTask( countHoldTime );
	startTask( lcdRunAutoRefresh );
	startTask( updateBar );
	startTask( lineFollow );
	// Main loop
	bool buttonReleased = true;
	int highestCombination = 0;
	while(true)
	{
		// LED indicators
		if (canFire()) {
			SensorValue[ledCanFire] = 1;
			SensorValue[ledCannotFire] = 0;
		}
		else {
			SensorValue[ledCanFire] = 0;
			SensorValue[ledCannotFire] = 1;
		}
		// LCD Button Handling
		if (nLCDButtons == 0) { // A button wasn't pressed
			if (buttonReleased == false) {
				// Button was pressed then released, calculate what to do
				if (highestCombination == 1) { // Left button pressed
					lcdLastPage();
					buttonReleased = false;
				}
				else
					if (highestCombination == 2) { // Center button pressed
					lcdHome();
					buttonReleased = false;
				}
				else
					if (highestCombination == 4) { // Right button pressed
					lcdNextPage();
					buttonReleased = false;
				}
				else
					if (highestCombination == 3) { // Left & center pressed
					lcdNext();
				}
				else
					if (highestCombination == 6) { // Right & center pressed
					lcdBack();
				}
				buttonReleased = true;
				highestCombination = 0;
			} // Else: Nothing was pressed
		}
		else // A button was pressed
		{
			lcdResetAutoRefresh();
			if (buttonReleased) {
				buttonReleased = false;
				lcdResetHoldTime();
			}

			if (nLCDButtons > highestCombination) {
				highestCombination = nLCDButtons;
			}
		}

		// Controller
		//runCalculation();
	}
}

// ####################
// # Launcher Methods #
// ####################

void setLauncherSpeed( int armSpeed ) {
	motor[motorLauncherOne] = armSpeed;
	motor[motorLauncherTwo] = armSpeed;
	motor[motorLauncherThree] = armSpeed;
	motor[motorLauncherFour] = armSpeed;
}

bool isAboutToFire() { // TODO use potentiometer to determine if we're about to shoot
	return true;
}

task fireTask() {
	// Move bar to launch position
	if (firePosition == 1) { // Long range
		startTask( barLongShot );
	}
	else
		if (firePosition == 2) { // Mid range
		startTask( barMediumShot );
	}
	else
		if (firePosition == 3) { // Long range
		startTask( barCloseShot );
	}
	else
		return; // Invalid position

	// Wait for bar to be in position
	setLauncherSpeed(30);
	while (!isBarReady()) { // Wait til bar is in hook position
		// TODO If we're about to fire, slow motors down
		wait1Msec(25);
	}

	// Move arm and intake when ready
	while (true) {
		if (!isAboutToFire()) { // Not about to fire
			setLauncherSpeed(60);
		}
		else // About to fire
		{
			setLauncherSpeed(30);
			// TODO intake 1 ball
			break;
		}
		wait1Msec(25);
	}

	// Stop the motors once we slip
	int prevValue = SensorValue[armPot];
	int currentValue;
	while (true) {
		if (prevValue-currentValue > 200) { // TODO change
			setLauncherSpeed(0);
			break;
		}
		wait1Msec(10);
	}

	// Once we hit the arm reset the bar to hook position
	while (true) {
		if (SensorValue[barLimit] == 1) {
			wait1Msec(300);
			startTask( barHookPos );
			break;
		}
		wait1Msec(25);
	}

	isFiring = false; // Set to false so we can fire again
}

// Position:
// 1 - Long range
// 2 - Mid range
// 3 - Close range
void fire(int position) { // Fire loop
	if (canFire()) {
		firePosition = position;
		isFiring = true;
		startTask(fireTask);
	}
}
