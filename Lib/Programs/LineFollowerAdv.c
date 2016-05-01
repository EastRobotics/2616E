#pragma config(Sensor, in1,    lineLeft,       sensorLineFollower)
#pragma config(Sensor, in2,    lineCenter,     sensorLineFollower)
#pragma config(Sensor, in3,    lineRight,      sensorLineFollower)
#pragma config(Sensor, in4,    lineTurnLeft,   sensorLineFollower)
#pragma config(Sensor, in5,    lineTurnRight,  sensorLineFollower)
#pragma config(Motor,  port1,           motorFrontLeft, tmotorNone, openLoop)
#pragma config(Motor,  port2,           motorBackLeft, tmotorNone, openLoop)
#pragma config(Motor,  port3,           motorFrontRight, tmotorNone, openLoop)
#pragma config(Motor,  port4,           motorBackRight, tmotorNone, openLoop)

/////////////////////////////////////////////////////////////////////////////////////////
//
//	                 -={ Advanced Line Following Example }=-
//
// This program uses either 3 or 5 line followers, depending on your setup. It uses higher levels
// of logic than a lot of other line followers, allowing it to fail safer and find the line again
// when something goes wrong. The pragmas above are just examples. You can change them to whatever you want
//
/////////////////////////////// Sensor Placement ////////////////////////////////////////
//
// The center, left, and right sensors should be in a group of 3 right next to eachother.
// The group should be ~3-4 inches back from the front of the robot and centered as much as possible.
// We've found that moving the center sensor ~1/2 an inch back helps with this programs smoothing loops.
//
/////////////////////////////////////////////////////////////////////////////////////////

// Setup vars
int threshold = 1200; // Below/equal this value counts as white, above counts as black.
int lineType = 0; // 0 for white line on dark surface, 1 for black line on light surface
bool runTurnLogic = true; // Whether or not you have 2 sensors in the front for turn logic

// Speed vars
int oneForwardSpeed = 25; // Used if only the right or left sensor sees line
int oneStrafeSpeed = 0; // Used if only the right or left sensor sees line
int oneTurnSpeed = 50; // Used if only the right or left sensor sees line


int centerForwardSpeed = 50; // Used if the center sees line
int centerStrafeSpeed = 10; // Used if the center and a side sees line
int centerTurnSpeed = -10; // Used if the center and a sidesees line


int noneForwardSpeed = 10; // Used if finding the line again when we loose it
int noneStrafeSpeed = 30; // Used if finding the line again when we loose it
int noneTurnSpeed = -20; // Used if finding the line again when we loose it


// Returns the opposite of the given value. Mainly for readability.
// PARAMETERS:
// 	int: The number to reverse
// RETURNS:
//	int: The reversed number
int oppositeOf(int value) {
	return -1*value;
}

// Determines whether or not the sensor value is a detected line based on above vars.
// PARAMETERS:
// 	int: Input from a line follow sensor, read like SensorValue[sensor]
// RETURNS:
//	bool: True if it's a line, false if not
bool isLine(int sensorData) {
	if (lineType == 0)
		return sensorData<=threshold;
	if (lineType == 1)
		return sensorData>threshold;
	return false;
}

