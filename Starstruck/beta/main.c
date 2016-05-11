#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    powerExpander,  sensorAnalog)
#pragma config(Sensor, in2,    gyroMain,       sensorGyro)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           driveFL,       tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port3,           driveBL,       tmotorVex393_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port4,           driveFR,       tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port5,           driveBR,       tmotorVex393_MC29, openLoop, encoderPort, I2C_4)
#pragma config(Motor,  port6,           intakeL,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           intakeR,       tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c" // Competition includes [DO NOT TOUCH]
#include "drive.c" // Handles pid, joystick input, etc
#include "sensors.c" // General sensor methods
#include "auton.c" // Autonomous controller
#include "lcd.c" // LCD methods

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_BUILD 0
#define VERSION_TYPE "ALPHA"

const int song[3][35] = {{58,12,600},{58,12,1200},{58,12,1800},{54,9,2250},{61,3,2400},{58,12,3000},{54,9,3450},
	{61,3,3600},{58,24,4800},{65,12,5400},{65,12,6000},{65,12,6600},{66,9,7050},{61,3,7200},{57,12,7800},{54,9,8250},
	{61,3,8400},{58,24,9600},{70,12,10200},{58,9,10650},{58,3,10800},{70,12,11400},{69,9,11850},{68,3,12000},{64,3,12150},
	{66,3,12300},{67,6,12600},{59,6,12900},{59,6,13200},{52,12,13800},{59,9,14250},{62,3,14400},{61,0,14410},{61,3,14560},{60,3,14710}};

const int songLength = 35;

const int noteThreshold = 5;

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			 2616E Starstruck
//
// Welcome to the 2616E Starstruck program.
//
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

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			  Pre-Autonomous
//
// - Set up general stuff
// - Get rid of compiler errors we don't need to see
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton()
{
	bStopTasksBetweenModes = true; // Tasks stop when we need them to. Not doing this can be dangerous.
	bLCDBacklight = true; // Turn our LCD backlight on

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
// TODO Write what we [personally] do here
//
/////////////////////////////////////////////////////////////////////////////////////////

/*
// Autonomous control task
*/
task autonomous()
{
	runAuton(); // auton.c
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control
//
// TODO Write what we [personally] do here
//
/////////////////////////////////////////////////////////////////////////////////////////

/*
// Test song :D
//A task to get past robotc's limitation of 10 notes
//in the tone queue, and play a song from an array of values
*/
task playSong()
{
	int currentNote = 0;
	clearTimer(T1);
	while(currentNote<songLength){
		if(bSoundQueueAvailable){
			if(abs(time1[T1]-song[currentNote][2])<noteThreshold){
				playTone(song[currentNote][0],song[currentNote][1]);
				currentNote++;
			} else if(time1[T1]>song[currentNote][2]){
				currentNote++;
			}
		}
		wait1Msec(1);
	}
}

/*
// Driver control task
*/
task usercontrol()
{
	lcdInit(); // Starts all tasks for handling the lcd. Check lcd.c
	int lastForwardSpeed, lastTurnSpeed, lastStrafeSpeed = 0;
	float lastDirection = 0.0;
	startTask ( playSong );
	while (true)
	{

		//////////////////////////////
		// Controller handling start
		//////////////////////////////

		// Drive train (forward speed, turn speed, strafe speed)(forward multiplier, turn multiplier, stafe multiplier
		if(!vexRT[Btn6D]) { // Normal drive... Bottom left bumper
			lastForwardSpeed = vexRT[Ch3];
			lastTurnSpeed = vexRT[Ch1];
			lastStrafeSpeed = vexRT[Ch4];
			lastDirection = gyroToFloat(SensorValue[gyroMain]);
			driveWithLogic(lastForwardSpeed, lastTurnSpeed, lastStrafeSpeed, 1.0, 1.0, 1.0);
			} else { // Time for some CRS (Crazy rotational shit)
			// Here's where'd you use michael's program to find the degree target.
			// We'd then find some way to mess with the degrees put into CRS to also turn towards that number
			// ... This is why it's called CRS. It's crazy.
			driveWithCRS(lastForwardSpeed, lastStrafeSpeed, lastDirection, gyroToFloat(SensorValue[gyroMain]));
		}

		// TODO add encoders to keep the sides synchronized
		if (vexRT[Btn5U]) { // Upper right bumper
			motor[intakeL] = 127;
			motor[intakeR] = 127;
			} else if (vexRT[Btn5D]) { // Lower right bumper
			motor[intakeL] = -127;
			motor[intakeR] = -127;
		}

		//////////////////////////////
		// Controller handling end
		//////////////////////////////
	}
}
