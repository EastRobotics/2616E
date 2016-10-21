#pragma config(Sensor, dgtl1,  autonCont,      sensorTouch)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define DRIVE_THRESHOLD_FORWARD 15 // Joystick forward threshold
#define DRIVE_THRESHOLD_TURN 15 // Joystick turn threshold
#define DRIVE_THRESHOLD_STRAFE 15 // Joystick strafe threshold
#define INITIAL_DRIVE_POWER 25 //The power that drive with logic will start it's linear function at for drive power
#define JOYSTICK_MOVEMENT_THRESHOLD 15 //The amount the joystick has to move for it to be used in the linear function to calculate RPM


/////////////////////////////////////////////////////////////////////////////////////////
//
//                        Normal drive methods (No sensors)
//
/////////////////////////////////////////////////////////////////////////////////////////

// Cleaner way of setting all four drive motor speeds.
// Drive with checks to rule out errors with joystick controls
// PARAMETERS:
//	int: -127 to 127, speed for the front left motor
//	int: -127 to 127, speed for the back left motor
//	int: -127 to 127, speed for the front right motor
//	int: -127 to 127, speed for the back right motor
void driveRaw(int speedFL, int speedBL, int speedFR, int speedBR) {
	motor[driveFL] = speedFL; // Front left
	motor[driveBL] = speedBL; // Back left
	motor[driveFR] = speedFR; // Front right
	motor[driveBR] = speedBR; // Back right
}



// Set mecanum wheel motor speeds based on forward, turn, and strafe speeds
// PARAMETERS:
//	int: -127 to 127, speed to drive forward or backward respectively
//	int: -127 to 127, speed to turn left or right respectively
//	int: -127 to 127, speed to strafe left or right respectively
void drive(int speedForward, int speedTurn, int speedStrafe) {
	int speedFL = speedForward + speedTurn + speedStrafe;
	int speedBL = speedForward + speedTurn - speedStrafe;
	int speedFR = speedForward - speedTurn - speedStrafe;
	int speedBR = speedForward - speedTurn + speedStrafe;
	driveRaw(speedFL, speedBL, speedFR, speedBR);
}

void driveBackwards(int speedForward, int speedTurn, int speedStrafe) {
	int speedBR = speedForward + speedTurn + speedStrafe; // FL equivalent
	int speedFR = speedForward + speedTurn - speedStrafe; // BL equivalent
	int speedBL = speedForward - speedTurn - speedStrafe; // FR equivalent
	int speedFL = speedForward - speedTurn + speedStrafe; // BR equivalent
	driveRaw(speedFL, speedBL, speedFR, speedBR);
}

// Drive with checks to rule out errors with joystick controls
// NOTE:
// 	Multipliers should always be (0 <= x <= 1.0). This avoids going too fast or slow and going out of proportion
// PARAMETERS:
//	int: -127 to 127, speed to drive forward or backward respectively
//	int: -127 to 127, speed to turn left or right respectively
//	int: -127 to 127, speed to strafe left or right respectively
//	float: What to reduce forward/backward speed to (0.7 -> 70% of input)
//	float: What to reduce left/right turn speed to (0.7 -> 70% of input)
//	float: What to reduce left/right strafe speed to (0.7 -> 70% of input)
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe, bool reverse) {
	int multipliedSpeedForward = speedForward; // ((float) speedForward)*forwardMultiplier;
	int multipliedSpeedTurn = speedTurn; //((float) speedTurn)*turnMultiplier;
	int multipliedSpeedStrafe = speedStrafe; //((float) speedStrafe)*strafeMultiplier;

	byte forwardMult = (multipliedSpeedForward < 0) ? -1 : 1;
	byte turnMult = (multipliedSpeedTurn < 0) ? -1 : 1;
	byte strafeMult = (multipliedSpeedStrafe < 0) ? -1 : 1;

	multipliedSpeedForward = abs(multipliedSpeedForward);
	multipliedSpeedTurn = abs(multipliedSpeedTurn);
	multipliedSpeedStrafe = abs(multipliedSpeedStrafe);

	if (abs(multipliedSpeedForward) <= DRIVE_THRESHOLD_FORWARD) multipliedSpeedForward = 0;
	if (abs(multipliedSpeedTurn) <= DRIVE_THRESHOLD_TURN) multipliedSpeedTurn = 0;
	if (abs(multipliedSpeedStrafe) <= DRIVE_THRESHOLD_STRAFE) multipliedSpeedStrafe = 0;

	if(abs(multipliedSpeedForward) <= JOYSTICK_MOVEMENT_THRESHOLD) multipliedSpeedForward = 0;
	if(abs(multipliedSpeedTurn) <= JOYSTICK_MOVEMENT_THRESHOLD) multipliedSpeedTurn = 0;
	if(abs(multipliedSpeedStrafe) <= JOYSTICK_MOVEMENT_THRESHOLD) multipliedSpeedStrafe = 0;

	// TODO Cooler stuff than just thresholds :P
	// Consider it TODONE

	//uses linear interpolation or lerp to fix the logarithmic nature of a motor's RPM to motor speed ratio into linear growth
	multipliedSpeedForward = getLerpedSpeed(multipliedSpeedForward, INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_FORWARD);
	multipliedSpeedTurn = getLerpedSpeed(multipliedSpeedTurn, INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_TURN);
	multipliedSpeedStrafe = getLerpedSpeed(multipliedSpeedStrafe, INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_STRAFE);

	if (abs(speedForward) <= JOYSTICK_MOVEMENT_THRESHOLD) multipliedSpeedForward = 0;
	if (abs(speedTurn) <= JOYSTICK_MOVEMENT_THRESHOLD) multipliedSpeedTurn = 0;
	if (abs(speedStrafe) <= JOYSTICK_MOVEMENT_THRESHOLD) multipliedSpeedStrafe = 0;

	multipliedSpeedForward *= forwardMult;
	multipliedSpeedTurn *= turnMult;
	multipliedSpeedStrafe *= strafeMult;
	//Double strafe speeds
	multipliedSpeedStrafe *= 2;
	multipliedSpeedStrafe = (multipliedSpeedStrafe > 127) ? 127 : multipliedSpeedStrafe;

	if (!reverse)
		drive(multipliedSpeedForward, multipliedSpeedTurn, multipliedSpeedStrafe); // Pass off the checked values to drive
	else
		driveBackwards(multipliedSpeedForward, multipliedSpeedTurn, multipliedSpeedStrafe); // Pass off the checked values to drive
}

