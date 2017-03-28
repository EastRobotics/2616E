#include "main.h"
#include "JINX.h"

#define DRIVE_THRESHOLD_FORWARD 15 // Joystick forward threshold
#define DRIVE_THRESHOLD_TURN 15    // Joystick turn threshold
#define DRIVE_THRESHOLD_STRAFE 15  // Joystick strafe threshold
#define INITIAL_DRIVE_POWER                                                    \
  25 // The power that drive with logic will start it's linear function at for
     // drive power
#define JOYSTICK_MOVEMENT_THRESHOLD                                            \
  15 // The amount the joystick has to move for it to be used in the linear
     // function to calculate RPM

#define PID_SENSOR_INDEX    myEncoder

#define PID_MOTOR_INDEX     myMotor
#define PID_MOTOR_SCALE     -1

#define PID_DRIVE_MAX       80

#define PID_INTEGRAL_LIMIT  50

#define PID_STRAIGHT_THRESH 4

unsigned char driveFL, driveBL, driveFR, driveBR;
char lastDriveFL, lastDriveBL, lastDriveFR, lastDriveBR;
bool driveFLReverse, driveBLReverse, driveFRReverse, driveBRReverse;
bool slewRate = 0;

/*
** Sets the drive motors we're using. This makes it so we don't need to provide
** the ports every time we want to do any drive methods.
**
** PARAMETERS:
**   unsigned char: Front left  drive motor port
**   unsigned char: Back  left  drive motor port
**   unsigned char: Front right drive motor port
**   unsigned char: Back  right drive motor port
*/
void driveInit(unsigned char _driveFL, unsigned char _driveBL,
               unsigned char _driveFR, unsigned char _driveBR) {
  print("[ELib] Initialized drive motors\n");
  driveFL = _driveFL; // Set our front left motor
  driveBL = _driveBL; // Set our back  left motor
  driveFR = _driveFR; // Set our front right motor
  driveBR = _driveBR; // Set our back  right motor
}

/*
** Sets whether each drive motor should be reverse or not
**
** PARAMETERS:
**   bool: Front left  drive motor reverse
**   bool: Back  left  drive motor reverse
**   bool: Front right drive motor reverse
**   bool: Back  right drive motor reverse
*/
void driveSetReverse(bool _driveFLReverse, bool _driveBLReverse,
                     bool _driveFRReverse, bool _driveBRReverse) {
  driveFLReverse = _driveFLReverse;
  driveBLReverse = _driveBLReverse;
  driveFRReverse = _driveFRReverse;
  driveBRReverse = _driveBRReverse;
}

/*
** Sets the given motor's speed, assuming the motor is valid. Otherwise, an
** error is output to the console.
**
** PARAMETERS:
**   unsigned char: Motor port to be set
**   int: The speed of the motor, -127 to 127
**   const char *: String to represent motor in console output if invalid
**
** OPTIONAL PARAMETERS:
**   const char *: Text representation of motor, used for error message
**
** NOTES:
**   Calls to printf can be taxing, so try to not have any erroneous calls
*/
void driveIfValid(unsigned char motor, int speed,
                  const char *string) { // Eventually want default for this, but
                                        // causing errors
  if (motor != NULL)
    motorSet(motor, speed);
  else
    printf(
        "[ELib] Tried to set motor speed, but motor wasn't initialized! [%s]\n",
        string);
}

/*
** Directly sets the motor speeds, checking to make sure the motors have been
** set. Base method for most other drive methods.
**
** PARAMETERS:
**   int: The speed of the front left  motor, -127 to 127
**   int: The speed of the back  left  motor, -127 to 127
**   int: The speed of the front right motor, -127 to 127
**   int: The speed of the back  right motor, -127 to 127
*/
void driveRaw(int speedFL, int speedBL, int speedFR, int speedBR) {
  // Set drive front left
  lastDriveFL = speedFL; // Save speed for slew code
  driveIfValid(driveFL, speedFL * (driveFLReverse ? -1 : 1), "driveFL");
  // Set drive back  left
  lastDriveBL = speedBL; // Save speed for slew code
  driveIfValid(driveBL, speedBL * (driveBLReverse ? -1 : 1), "driveBL");
  // Set drive front right
  lastDriveFR = speedFR; // Save speed for slew code
  driveIfValid(driveFR, speedFR * (driveFRReverse ? -1 : 1), "driveFR");
  // Set drive back  right
  lastDriveBR = speedBR; // Save speed for slew code
  driveIfValid(driveBR, speedBR * (driveBRReverse ? -1 : 1), "driveBR");
}

