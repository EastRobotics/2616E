#define DRIVE_THRESHOLD_FORWARD 15 // Joystick forward threshold
#define DRIVE_THRESHOLD_TURN 15 // Joystick turn threshold
#define DRIVE_THRESHOLD_STRAFE 15 // Joystick strafe threshold

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
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe, bool reverse, float forwardMultiplier, float turnMultiplier, float strafeMultiplier) {
	int multipliedSpeedForward = speedForward; // ((float) speedForward)*forwardMultiplier;
	int multipliedSpeedTurn = speedTurn; //((float) speedTurn)*turnMultiplier;
	int multipliedSpeedStrafe = speedStrafe; //((float) speedStrafe)*strafeMultiplier;

	if (abs(multipliedSpeedForward) <= DRIVE_THRESHOLD_FORWARD) multipliedSpeedForward = 0;
	if (abs(multipliedSpeedTurn) <= DRIVE_THRESHOLD_TURN) multipliedSpeedTurn = 0;
	if (abs(multipliedSpeedStrafe) <= DRIVE_THRESHOLD_STRAFE) multipliedSpeedStrafe = 0;

	// TODO Cooler stuff than just thresholds :P

	if (!reverse)
		drive(multipliedSpeedForward, multipliedSpeedTurn, multipliedSpeedStrafe); // Pass off the checked values to drive
	else
		driveBackwards(multipliedSpeedForward, multipliedSpeedTurn, multipliedSpeedStrafe); // Pass off the checked values to drive
}

