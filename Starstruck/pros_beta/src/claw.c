#include "main.h"
#include "math.h"
#define CLAW_MISALIGN_THRESHOLD 150 // Difference we consider as misaligned
#define CLAW_CATCHUP_SPEED 30
#define CLAW_SPEED 127

TaskHandle claw;

//Converts an RPM value to a motor speed using the function f(x) = 11.431e^(0.0217x)
//For use with a torque geared motor
//PARAMETERS:
// float: The RPM to be converted to a motor speed
//RETURNS:
// int: The motor speed that would get the RPM given
int RPMToMotorClaw(float RPM){
	RPM = 11.431 * ((float)pow(2.71828,(RPM*0.0217)));
	RPM = (RPM > 127.0) ? 127.0 : ((RPM < -127.0) ? -127.0 : RPM);
	return ((int)round(RPM));
}

//Converts a motor speed to an RPM value using the function f(x) = 44.486ln(x) - 105.47
//For use with a torque geared motor
//PARAMETERS:
// int: The motor speed to be converted to an RPM value
//RETURNS:
// float: The RPM that a motor at the given speed should spin at
float motorToRPMClaw(int motorSpeed){
	return (motorSpeed<0) ? ((log(((abs(motorSpeed))))*44.486) - 105.47) : -1*((log(((abs(motorSpeed))))*44.486) - 105.47);
}

unsigned char getButtonDirection() {
  // 0 = no buttons pressed
  // 1 = close
  // 2 = open
  return 0;
}

void initClawControl(){
	claw = taskCreate(manageClaw, TASK_DEFAULT_STACK_SIZE,
    NULL, TASK_PRIORITY_DEFAULT);
}

void suspendClawControl(){
	taskSuspend(claw);
}

void resumeClawControl(){
	taskResume(claw);
}

void manageClaw(void * ignored){
	// Temporary bang bang until we have claw.c done
	while(true){
		const int misal = 30;
		int clawLastPosLeft;
		int clawLastPosRight;
		int clawPosLeft = encoderGet(getEncoderClawL());
		int clawPosRight = encoderGet(getEncoderClawR());
		clawPosRight *= -1; // so they both count up as they go out
		if (joystickGetDigital(1, 6, JOY_DOWN) || joystickGetDigital(2, 6, JOY_DOWN)) { // Opening, bigger is more open
			if (abs(clawPosLeft-clawPosRight) > misal) { // Misaligned
				motorSet(MOTOR_CLAW_L,(clawPosRight < clawPosLeft) ? 90 : 127);
				motorSet(MOTOR_CLAW_R,(clawPosRight > clawPosLeft) ? -90 : -127);
			} else { // We good
				motorSet(MOTOR_CLAW_L,127);
				motorSet(MOTOR_CLAW_R,-127);
			}
			clawLastPosLeft = 0;
			clawLastPosRight = 0;
		} else if (joystickGetDigital(1, 6, JOY_UP) || joystickGetDigital(2, 6, JOY_UP)) { // Closing, smaller more closed
			if (abs(clawPosLeft-clawPosRight) > misal) { // Misaligned
				motorSet(MOTOR_CLAW_L,(clawPosRight > clawPosLeft) ? -90 : -127);
				motorSet(MOTOR_CLAW_R,(clawPosRight < clawPosLeft) ? 90 : 127);
			} else { // We good
				motorSet(MOTOR_CLAW_L,-127);
				motorSet(MOTOR_CLAW_R,127);
			}
			clawLastPosLeft = 0;
			clawLastPosRight = 0;
		} else { // Sitting still
			if (clawLastPosLeft == 0) {
				clawLastPosLeft = clawPosLeft;
				clawLastPosRight = clawPosRight;
			}
			// If we're misaligning, counteract
			// Left claw
			if (abs(clawPosLeft-clawLastPosLeft) > misal) {
				motorSet(MOTOR_CLAW_L,clawPosLeft-clawLastPosLeft > 0 ? -50 : 50);
			} else {
				motorSet(MOTOR_CLAW_L,0);
			}
			// Right claw
			if (abs(clawPosRight-clawLastPosRight) > misal) {
				motorSet(MOTOR_CLAW_R,clawPosRight-clawLastPosRight > 0 ? 50 : -50);
			} else {
				motorSet(MOTOR_CLAW_R,0);
			}
		}
		delay(20);
	}
}