// Should handle driving, same way as you would from a controller.
// PARAMETERS:
//	int: -127 to 127, forward/backward respectively
//	int: -127 to 127, strafe left/right respectively
// 		-> Note, if you're using a non-strafeable drive, just ignore this value.
//	int: -127 to 127, turn left/right respectively
void drive(int forwardDirection, int strafeDirection, int turnDirection) {
	// This is for a mecanum drive, however you should replace it for your drive
	motor[motorFrontLeft] = forwardDirection + turnDirection + strafeDirection;
	motor[motorBackLeft] =  forwardDirection + turnDirection - strafeDirection;
	motor[motorFrontRight] = forwardDirection - turnDirection - strafeDirection;
	motor[motorBackRight] =  forwardDirection - turnDirection + strafeDirection;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//														-={ Main line following task }=-
//
//	You should start this task whenever you want to follow a line, for example in autonomous.
//
/////////////////////////////////////////////////////////////////////////////////////////

task followLine() {
	// If we have an invalid line type, assume the default for vex
	if (lineType != 1 && lineType != 0)
		lineType = 0;

	// Direction variable
	// 	-1: Not yet set
	//	 0:
	//	 1:
	//	 2:
	int lastDirection = -1;

	// NOTE: If you want to interact with the vars (declared outside of while) just move the declarations
	// 	-> outside of the task and create methods to read them.

	// Normal line variables
	int lastTurn = -1;
	int moving = -1;
	int lineL = 0;
	int lineR = 0;
	int lineM = 0;

	// Turn logic variables
	bool detectedLeft, detectedRight = false;
	bool foundLine = true;
	bool droppedLine = false;
	int lineFL = 0;
	int lineFR = 0;

	// This loop is the heart of the line follower. It's what does all of the checks and determines what to do
	while(true){
		lineL = SensorValue[lineLeft];
		lineR = SensorValue[lineRight];
		lineM = SensorValue[lineCenter];
		string lineValue;

		if (runTurnLogic) {
			lineFL = SensorValue[lineTurnLeft]; // Set this to 0 instead of SensorValue[x] if not using turn logic
			lineFR = SensorValue[lineTurnRight]; // Set this to 0 instead of SensorValue[x] if not using turn logic
			if (foundLine && droppedLine) {
				detectedLeft = false;
				detectedRight = false;
			}
			if(isLine(lineFL)){
				detectedLeft = true;
			}
			if(isLine(lineFR)){
				detectedRight = true;
			}
		}

		// Since we're not continuing when we get a positive check, the center check always runs last.
		// This means that even if we set the motor speeds for left/right sensor only, they'll be overwritten
		// by the center line check.

		// RIGHT sensor sees line:
		if(isLine(lineR))
		{
			// Turn towards the right and move forward
			drive(oneForwardSpeed,oneStrafeSpeed,oneTurnSpeed);
			// Set our logic vars
			lastDirection = lastTurn = 0;
			moving = 0;
		}

		// LEFT sensor sees line:
		if(isLine(lineL))
		{
			// Turn towards the left and move forward
			drive(oneForwardSpeed,oppositeOf(oneStrafeSpeed),oppositeOf(oneTurnSpeed));
			// Set our logic vars
			lastDirection = lastTurn = 1;
			moving = 2;
		}

		// CENTER sensor sees line:
		if(isLine(lineM))
		{
			// Default to drive straight. If left and right don't see line, this will be used
			drive(centerForwardSpeed,0,0);
			// The below block smooths the robot on a straight line, so we don't follow a straight line
			// -> in zig zags.
			if (!(isLine(lineL) && isLine(lineR))) { // If not both left and right see line
				// If white sensor sees line
				if (isLine(lineL)) {
					// Turn slightly left, strafe left, and go straight
					drive(centerForwardSpeed,centerStrafeSpeed,centerTurnSpeed);
				}	else
					if (isLine(lineR)) {
					// Turn slightly right, strafe right, and go straight
					drive(centerForwardSpeed,oppositeOf(centerStrafeSpeed),oppositeOf(centerTurnSpeed));
				}
			}

			// Set our logic vars
			moving = 1;
			lastDirection = 2;
		}

		// If none of the sensors detected a line
		if(!(isLine(lineL)||isLine(lineM)||isLine(lineR))){
			// So we can run an 'else if' type check with ifs in different blocks
			bool ranTurnLogic = false;
			// We know we aren't on a line, meaning we dropped one. Tell the program.
			foundLine = false;
			droppedLine = true;
			// Set our logic vars
			moving = 5;
			// Run our turn logic. *** HERE IS WHERE YOU DO ANY LOGICAL TURN HANDLING IF YOU PLEASE ***
			if (runTurnLogic) {
				// At this point, it's really up to the challenge and programmer
				// to decide how to take our action. It's really purpose specific.
				// For our test robot, it just turns in the direction of the line.
				// NOTE: To handle a T- intersection, check if both are true before checking
				// them individually.
				if(detectedLeft){
					drive(0,0,70);
					} else if(detectedRight){
					drive(0,0,-70);
			}}


			// If we didn't run turn logic, that means we lost the line. Figure out where we were going.
			if (lastTurn ==1 && !ranTurnLogic) {
				// We last turned right, so go left
				drive(noneForwardSpeed,noneStrafeSpeed,noneTurnSpeed);
				moving = 3;
				} else if(lastTurn==0){
				// We last turned left, so go right
				drive(noneForwardSpeed,oppositeOf(noneStrafeSpeed),oppositeOf(noneTurnSpeed));
				moving = 4;
				} else {
				// If all else fails, pivot turn right to victory
				drive(0,0,-20);
				// If you are getting a moving above 6, you know it's this.
				// Subtract 6 to figure out what value you aren't handling
				moving = 6+lastDirection;
			}
		} else // One of the sensors had a value, so we know we found the line
			foundLine = true;
	}
}