// Overriding method. See driveWithLogic(int, int, int, float, float, float).
// NOTE:
// 	-> Sets multipliers to 1.0
void driveWithLogic(int speedForward, int speedTurn, int speedStrafe, bool reverse) {
	driveWithLogic(speedForward, speedTurn, speedStrafe, reverse, 1.0, 1.0, 1.0);
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

// Sets the encoders on all of our drive motors back to 0.
void clearDriveEncoders() {
	nMotorEncoder[driveFR] = 0;
	nMotorEncoder[driveFL] = 0;
	nMotorEncoder[driveBL]= 0;
	nMotorEncoder[driveBR] = 0;
}

// Will make sure we don't do conflicting tasks in the future
bool doingOperation = false;

// This is used by the motorMirrorTask to store what it needs to do
tMotor motorToMotorData[3][2];
bool motorToMotorReverse[3];

// This task sets the value motor speed to the key motor speed in the nested arrays above using PID
// Michael: You might need to store more data than what's above, so feel free to create more arrays.
// Just make sure you're setting and clearing them in the below code
task setMotorsToMotorsPID() {
	// Ends when task is terminated by the target controlling task
	while (true) {
		// Loop through each key/value pair of motors
		for (int key = 0; key < 2; key++) {
			// Check if either of the motors are null, aka not set up
			if (!motorToMotorData[key][0] || !motorToMotorData[key][1])
				continue;
			bool reverse = motorToMotorReverse[key]; // Whether or not to match the speed opposite
			int currentSpeed = motor[motorToMotorData[key][1]]; // Motor speed of our value (-127 to 127)
			// Michael: This is where you do the PID stuffs to match motors.
			// You'll need to get RPM of both motors, and increment currentSpeed to try to match motorToMotorData[key][1]
			// Then set the motor like following
			// motor[motorToMotorData[key][1]] = new speed
		}
	}
}

tMotor motorToChange;
long tickTarget;
float lastError;

int PID_SENSOR_SCALE = 1;

int PID_MOTOR_SCALE = 1;

int PID_DRIVE_MAX = 127;
int PID_DRIVE_MIN = -127;

int PID_INTEGRAL_LIMIT = 50;

// These could be constants but leaving
// as variables allows them to be modified in the debugger "live"
float  pid_Kp = 2.0;
float  pid_Ki = 0.04;
float  pid_Kd = 0.04;

static int   pidRunning = 1;
static float pidRequestedValue;

// This method is used for getting a motor encoder value to match a target
// This will likely be replaced in the future to integrate accelerometer/gyro alongside encoders
// Basically, gyro PID supplies speed to turn at and which direction, encoder PID keeps us at that speed
task driveMotorToTargetPID() {
	doingOperation = true;
	startTask( setMotorsToMotorsPID );

	// Init the variables
	float  pidSensorCurrentValue;

	float  pidError;
	float  pidLastError;
	float  pidIntegral;
	float  pidDerivative;
	float  pidDrive;

	pidLastError  = 0;
	pidIntegral   = 0;

	// Michael: This is where you do the PID stuffs to move a certain distance.
	// curentTicks is the motors current position, the above tickTarget is what we need to be at
	// Make sure you're checking nMotorEncoder ticks the current to see which way you need to spin

	while( true )
	{
		// Is PID control active ?
		if( doingOperation )
		{
			// Read the sensor value and scale
			long pidSensorCurrentValue = nMotorEncoder[motorToChange] * ((long)PID_SENSOR_SCALE);

			// calculate error
			pidError = ((float)pidSensorCurrentValue) - ((float)tickTarget);

			// integral - if Ki is not 0
			if( pid_Ki != 0 )
			{
				// If we are inside controlable window then integrate the error
				if( abs(pidError) < PID_INTEGRAL_LIMIT )
					pidIntegral = pidIntegral + pidError;
				else
					pidIntegral = 0;
			}
			else
				pidIntegral = 0;

			// calculate the derivative
			pidDerivative = pidError - pidLastError;
			pidLastError  = pidError;

			// calculate drive
			pidDrive = (pid_Kp * pidError) + (pid_Ki * pidIntegral) + (pid_Kd * pidDerivative);

			// limit drive
			if( pidDrive > PID_DRIVE_MAX )
				pidDrive = PID_DRIVE_MAX;
			if( pidDrive < PID_DRIVE_MIN )
				pidDrive = PID_DRIVE_MIN;

			// send to motor
			motor[ motorToChange ] = pidDrive * PID_MOTOR_SCALE;

		}
		else
		{
			// clear all
			pidError      = 0;
			pidLastError  = 0;
			pidIntegral   = 0;
			pidDerivative = 0;
		}

		// Run at 50Hz
		wait1Msec( 25 );
		endTimeSlice();
	}

	// Then set the motor like following
	// motor[motorToChange] = new speed

	// IMPORTANT: Make sure when we reach the target, you're calling "break" so we stop the mirroring

	stopTask( setMotorsToMotorsPID );
	doingOperation = false;
}

// Clean way to set variables for driveMotorToTargetPID
// PARAMETERS:
//  tMotor: motor to use
//  long: ticks to move
// NOTE:
//  - You should enter the delta, the method will figure out the total
//  - For example, if you want it to go forward 300 and the encoder is already
//    at 1000, you should still only put in 300.
//  - Negative = backwards wheel movement.
void setupMotorTicks(tMotor _motorToChange, long ticks) {
	motorToChange = _motorToChange;
	tickTarget = ticks + nMotorEncoder[motorToChange];
}

// Drives the robot forward using PID to keep straight
// PARAMETERS:
//  long: How much to move forward (if positive) or backwards (if negative)
void driveStraightPID(long ticksToMove) {
	setupMotorTicks(driveFL, ticksToMove);
	tMotor _motorToMotorData[3][2] = {{driveFL, driveFR},{driveFL, driveBR},{driveFL, driveBL}};
	motorToMotorData = _motorToMotorData;
	bool _motorToMotorReverse[3] = {false, false, false};
	motorToMotorReverse = _motorToMotorReverse;
	startTask(driveMotorToTargetPID);
}

// Does a point turn using PID to stay in place
// PARAMETERS:
//  long: How much to turn right (if positive) or left (if negative)
void drivePointTurnPID(long ticksToMove) {
	setupMotorTicks(driveFL, ticksToMove);
	tMotor _motorToMotorData[3][2] = {{driveFL, driveFR},{driveFL, driveBL},{driveFR, driveBR}};
	motorToMotorData = _motorToMotorData;
	bool _motorToMotorReverse[3] = {true, false, false};
	motorToMotorReverse = _motorToMotorReverse;
	startTask(driveMotorToTargetPID);
}

// TODO: Set all the values here, they're currently same as point turn
// Does a strafe in a certain direction using PID to stay straight
// PARAMETERS:
//  long: How much to move right (if positive) or left (if negative)
void driveStrafePID(long ticksToMove) {
	setupMotorTicks(driveFL, ticksToMove);
	tMotor _motorToMotorData[3][2] = {{driveFL, driveFR},{driveFL, driveBL},{driveFR, driveBR}};
	motorToMotorData = _motorToMotorData;
	bool _motorToMotorReverse[3] = {true, false, false};
	motorToMotorReverse = _motorToMotorReverse;
	startTask(driveMotorToTargetPID);
}

// DEPRECATED: Soon to be removed in favor of PID methods
// Drive tick method. Controls left and right side useful for point turns.
// Reads encoder valte of the front motors for each side.
// PARAMETERS:
//  int: How fast to move the left side drive (-127 to 127)
//  long: How far to spin the left drive, in ticks. If above speed is negative, so should this number.
//  int: How fast to move the right side drive (-127 to 127)
//  long: How far to spin the right drive, in ticks. If above speed is negative, so should this number.
/*void driveTilRightLeft(int speedLeft, long ticksLeft, int speedRight, long ticksRight) {
//setupMotorTicks(ticksLeft, ticksLeft, ticksRight, ticksRight);
// While we're not where we want to be (Ternary allows for both forward and backward input)
while ((ticksLeft > 0 ? nMotorEncoder[driveFL] < ticksFrontLeft : nMotorEncoder[driveFL] > ticksFrontLeft) ||
(ticksRight > 0 ? nMotorEncoder[driveFR] < ticksFrontRight : nMotorEncoder[driveFR] > ticksFrontRight)) {
driveRaw(speedLeft, speedLeft, speedRight, speedRight);
}
}*/

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
