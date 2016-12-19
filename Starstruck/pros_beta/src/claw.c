#include "main.h"
#include "math.h"
#define CLAW_MISALIGN_THRESHOLD 150 // Difference we consider as misaligned
#define CLAW_CATCHUP_SPEED 30
#define CLAW_SPEED 127

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

void manageClaw(void * ignored){
  bool lastDirection = false; //false is close, true is open

  while(true) {
    // Find the ROC of each claw side
    float rpmPlaceholder = 0.0;

		int clawPosLeft;
		imeGet (IME_CLAW_L, &clawPosLeft);
		int clawPosRight;
		imeGet (IME_CLAW_R, &clawPosRight);

    int targetSpeed;
    // Figure out which direction we're going
    if(getButtonDirection() != 0) { // A button is being pressed
      lastDirection = getButtonDirection() == 2; // True for open, false close
      if (lastDirection) // Opening
        targetSpeed = 127;
      else // Closing
        targetSpeed = -127;
    } else { // A button is not being pressed
      targetSpeed = 0; // Nothing
    }

    unsigned char master;
    unsigned char slave;

    // Set master and slave based on direction and position
    if (lastDirection) { // If we were last opening
      master = (clawPosLeft > clawPosRight) ? IME_CLAW_L :
        IME_CLAW_R; // Set master to the most open
      slave = (clawPosLeft > clawPosRight) ? IME_CLAW_R :
        IME_CLAW_L; // Set slave to the most closed
    } else { // If we were last closing
      master = (clawPosLeft < clawPosRight) ? IME_CLAW_L :
        IME_CLAW_R; // Set master to the most closed
      slave = (clawPosLeft < clawPosRight) ? IME_CLAW_R :
        IME_CLAW_L; // Set slave to the most open
    }

    // Rename left and right positions to master and slave
    int clawPosMaster = (master == IME_CLAW_L) ? clawPosLeft :
      clawPosRight; // Set master position to the approriate position
    int clawPosSlave = (slave == IME_CLAW_L) ? clawPosLeft :
      clawPosRight; // Set slave position to the approriate position

    // Set the master speed to the slave's
    int masterSpeed = RPMToMotorClaw(rpmPlaceholder); // TODO Replace with slave RPM
     // If they're considered misaligned let the slave catch up
    if (abs(clawPosMaster-clawPosSlave) > CLAW_MISALIGN_THRESHOLD) {
      masterSpeed -= CLAW_CATCHUP_SPEED; // TODO Account for direction, should slow down
    }

    // Set the claws to their appropriate speeds
    motorSet(MOTOR_CLAW_L, (master == IME_CLAW_L) ?
      masterSpeed : targetSpeed);
    motorSet(MOTOR_CLAW_R, (master == IME_CLAW_L) ?
      masterSpeed : targetSpeed);

    //hold claw at certain position
      //make sure claw is forward (not askew)
      //watch for error, and correct it if it occurs
    delay(20);
  }
}