// Enables slew rate, limiting speed change to _slewRate every update cycle
// PARAMETERS:
//	int: Amount of speed change allowed every 20ms
void enableSlew(int _slewRate) { slewRate = _slewRate; }

// Disables slew rate code
void disableSlew() { slewRate = 0; }

// Returns a speed based on the last speed and set slew rate
// PARAMETERS:
//	int: The current speed of the drive
//  int: The target speed of the drive
// RETURNS:
//  int: The slewed speed to set the motors to
int slew(int currentSpeed, int targetSpeed) {
  // If slew rate is off or we are closer to the target than slewRate
  if (slewRate == 0 || abs(currentSpeed - targetSpeed) < slewRate)
    return targetSpeed; // Return the target
  else {                // We need to slew
    // Add positive or negative slewRate to the given speed and return it
    currentSpeed += (currentSpeed > targetSpeed) ? -1 * slewRate : slewRate;
    return currentSpeed;
  }
}

/*
** Calls driveRaw with slewed speeds, using slew rate set by enableSlew
**
** PARAMETERS:
**   int: The target speed of the front left  motor, -127 to 127
**   int: The target speed of the back  left  motor, -127 to 127
**   int: The target speed of the front right motor, -127 to 127
**   int: The target speed of the back  right motor, -127 to 127
*/
void driveRawSlew(int speedFL, int speedBL, int speedFR, int speedBR) {
  driveRaw(slew(lastDriveFL, speedFL), slew(lastDriveBL, speedBL),
           slew(lastDriveFR, speedFR), slew(lastDriveBR, speedBR));
}

/*
** Sets the drive motors based on forward and turn speed. This is for tank
** style drives. Use driveHolonomic() for x-drive and mecanum style drives.
**
** PARAMETERS:
**   int: The speed/rate to move, -127 (backward) to 127 (forward)
**   int: The speed/rate to turn, -127 (left)     to 127 (right)
*/
void drive(int speedForward, int speedTurn) {
  int speedFL = speedForward - speedTurn;
  int speedBL = speedForward - speedTurn;
  int speedFR = speedForward + speedTurn;
  int speedBR = speedForward + speedTurn;
  driveRawSlew(speedFL, speedBL, speedFR, speedBR);
}

/*
** Sets left and right side drive speeds. Very basic drive style, and it's
** suggested to use drive() for normal driving. Made for tank style drives.

** PARAMETERS:
**   int: The speed for left  drive, -127 (backward) to 127 (forward)
**   int: The speed for right drive, -127 (backward) to 127 (forward)
*/
void driveTank(int speedLeft, int speedRight) {
  driveRawSlew(speedLeft, speedLeft, speedRight, speedRight);
}

/*
** Sets the drive motors based on forward, turn, and strafe speed. This is for
** holonomic drives like x-drives and mecanum. Use drive() for tank drives.
**
** PARAMETERS:
**   int: The speed/rate to move,   -127 (backward) to 127 (forward)
**   int: The speed/rate to turn,   -127 (left)     to 127 (right)
**   int: The speed/rate to strafe, -127 (left)     to 127 (right)
*/
void driveHolonomic(int speedForward, int speedTurn, int speedStrafe) {
  int speedFL = speedForward + speedTurn + speedStrafe;
  int speedBL = speedForward + speedTurn - speedStrafe;
  int speedFR = speedForward - speedTurn - speedStrafe;
  int speedBR = speedForward - speedTurn + speedStrafe;
  driveRawSlew(speedFL, speedBL, speedFR, speedBR);
}

