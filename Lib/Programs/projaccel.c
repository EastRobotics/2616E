#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    powerExpander,  sensorAnalog)
#pragma config(Sensor, in2,    gyroMain,       sensorGyro)
#pragma config(Sensor, in3,    potArm,         sensorPotentiometer)
#pragma config(Sensor, dgtl1,  autonCont,      sensorTouch)
#pragma config(Sensor, dgtl2,  quadBR,         sensorQuadEncoder)
#pragma config(Sensor, dgtl4,  quadBL,         sensorQuadEncoder)
#pragma config(Sensor, dgtl10, liftLimit,      sensorNone)
#pragma config(Sensor, dgtl11, quadDriveBL,    sensorQuadEncoder)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           clawR,         tmotorVex393_HBridge, openLoop, reversed, encoderPort, dgtl2)
#pragma config(Motor,  port2,           driveFR,       tmotorVex393_MC29, openLoop, reversed, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port3,           driveBR,       tmotorVex393_MC29, openLoop, reversed, driveLeft)
#pragma config(Motor,  port4,           liftR1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           liftYR23,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           liftYL23,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           liftL1,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           driveFL,       tmotorVex393_MC29, openLoop, driveRight)
#pragma config(Motor,  port9,           driveBL,       tmotorVex393_MC29, openLoop, driveRight, encoderPort, dgtl11)
#pragma config(Motor,  port10,          clawL,         tmotorVex393_HBridge, openLoop, reversed, encoderPort, dgtl4)

#pragma platform(VEX2)
#pragma competitionControl(Competition)

#include "Vex_Competition_Includes.c"

int clawTargetLeft;
int clawTargetRight;
bool clawRunning = false;
int clawSpeed;

task adjustClawPosition() {
	int clawPosRight;
	int clawPosLeft;
	const int misal = 15;
	do { // Align the claws
		const int misal = 15;
		int localClawSpeed = clawRunning ? clawSpeed : 30;
		int clawPosLeft = getMotorEncoder(clawL);
		int clawPosRight = getMotorEncoder(clawR);
		//clawPosRight *= -1; // so they both count up as they go out
		clawPosRight *= -1;
		// If we're misaligning, counteract
		// Left claw
		if (abs(clawPosLeft-clawTargetLeft) > misal) {
			motor[clawL] = clawPosLeft-clawTargetLeft > 0 ? localClawSpeed : localClawSpeed * -1;
			} else {
			motor[clawL] = 0;
		}
		// Right claw
		if (abs(clawPosRight-clawTargetRight) > misal) {
			motor[clawR] = clawPosRight-clawTargetRight > 0 ? -1 * localClawSpeed : localClawSpeed;
			} else {
			motor[clawR] = 0;
		}
		wait1Msec(20);
		if (clawRunning)
			clawRunning = (abs(clawPosRight-clawTargetRight) > misal) || (abs(clawPosLeft-clawTargetLeft) > misal);
	} while // They aren't aliged
		(true); // Left claw not aligned
}

void stopClaw() {
	clawTargetRight = getMotorEncoder(clawL);
	clawTargetLeft = getMotorEncoder(clawR);
}

void stopClawTask(){
	stopTask(	adjustClawPosition );
}

void startClawTask() {
	startTask( adjustClawPosition );
}

void setClawSpeed(int speed) {
	clawSpeed = speed;
}

// Should really only be used for opening, see clawClose(int ms)
void setClaw(int target, int speed) {
	clawRunning = true;
	clawSpeed = speed;
	clawTargetLeft = target;
	clawTargetRight = target;
}

// Used for when we want to clamp an unknown orientation, and can't guarentee a certain close value
void clawClose(int ms, int speed) {
	setClaw(-100, speed); // Try and close the claw
	int timeGone = 0;
	while (timeGone < ms || clawRunning) { // While we still have time or haven't reached 0ms
		wait1Msec(10);
		timeGone += 10;
	}
	setClawSpeed(30);
}

// Wait for the claw to reach it's target
void waitForClaw() {
	while (clawRunning)
		wait1Msec(20);
}

void pre_auton() {
	bLCDBacklight = true;
}

task autonomous() {
}

task usercontrol() {
	startTask(adjustClawPosition);
	int clawTestTarg = 0;
	while (true) {
		clearLCDLine(0);
		clearLCDLine(1);
		//displayLCDNumber(0,0,vexRT[AccelX]);
		//displayLCDNumber(1,0,vexRT[AccelY]);
		int accX = vexRT[AccelY];
		if (abs(accX) > 20)
			clawTestTarg += accX/10;
		displayLCDNumber(0,0,clawTestTarg);
		displayLCDNumber(1,0,vexRT[AccelY]);
		setClaw(-1*clawTestTarg,60);
		wait1Msec(50);
  }
}
