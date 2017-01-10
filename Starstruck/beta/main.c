#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    powerExpander,  sensorAnalog)
#pragma config(Sensor, in2,    gyroMain,       sensorGyro)
#pragma config(Sensor, in3,    potLauncher,    sensorPotentiometer)
#pragma config(Sensor, dgtl1,  autonCont,      sensorTouch)
#pragma config(Sensor, dgtl2,  quadBR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl4,  quadBL,         sensorQuadEncoder)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           clawR,         tmotorVex393_HBridge, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port2,           driveFR,       tmotorVex393_MC29, openLoop, reversed, driveLeft)
#pragma config(Motor,  port3,           driveBR,       tmotorVex393_MC29, openLoop, reversed, driveLeft)
#pragma config(Motor,  port4,           liftR1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           liftYR23,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           liftYL23,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           liftL1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           driveFL,       tmotorVex393_MC29, openLoop, driveRight)
#pragma config(Motor,  port9,           driveBL,       tmotorVex393_MC29, openLoop, driveRight)
#pragma config(Motor,  port10,          clawL,         tmotorVex393_HBridge, openLoop, reversed, encoderPort, I2C_1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(125)

#include "Vex_Competition_Includes.c" // Competition includes [DO NOT TOUCH]
#include "sensors.c" // Sensor handling and math methods
#include "drive.c" // Handles pid, joystick input, etc
#include "auton.c" // Autonomous controller
#include "lcd.c" // LCD methods

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_BUILD 3
#define VERSION_TYPE "ALPHA"

#define LIFT_ACC_MOV_RANGE 30  // amount lift will move before it is held in place
#define LIFT_MIN_SPEED 30      // lowest speed that will turn on the lift
#define LIFT_MIN_HEIGHT 100    // lowest potentiometer value for the lift
#define LIFT_MAX_HEIGHT 1500   // highest potentiometer value for the lift
#define LIFT_SLOW_RANGE 100    // how close to the bounds the lift should slow down
#define LIFT_SLOW_MOD 0.8      // how much (0-1] that the lift should be slowed down at bounds

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			 2616E Starstruck
//
// Welcome to the 2616E Starstruck program.
//
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------//
/////////////////////////////////////////////////////////////////////////////////////////
//
// Pinmap: https://drive.google.com/open?id=1mSrSJ1BPUzoXAdo2x8ED-OwT-OZY7r5Y9P7FmuNDcLY
// Controller map: // TODO Add link
// Trello: https://trello.com/b/oXrKvfA7/2616e-starstruck
//
// - Ask Cameron if you need/want permission to any of the above sites
// - Please keep these up-to-date when you do any pins or buttons.
// - Make sure you notify builders if you change ports up, and drivers if buttons!
// - The maps above [hopefully] will hopefully be printed on every release version for reference
//   -> and stored in the team boxes.
//
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------//
/////////////////////////////////////////////////////////////////////////////////////////
// Timer distribution:
// - T1: Unassigned
// - T2: Unassigned
// - T3: Game time tracking
// - T4: LCD Hanlding
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------//
/////////////////////////////////////////////////////////////////////////////////////////
// Global TODO:
// - Replace data/unchanging variables to defitions
// - Comment bare code
// - Remove unused testing code
/////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Bot methods
//
// Methods for doing different actions on the robot, like launching, intaking, etc using
// our robot's sensor array
//
/////////////////////////////////////////////////////////////////////////////////////////

int startingAngle = 0;

/*
// Claw
*/

task manageClaw() {
	// Temporary bang bang until we have claw.c done
	while(true){
		int clawLastPosLeft;
		int clawLastPosRight;
		const int misal = 30;
		int clawPosLeft = getMotorEncoder(clawL);
		int clawPosRight = getMotorEncoder(clawR);
		clawPosRight *= -1; // so they both count up as they go out
		if (vexRT[Btn6D] || vexRT[Btn6DXmtr2]) { // Opening, bigger is more open
			if (abs(clawPosLeft-clawPosRight) > misal) { // Misaligned
				motor[clawL] = (clawPosRight < clawPosLeft) ? 90 : 127;
				motor[clawR] = (clawPosRight > clawPosLeft) ? -90 : -127;
			} else { // We good
				motor[clawL] = 127;
				motor[clawR] = -127;
			}
			clawLastPosLeft = 0;
			clawLastPosRight = 0;
		} else if (vexRT[Btn6U] || vexRT[Btn6UXmtr2]) { // Closing, smaller more closed
			if (abs(clawPosLeft-clawPosRight) > misal) { // Misaligned
				motor[clawL] = (clawPosRight > clawPosLeft) ? -90 : -127;
				motor[clawR] = (clawPosRight < clawPosLeft) ? 90 : 127;
			} else { // We good
				motor[clawL] = -127;
				motor[clawR] = 127;
			}
			clawLastPosLeft = 0;
			clawLastPosRight = 0;
		} else { // Sitting still
			if (clawLastPosLeft == 0) {
				clawLastPosLeft = clawPosLeft;
				clawLastPosRight = clawPosRight;
			}
			if(vexRT[Btn8D]) {
				clawLastPosLeft = 1;
				clawLastPosRight = 1;
			}
			// If we're misaligning, counteract
			// Left claw
			if (abs(clawPosLeft-clawLastPosLeft) > misal) {
				motor[clawL] = clawPosLeft-clawLastPosLeft > 0 ? -50 : 50;
			} else {
				motor[clawL] = 0;
			}
			// Right claw
			if (abs(clawPosRight-clawLastPosRight) > misal) {
				motor[clawR] = clawPosRight-clawLastPosRight > 0 ? 50 : -50;
			} else {
				motor[clawR] = 0;
			}
		}
		wait1Msec(20);
	}
}

