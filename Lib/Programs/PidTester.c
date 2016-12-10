#pragma config(Motor,  port2,           driveFR,       tmotorVex393_MC29, openLoop, reversed, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port3,           driveBR,       tmotorVex393_MC29, openLoop, reversed, driveLeft, encoderPort, I2C_4)
#pragma config(Motor,  port8,           driveFL,       tmotorVex393_MC29, openLoop, driveRight, encoderPort, I2C_1)
#pragma config(Motor,  port9,           driveBL,       tmotorVex393_MC29, openLoop, driveRight, encoderPort, I2C_3)

#pragma platform(VEX2)
#pragma competitionControl(Competition)
#include "Vex_Competition_Includes.c"

#define PID_SENSOR_INDEX    myEncoder
#define PID_SENSOR_SCALE    1

#define PID_MOTOR_INDEX     myMotor
#define PID_MOTOR_SCALE     -1

#define PID_DRIVE_MAX       60
#define PID_DRIVE_MIN     (-40)

#define PID_INTEGRAL_LIMIT  50

float RPMValues[10] = {0,0,0,0,0,0,0,0,0,0};
long lastTickCount[10] = {0,0,0,0,0,0,0,0,0,0};
float RPMReadRate = 20.0;
long pidTarget = 1000;
float increments[] = {0.5,0.25, 0.1, 0.05, 0.025, 0.01};

void pre_auton()
{
	bStopTasksBetweenModes = true;
}

task autonomous()
{
	AutonomousCodePlaceholderForTesting();
}

void clearDriveEncoders() {
	nMotorEncoder[driveFR] = 0;
	nMotorEncoder[driveBR] = 0;
	nMotorEncoder[driveFL]= 0;
	nMotorEncoder[driveBL] = 0;
}

task getRPMValues() {
	while(true){
		long tickCount[10] = {0,nMotorEncoder[driveFR],nMotorEncoder[driveBR],0,0,0,0,nMotorEncoder[driveFL],nMotorEncoder[driveBL],0};
		for(int i = 0; i < 10; i++){
			RPMValues[i] = (((((float)tickCount[i])-((float)lastTickCount[i]))*(60000.0/RPMReadRate))/627.2); // Made for torque geared motors
			lastTickCount[i] = tickCount[i];
		}
		datalogDataGroupStart();
		datalogAddValue(0,RPMValues[1]);
		datalogAddValue(1,pidTarget);
		datalogAddValue(2,tickCount[1]);
		datalogAddValue(3,motor[driveFR]);
		datalogDataGroupEnd();

		wait1Msec((int)RPMReadRate);
	}
}

tMotor motorsToChange[4] = {driveFL,driveFR,driveBR,driveBL};
bool motorToMotorReverse[4] = {false, false, false, false};
//Setup the weights for the various stages of pid
float kP = 0.00; //Proportional Gain
float kI = 0.00; //Integral Gain
float kD = 0.00; //Derivitive Gain
float kL = 50.0; //Apparently this is there to be the integral limit, I think we missed it when working last time

task usercontrol()
{
	while (true)
	{
		float  pidSensorCurrentValue;
		float  pidError;
		float  pidLastError;
		float  pidIntegral;
		float  pidDerivative;
		float  pidDrive;
		clearDriveEncoders();
		pidLastError  = 0;
		pidIntegral   = 0;

		while( true )
		{
			// Read the sensor value and scale
			pidSensorCurrentValue = nMotorEncoder[driveFR] * PID_SENSOR_SCALE;

			// calculate error
			pidError = pidSensorCurrentValue - pidTarget;

			// integral - if Ki is not 0
			if( kI != 0 )
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
			pidDrive = (kP * pidError) + (kI * pidIntegral) + (kD * pidDerivative);
			pidDrive *= PID_MOTOR_SCALE;

			// limit drive
			if( pidDrive > PID_DRIVE_MAX )
				pidDrive = PID_DRIVE_MAX;
			if( pidDrive < PID_DRIVE_MIN )
				pidDrive = PID_DRIVE_MIN;

			string debug = "";
			sprintf(debug,"PIDS: %i",pidDrive);
			writeDebugStreamLine(debug);
			debug = "";
			sprintf(debug,"Error: %i",pidError);
			writeDebugStreamLine(debug);
			debug = "";
			sprintf(debug,"Targ: %i",pidTarget);
			writeDebugStreamLine(debug);

			// send to motor
			motor[ driveFL ] = pidDrive;
			motor[ driveFR ] = pidDrive;
			motor[ driveBL ] = pidDrive;
			motor[ driveBR ] = pidDrive;
			wait1Msec( 20 );
		}

		// clear all
		pidError      = 0;
		pidLastError  = 0;
		pidIntegral   = 0;
		pidDerivative = 0;
		motor[ driveFL ] = 0;
		clearDriveEncoders();
	}
}