// Drive with checks to rule out errors with joystick controls
// NOTES:
// 	Multipliers should always be (0 <= x <= 1.0). This avoids going too fast
// or slow and going out of proportion
// PARAMETERS:
//	int: -127 to 127, speed to drive forward or backward respectively
//	int: -127 to 127, speed to turn left or right respectively
//	int: -127 to 127, speed to strafe left or right respectively
//	float: What to reduce forward/backward speed to (0.7 -> 70% of input)
//	float: What to reduce left/right turn speed to (0.7 -> 70% of input)
//	float: What to reduce left/right strafe speed to (0.7 -> 70% of input)
void driveHolonomicWithLogic(int speedForward, int speedTurn, int speedStrafe) {
  int multipliedSpeedForward =
      speedForward; // ((float) speedForward)*forwardMultiplier;
  int multipliedSpeedTurn = speedTurn; //((float) speedTurn)*turnMultiplier;
  int multipliedSpeedStrafe =
      speedStrafe; //((float) speedStrafe)*strafeMultiplier;

  char forwardMult = (multipliedSpeedForward < 0) ? -1 : 1;
  char turnMult = (multipliedSpeedTurn < 0) ? -1 : 1;
  char strafeMult = (multipliedSpeedStrafe < 0) ? -1 : 1;

  multipliedSpeedForward = abs(multipliedSpeedForward);
  multipliedSpeedTurn = abs(multipliedSpeedTurn);
  multipliedSpeedStrafe = abs(multipliedSpeedStrafe);

  if (abs(multipliedSpeedForward) <= DRIVE_THRESHOLD_FORWARD)
    multipliedSpeedForward = 0;
  if (abs(multipliedSpeedTurn) <= DRIVE_THRESHOLD_TURN)
    multipliedSpeedTurn = 0;
  if (abs(multipliedSpeedStrafe) <= DRIVE_THRESHOLD_STRAFE)
    multipliedSpeedStrafe = 0;

  if (abs(multipliedSpeedForward) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedForward = 0;
  if (abs(multipliedSpeedTurn) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedTurn = 0;
  if (abs(multipliedSpeedStrafe) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedStrafe = 0;

  // Uses linear interpolation/lerp to fix the logarithmic nature of a motor's
  //  RPM to motor speed ratio into linear growth
  multipliedSpeedForward = getLerpedSpeed(
      multipliedSpeedForward, INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_FORWARD);
  multipliedSpeedTurn = getLerpedSpeed(multipliedSpeedTurn, INITIAL_DRIVE_POWER,
                                       DRIVE_THRESHOLD_TURN);
  multipliedSpeedStrafe = getLerpedSpeed(
      multipliedSpeedStrafe, INITIAL_DRIVE_POWER, DRIVE_THRESHOLD_STRAFE);

  if (abs(speedForward) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedForward = 0;
  if (abs(speedTurn) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedTurn = 0;
  if (abs(speedStrafe) <= JOYSTICK_MOVEMENT_THRESHOLD)
    multipliedSpeedStrafe = 0;

  multipliedSpeedForward *= forwardMult;
  multipliedSpeedTurn *= turnMult;
  multipliedSpeedStrafe *= strafeMult;
  // Double strafe speeds
  multipliedSpeedStrafe *= 2;
  multipliedSpeedStrafe =
      (multipliedSpeedStrafe > 127) ? 127 : multipliedSpeedStrafe;

  driveHolonomic(multipliedSpeedForward, multipliedSpeedTurn,
                 multipliedSpeedStrafe); // Pass off the checked values to drive
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                        PID (Currently on the frontburner)(lol)
//
/////////////////////////////////////////////////////////////////////////////////////////

//Setup the weights for the various stages of pid
float kP = 0.05; //Proportional Gain
float kI = 0.00; //Integral Gain
float kD = 0.00; //Derivitive Gain
float kL = 0; // Set integral limit to only allow for speed delta of 20

bool pidRunning = false;
float pidRequestedValue = 0;
int pidMode = 0;
int pidSensor = -1; // -1 for encoders, port of the sensor to use a sensor
int pidSensorOffset = 0;  // amount to offset the sensor by (used for gyro turns)
float pidSensorScale = 1.0;
bool straightAssist = true;
int straightStartAngle = 0;
int breakoutCount = 0;

TaskHandle pidTask;

int pidCorrectSpeed(int input) {
	// limit drive
	if( abs(input) > PID_DRIVE_MAX )
		return (input > 0) ? PID_DRIVE_MAX : PID_DRIVE_MAX * -1;
	return input;
}

void taskDrivePid(void * ignored) {
	pidRunning = true;
  print("[PID] Ich bin come to life");

	if (straightAssist) {
		straightStartAngle = analogRead(ANALOG_GYRO);
	}

	float  pidSensorCurrentValue;
	float  pidError;
	float  pidLastError;
	float  pidIntegral;
	float  pidDerivative;
	float  pidDrive;

	encoderReset(getEncoderBL());
  encoderReset(getEncoderBR());

	// Init the variables - thanks Glenn :)
	pidLastError  = 0;
	pidIntegral   = 0;
	breakoutCount = 0;

	pidRequestedValue += ((pidSensor==-1) ? encoderGet(getEncoderBL()) : 0); // Account for existing ticks

	while( true )
	{
		// Is PID control active ?
		if( true )
		{
      print("[PID] Ich bin in the loop!");
			// Read the sensor value and scale
			pidSensorCurrentValue = ((pidSensor==-1) ? encoderGet(getEncoderBL()) :
          digitalRead(pidSensor) * pidSensorScale + pidSensorOffset);

			// calculate error
			pidError = pidSensorCurrentValue - pidRequestedValue;

			// integral - if Ki is not 0
			if( kI != 0 )
			{
				// If we are inside controlable window then integrate the error
				if (abs(pidError) < kL)
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
			pidDrive = (kP * pidError) + (kI * pidIntegral) + (kD * pidDerivative);
			pidDrive *= PID_MOTOR_SCALE;

			// limit drive
			pidDrive = pidCorrectSpeed(pidDrive);

      // Log base drive speed
      static char temp[16];
      sprintf(temp, "%f", pidDrive);
      writeJINXData ("pidDrive", temp);
      // Log error
      memset(&temp[0], 0, sizeof(temp));
      sprintf(temp, "%f", pidError);
      writeJINXData ("pidError", temp);

			int pidDriveLeft = pidDrive;
			int pidDriveRight = pidDrive;
			// send to motor
			if (pidMode == 0) { // Normal drive
				// Save speeds to positive values
				int turningFactor;
				if(straightAssist && abs(pidDrive) > 20) {
					turningFactor = analogRead(ANALOG_GYRO) * 2; // NOTE: This needs to account for starting angle
					// Limit turning factor
					if (turningFactor > 20)
						turningFactor = 20;
					if (turningFactor < -20)
						turningFactor = -20;
					pidDriveLeft += turningFactor;
					pidDriveRight -= turningFactor;
				}

				if (abs(pidError) < 10) {
					int breakSpeed = 5 * (pidDrive > 0 ? -1 : 1);
					//stopSlewTask();
					driveRaw(breakSpeed,breakSpeed,breakSpeed,breakSpeed);
					delay(100);
					driveRaw(0,0,0,0);
					//startSlewTask();
					break;
					print("I finished brah");
					pidRunning = false;
					break;
				}

				// Linearize speeds
				pidDriveLeft = (pidDriveLeft > 0 ? 1 : -1) * getLerpedSpeed(abs(pidDriveLeft), 15, 0);
				pidDriveRight = (pidDriveRight > 0 ? 1 : -1) * getLerpedSpeed(abs(pidDriveRight), 15, 0);

        // Log gyro
        memset(&temp[0], 0, sizeof(temp));
        sprintf(temp, "%i", analogRead(ANALOG_GYRO));
        writeJINXData ("gyro", temp);
        // Log left drive
        memset(&temp[0], 0, sizeof(temp));
        sprintf(temp, "%i", pidDriveLeft);
        writeJINXData ("driveLeft", temp);
        // Log right drive
        memset(&temp[0], 0, sizeof(temp));
        sprintf(temp, "%i", pidDriveRight);
        writeJINXData ("driveRight", temp);

				driveRaw(pidDriveLeft, pidDriveLeft, pidDriveRight, pidDriveRight);
				} else if (pidMode == 1) { // Point turn
  				// Linearize speeds
  				if (abs(pidError) < 20) {
  					int breakSpeed = 65 * (pidError > 0 ? 1 : -1);
  					//stopSlewTask();
            driveRaw(breakSpeed, breakSpeed, breakSpeed*-1, breakSpeed*-1);
  					delay(100);
  					driveRaw(0,0,0,0);
  					//startSlewTask();
  					break;
  					//breakCount++;
				  }

				pidDrive = (pidDrive > 0 ? 1 : -1) * getLerpedSpeed(abs(pidDrive), 15, 0);

				driveRaw(pidDrive, pidDrive, pidDrive*-1, pidDrive*-1);
			}
		}
  	else
  		{
  			// clear all
  			pidError      = 0;
  			pidLastError  = 0;
  			pidIntegral   = 0;
  			pidDerivative = 0;
  		}

		delay( 50 );
	}

	driveRaw(0,0,0,0);
	pidRunning = false;
  pidTask = NULL;
}

void endPid() {
	if(pidTask != NULL)
    taskSuspend(pidTask);
}

void startPid() {
  if(pidTask == NULL)
    initPID();
  else {
      print("[ELib] Tried to start PID, loop already running!");
      return;
  }
}

// Increase Kp until constant oscillations
// Multiply by 0.8
// Increase Kd until oscillations stop
void pidDriveStraight(long ticksToMove) {
  print("[PID] Ich bin PID drive straight");
	straightAssist = true;
	pidRequestedValue = ticksToMove;
	pidMode = 0;
	pidSensor = -1;
	pidSensorOffset = 0;
	pidSensorScale = 1.00;
	kP = 0.5; //Proportional Gain
	kI = 0.0; //Integral Gain
	kD = 1.0; //Derivitive Gain
	kL = 50.0; //Integral Limit
  print("[PID] Ich bin starting PID back");
	startPid();
  printf("[PID] Ich bin running (pid straight)? %d",pidRunning);
}

void pidDrivePoint(long ticksToMove) {
	straightAssist = false;
	pidRequestedValue = ticksToMove;
	pidMode = 1;
	pidSensor = -1;
	pidSensorOffset = 0;
	pidSensorScale = 1.00;
	kP = 2.45; //Proportional Gain
	kI = 0.0; //Integral Gain
	kD = 0.8; //Derivitive Gain
	kL = 50.0; //Integral Limit
	startPid();
}

// breaks out if PID taking too long
void pidDriveStraightLimit(long ticksToMove, int termLimit) {
	pidDriveStraight(ticksToMove);
	int timeRunning = 0;
	while(pidRunning) {
		delay(10);
		timeRunning+=10;
		if(timeRunning > termLimit) {
			endPid();
			pidRunning = false;
			driveRaw(0,0,0,0);
			break;
		}
	}
}

// breaks out if PID taking too long
void pidDrivePointLimit(long ticksToMove, int termLimit) {
	pidDrivePoint(ticksToMove);
	int timeRunning = 0;
	while(pidRunning) {
		delay(10);
		timeRunning+=10;
		if(timeRunning > termLimit) {
			endPid();
			pidRunning = false;
			driveRaw(0,0,0,0);
			break;
		}
	}
}

void waitForPid() {
	while (pidRunning)
		delay(10);
}

void waitForPidLimit(int termLimit) {
	int timeRunning = 0;
	while(pidRunning) {
		delay(10);
		timeRunning+=10;
		if(timeRunning > termLimit) {
			endPid();
			pidRunning = false;
			driveRaw(0,0,0,0);
			break;
		}
	}
}

void initPidControl() {
  pidTask = taskCreate(taskDrivePid, TASK_DEFAULT_STACK_SIZE, NULL,
                    TASK_PRIORITY_DEFAULT);
}

// TODO Add to header and add to end of auton
void killPid() {
  // TODO Communicate to the task to kill itself so it can free up it's memory
  if (pidTask != NULL)
    taskDelete(pidTask);
}