/*
// Lift
*/

bool holdActive = false; // is the lift currently attempting to hold in place
int desiredHeight = 0;   // desired height for the hold task to hold at

void setLiftMotors(int speed){
  motor[liftR1] = motor[liftYR23] = speed;
  motor[liftL1] = motor[liftYL23] = speed * -1;
}

task holdLiftTask() {
	int currHeight;
  while(true){
    // get current potentiometer value
    currHeight = SensorValue[potLauncher];
    //if the lift has moved too far from the desired position
    if (abs(currHeight-desiredHeight) > LIFT_ACC_MOV_RANGE) {
      // set the motors to move up if below target, or down if above target
      setLiftMotors(currHeight<desiredHeight ? 50 : -50);
    } else {
      // hold the lift in it's current position
      setLiftMotors(0);
    }
  wait1Msec(25);
	}
}

// Setup the lift to hold the position of it's current potentiometer value
void lockLift(){
  if(!holdActive){
    holdActive = true;
    desiredHeight = SensorValue[potLauncher];
    startTask(holdLiftTask);
  }
}

// Stop the lift from holding the position of it's current potentiometer value
void unlockLift(){
  if(holdActive){
    stopTask(holdLiftTask);
    holdActive = false;
  }
}

// Sets the lift motors with a range of speed, and can auto override the hold
// (WARNING: If you wish to reinstate a hold afterwards you must call lockLift().
// This function will not automatically reset the hold when you are done, if you
// wish to do so, it only will automatically disable the hold)
// PARAMS:
//  int: the speed to set to the motors
//  bool: whether or not to turn off the hold if it is active
//  bool: whether or not to slow down the lift at it's bounds
void moveLiftWithLogic(int speed, bool overrideHold, bool dampenSpeed){
  // if the speed is too low, set it to zero
  if(abs(speed) < LIFT_MIN_SPEED){
    speed = 0;
  }
  //unlock lift if it was locked
  if(holdActive && overrideHold){
    unlockLift();
  } else if(holdActive) { // if the hold is active, but can't override, do nothing
    return;
  }

  int currHeight = SensorValue[potLauncher];
  // if moving up
  if(speed>0) {
    // if within the slow zone, dampen the speed (if feature on)
    if(dampenSpeed && (abs(LIFT_MAX_HEIGHT-currHeight) < LIFT_SLOW_RANGE)){
      speed *= LIFT_SLOW_MOD;
    }
    // if greater than max and trying to go up, shut off
    if(currHeight > LIFT_MAX_HEIGHT){
      speed = 0;
    }
  } else { //if moving down
    // if within the slow zone, dampen the speed
    if(dampenSpeed && (abs(LIFT_MIN_HEIGHT-currHeight) < LIFT_SLOW_RANGE)){
      speed *= LIFT_SLOW_MOD;
    }
    // if less than min and trying to go down, shut off
    if(currHeight < LIFT_MIN_HEIGHT){
      speed = 0;
    }
  }

  // set the lift motors to the desired speed
  setLiftMotors(speed);
}