// Set each side of the drive to a certain speed.
// PARAMETERS:
//  int: How fast to move the left side (-127 to 127)
//  int: How fast to move the right side (-127 to 127)
void driveTank(int speedLeft, int speedRight){
	if(abs(speedLeft) <= DRIVE_THRESHOLD_FORWARD) speedLeft = 0;
	if(abs(speedRight) <= DRIVE_THRESHOLD_FORWARD) speedRight = 0;
	driveRaw(speedLeft,speedLeft,speedRight,speedRight);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                        Smart drive methods (Encoders & PID)
//
/////////////////////////////////////////////////////////////////////////////////////////

float RPMValues[10] = {0,0,0,0,0,0,0,0,0,0};
long lastTickCount[10] = {0,0,0,0,0,0,0,0,0,0};
float RPMReadRate = 20.0;

/*
task getRPMValues() {
	while(true){
		long tickCount[10] = {0,nMotorEncoder[driveFR],nMotorEncoder[driveBR],0,0,0,0,nMotorEncoder[driveFL],nMotorEncoder[driveBL],0};
		for(int i = 0; i < 10; i++){
			RPMValues[i] = (((((float)tickCount[i])-((float)lastTickCount[i]))*(60000.0/RPMReadRate))/627.2); // Made for torque geared motors
			lastTickCount[i] = tickCount[i];
		}
		Logging RPM
	  // datalogDataGroupStart();
	  // datalogAddValue(0,RPMValues[1]);
	  // datalogAddValue(1,RPMValues[2]);
	  // datalogAddValue(2,RPMValues[7]);
		// datalogAddValue(3,RPMValues[8]);
		// datalogAddValue(4,50);
		// datalogDataGroupEnd();

		wait1Msec((int)RPMReadRate);
	}
}
*/

// Sets the encoders on all of our drive motors back to 0.
void clearDriveEncoders() {
	nMotorEncoder[driveFR] = 0;
	nMotorEncoder[driveBR] = 0;
	nMotorEncoder[driveFL]= 0;
	nMotorEncoder[driveBL] = 0;
}

// Variables used for encoder. Should probably be replaced with a struct
tMotor motorsToChange[5] = {driveFR,driveFR,driveBR,driveFL,driveBL}; //robotc glitch with value 0 we think
bool motorToMotorReverse[5] = {false,false, false, false, false};
int motorsToChangeSpeed[5] = {80,80,80,80,80};
long tickTarget[5];

// Clean way to set variables for storing tick values
// PARAMETERS:
//  tMotor: array of motors to use
//  long: ticks to move
// NOTE:
//  - You should enter the delta, the method will figure out the total
//  - For example, if you want it to go forward 300 and the encoder is already
//    at 1000, you should still only put in 300.
//  - Negative = backwards wheel movement.
void setupMotorTicks(tMotor *_motorsToChange, long ticks) {
	motorsToChange = (tMotor) _motorsToChange;
	for(int i = 1; i < 5; i++){
		tickTarget[i] = (motorToMotorReverse[i]) ? ((ticks*-1)
		+ nMotorEncoder[motorsToChange[i]]) : (ticks + nMotorEncoder[motorsToChange[i]]);
	}
}

// Logic driving method to check against ticks. Base method for other encoder methods
// Reads encoder valte of the front motors for each side.
// PARAMETERS:
//  motor array: The motors to change
void driveTilEncoder(tMotor *_motorsToChange, int arrayLength) {
	writeDebugStreamLine("------------------------------------");
	writeDebugStreamLine("--       DRIVING TIL ENCODER      --");
	writeDebugStreamLine("------------------------------------");
	string debug = "";
	writeDebugStreamLine("Driving til Encoder");
	sprintf(debug,"FR: %i AR: %i",driveFR,motorsToChange[0]);
	writeDebugStreamLine(debug);
	motorsToChange = (tMotor) _motorsToChange;
	bool continueRunning = true;
	while (continueRunning) {
		continueRunning = false;
		for (int i=1; i<arrayLength; i++) {
			tMotor _motor = motorsToChange[i];
			// If we're not at the target yet
			sprintf(debug, "at %i : goal: %i",nMotorEncoder[_motor],tickTarget[i]);
			writeDebugStreamLine(debug);
			if (motorToMotorReverse[i] ? nMotorEncoder[_motor] > tickTarget[i]
				: nMotorEncoder[_motor] < tickTarget[i]) {
				// Make sure we keep running and are at the right speed
				continueRunning = true;
				motor[_motor] = (motorToMotorReverse[i]) ? motorsToChangeSpeed[i]*-1 : motorsToChangeSpeed[i];
				sprintf(debug,"set m %i to %i",i,motorsToChangeSpeed[i]);
				writeDebugStreamLine(debug);
				sprintf(debug,"mtmr: %d m:%i",motorToMotorReverse[i],_motor);
				writeDebugStreamLine(debug);
				sprintf(debug,"FR:%i,BR:%i,FL:%i,BL:%i",driveFR,driveBR,driveFL,driveBL);
				writeDebugStreamLine(debug);
				sprintf(debug,"FR:%i,BR:%i,FL:%i,BL:%i",motorsToChange[0],motorsToChange[1],motorsToChange[2],motorsToChange[3]);
				writeDebugStreamLine(debug);
				} else {
				// Stop the motor
				motor[_motor] = 0;
			}
		}
	}
}

void setEncoderDriveSpeed(int speed) {
	for(int i = 1; i < 5; i ++){
		motorsToChangeSpeed[i] = speed;
	}
}

// Point turn
void driveEncoderPointTurn(int ticks, bool right, int speed) {
	setEncoderDriveSpeed(speed);
	motorToMotorReverse[1] = right ? true : false;
	motorToMotorReverse[2] = right ? true : false;
	motorToMotorReverse[3] = right ? false : true;
	motorToMotorReverse[4] = right ? false : true;
	setupMotorTicks(motorsToChange, ticks);
	driveTilEncoder(motorsToChange, 5);
}

// Straight
void driveEncoderNormal(int ticks, bool forward, int speed) {
	setEncoderDriveSpeed(speed);
	motorToMotorReverse[1] = forward ? false : true;
	motorToMotorReverse[2] = forward ? false : true;
	motorToMotorReverse[3] = forward ? false : true;
	motorToMotorReverse[4] = forward ? false : true;
	setupMotorTicks(motorsToChange, ticks);
	driveTilEncoder(motorsToChange, 5);
}

// Strafe
void driveEncoderStrafe(int ticks, bool right, int speed) {
	setEncoderDriveSpeed(speed);
	motorToMotorReverse[1] = right ? true : false;
	motorToMotorReverse[2] = right ? false : true;
	motorToMotorReverse[3] = right ? false : true;
	motorToMotorReverse[4] = right ? true : false;
	setupMotorTicks(motorsToChange, ticks);
	driveTilEncoder(motorsToChange, 5);
}

// Future control loop example:
// We want to turn 45 degrees right...
// -> PID loop gets speed based on degree from angle
// --> Loop supplies speed to PID loop which keeps 1 wheel at that speed
// ---> 3rd loop keeps other wheels at their speeds respective to the first

/////////////////////////////////////////////////////////////////////////////////////////
//
//                        CRS (Currently on the backburner)
//
/////////////////////////////////////////////////////////////////////////////////////////

// Considers the input as if the front of the robot was at a different angle (For example, considering
// -> the right side of the robot to be the front, or the front right corner to be the front) all while
// -> facing the same direction so you can move towards a scoring object and launch stuff over the fence
// CRS: Correction of Rotation Stick (or crazy rotational stuff)
// Beta, will add documentation when complete
/*
void driveWithCRS(int speedForward, int speedStrafe, float startDegree, float currentDegree) {
if (speedForward <= DRIVE_THRESHOLD_FORWARD) speedForward = 0;
if (speedStrafe <= DRIVE_THRESHOLD_STRAFE) speedStrafe = 0;
float degree = degreeToRad(currentDegree-startDegree);
// Considering speed as x, x' = y*sin(a) + x*cos(a)
speedForward = speedStrafe*sin(degree) + speedForward*cos(degree);
// Considering strafe as y, y' = y*cos(a) - x*sin(a)
speedStrafe = speedStrafe*cos(degree) - speedForward*sin(degree);
}
*/

/////////////////////////////////////////////////////////////////////////////////////////
//
//                        PID (Currently on the backburner)
//
/////////////////////////////////////////////////////////////////////////////////////////

/*
tMotor motorsToChange[4] = {driveFL,driveFR,driveBR,driveBL};
bool motorToMotorReverse[4] = {false, false, false, false};
long tickTarget[4];
//Setup the weights for the various stages of pid
float kP = 0.01; //Proportional Gain
float kI = 0.0; //Integral Gain
float kD = 0.0; //Derivitive Gain
float kL = 50.0; //Apparently this is there to be the integral limit, I think we missed it when working last time

task drivePID() {
//Initialize the PID variables
long error[4] = {0, 0, 0, 0};
long pError[4] = {0, 0, 0, 0};
long p[4] = {0, 0, 0, 0};
long i[4] = {0, 0, 0, 0};
long d[4] = {0, 0, 0, 0};
string debugString = ""; //FOR DEBUG, REMOVE LATER
writeDebugStreamLine("STARTING PID");

while(true){
long driveAvg = 0;
long errorAvg = 0;
for (int index = 0; index < 4; index ++){
error[index] = tickTarget[index] - nMotorEncoder[motorsToChange[index]];
//DEBUG
debugString = "";
sprintf(debugString,"Er[%i]:%i",index,error[index]);
writeDebugStreamLine(debugString);
//DEBUG
p[index] = error[index]; //P is simply the error
i[index] = abs(i[index] + error[index]) < kL ? i[index] + error[index] : sgn(i[index] + error[index])*kL; //I is the sum of errors
d[index] = error[index] - pError[index]; //D is the change in error or delta error
driveAvg += abs(p[index]*kP + i[index]*kI + d[index]*kD);
errorAvg += abs(error[index]);
}
//Find the average drive speed and error
driveAvg = round(driveAvg/4);
errorAvg = round(errorAvg/4);
debugString = "";
sprintf(debugString,"DrAv:%i \nErAv:%i",driveAvg,errorAvg);
writeDebugStreamLine(debugString);
//float slope = ((105.0-(float)INITIAL_DRIVE_POWER)/(127.0-(float)JOYSTICK_MOVEMENT_THRESHOLD));
//float yInt = 105.0-(slope*127.0);
//int speedForward = RPMToMotor((driveAvg*slope)+yInt);
for(int index = 0; index < 4; index ++){
debugString = "";
sprintf(debugString,"MoSp:%i",(abs(errorAvg) > 5) ? ((motorToMotorReverse[index]) ? (driveAvg*-1) : (driveAvg)) : (0)); //TODO switch back to speedForward
writeDebugStreamLine(debugString);
//if the error is low turn off the motor, otherwise set to average, and reverse if this motor is meant to be reversed
motor[motorsToChange[index]] = (abs(errorAvg) > 5) ? ((motorToMotorReverse[index]) ? (driveAvg*-1) : (driveAvg)) : (0); //TODO switch back to speedForward
}
//if the error is low end the PID loop
if(abs(errorAvg) < 5){
writeDebugStreamLine("STOPPING PID");
break;
}
wait1Msec(25);
}
}

// Drives the robot forward using PID to keep straight
// PARAMETERS:
//  long: How much to move forward (if positive) or backwards (if negative)
void driveStraightPID(long ticksToMove) {
bool _motorToMotorReverse[4] = {false, false, false, false};
motorToMotorReverse = _motorToMotorReverse;
tMotor _motorsToChange[4] = {driveFL,driveFR,driveBR,driveBL};
setupMotorTicks(_motorsToChange, ticksToMove);
startTask( drivePID );
}

// Does a point turn using PID to stay in place
// PARAMETERS:
//  long: How much to turn right (if positive) or left (if negative)
void drivePointTurnPID(long ticksToMove) {
bool _motorToMotorReverse[4] = {false, true, true, false};
motorToMotorReverse = _motorToMotorReverse;
tMotor _motorsToChange[4] = {driveFL,driveFR,driveBR,driveBL};
setupMotorTicks(_motorsToChange, ticksToMove);
startTask( drivePID );
}
*/