// Intended for autonomous, waits until the lift has fully raised or lowered.
// Which one it waits for depends on the direction of the motion.
// PARAMS:
//  int: the speed to set the motors (also +/- indicates direction (see above))
void waitForLift(int speed){
  // set the lift to the speed, and turn on inertial dampening, but only
  // if this will NOT slow it too much to even move
  moveLiftWithLogic(speed,true,(speed>=ceil(LIFT_MIN_SPEED/LIFT_SLOW_MOD)));
  // if the lift is being raised
  if(speed>=0){
    // wait until the lift is at it's max value
    while(SensorValue[potLauncher] < LIFT_MAX_HEIGHT){
      moveLiftWithLogic(speed,true,(speed>=ceil(LIFT_MIN_SPEED/LIFT_SLOW_MOD))); // the speed must be reset for the auto slow down to work
      wait1Msec(25);
    }
  } else { // if the lift is being lowered
    // wait until the lift is at it's min value
    while(SensorValue[potLauncher] > LIFT_MIN_HEIGHT){
      moveLiftWithLogic(speed,true,(speed>=ceil(LIFT_MIN_SPEED/LIFT_SLOW_MOD))); // the speed must be reset for the auto slow down to work
      wait1Msec(25);
    }
  }

  setLiftMotors(0);

}

// Sets up all necessary tasks and instance data for the lift
void initLift(){
	startTask(holdLiftTask);
  unlockLift();
  holdActive = false;
  desiredHeight = SensorValue[potLauncher];
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			  Pre-Autonomous
//
// Gets rid of useless errors, does general setup.
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton()
{
	bStopTasksBetweenModes = true; // Tasks stop when we need them to. Not doing this can be dangerous.
	bLCDBacklight = true; // Turn our LCD backlight on

	// Reset the motor encoders
	clearDriveEncoders();

	// Clear datalog incase we are using it
	clearDebugStream();
	datalogClear();

	// Set the arm position
	startingAngle = SensorValue[potLauncher];

	//Completely clear out any previous sensor readings by setting the port to "sensorNone"
	 SensorType[in2] = sensorNone;
	 wait1Msec(1000);
	 //Reconfigure Analog Port 8 as a Gyro sensor and allow time for ROBOTC to calibrate it
	 SensorType[in2] = sensorGyro;
	 wait1Msec(2000);

	// Never passing if statement. Lets us get rid of compile warnings so we can focus on the ones we need to see.
	if (false) {
		UserControlCodePlaceholderForTesting();
		AutonomousCodePlaceholderForTesting();
		sensorsErrorEscape(); // Get rid of unused methods for the sensor lib
		// Feel free to add currently un-used methods/tasks here if you're sure they are meant to not be used.
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous
//
// Runs the auton selector in auton.c
//
/////////////////////////////////////////////////////////////////////////////////////////

/*
// Autonomous control task
*/
task autonomous()
{
	setupGameTimer(false); // sensors.c
	runAuton(); // auton.c
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control
//
// Handle controls from the user, run the LCD control, and generally allow user control of
// the robot.
//
/////////////////////////////////////////////////////////////////////////////////////////

/*
// Driver control task
*/
task usercontrol()
{

	setupGameTimer(true); // sensors.c

	// Inits for normal games and skills. See below for specifics
	lcdInit(); // Starts all tasks for handling the lcd. Check lcd.c

	/*
	// Claw
	*/
	startTask(manageClaw);

	bool isSkills = false; // TODO: Set this somehow (LCD?)
	if (isSkills) {
		writeDebugStreamLine("Running Driver Control [Skills]...");
		} else {
		writeDebugStreamLine("Running Driver Control [Normal]...");
	}

	while (true)
	{
		//////////////////////////////
		// Controller handling start
		//////////////////////////////

		/*
		// Drive
		*/
		int speedForward = round(vexRT[Ch3]);
		int speedTurn = round(vexRT[Ch1]);
		int speedStrafe = round(vexRT[Ch4]);
		driveWithLogic(speedForward, speedTurn, speedStrafe, false);


		/*
		// Claw
		*/

		/*
		// Lift
		*/
		// Move the lift, using the buttons 5U (Up), 5D (Down), and 7D(Lock)
		if (vexRT[Btn5U] || vexRT[Btn5UXmtr2]) {
			moveLiftWithLogic(127, true, true);
		} else if (vexRT[Btn5D] || vexRT[Btn5DXmtr2]) {
			moveLiftWithLogic(-127, true, true);
		} else {
			moveLiftWithLogic(0,true,true);
			setLiftMotors(0);
		}

		if (vexRT[Btn7D] || vexRT[Btn7DXmtr2]) {
			lockLift();
		} else {
			unlockLift(); // If no buttons are pressed, simply unlock the lift
		}

		/*
		// Auton Tester
		*/
		if(vexRT[Btn7U]) {
			if(vexRT[Btn7D] && vexRT[Btn7R] && vexRT[Btn7L]) {
				stopTask(manageClaw);
				runAuton();
				startTask(manageClaw);
			}
		}

		//if (SensorValue[resetDriveEnc]) {
		//	clearDriveEncoders();
		//}

		//////////////////////////////
		// Controller handling end
		//////////////////////////////
	}
}
