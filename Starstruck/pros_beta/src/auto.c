#include "main.h"
#include "math.h"

// KEEP IN MIND, AFTER AUTON, IF WE'RE PLUGGED IN SOME SENSORS WILL SHUT DOWN
// DON'T BE DUMB AND FORGET THIS UNTIL A COMPETITION. THAT WOULD BE BAD.
// From: Cameron, To: Cameron.

#define WHITE_THRESH 500 // Value to accept of the light sensor as the white line
#define LINE_BREAK_TIME 75 // Milliseconds for the brake to last for on line up

double KP = 2.0;
double KI = 0.04;
double KD = 0.0;

// Gets the encoder value of a specific motor *without parameters*
// To be used in PID
double getEncoderValue() { return encoderGet(getEncoderBR()); }

// Setter from the motor speed PID loop with *double parameter*
void setMotorSpeedPID(double speed) {
  int speedInt = round(speed);
  driveRaw(speedInt, speedInt, speedInt, speedInt);
}

bool isWhite(bool right){
  return (right ? analogRead(ANALOG_LINE_RIGHT) : analogRead(ANALOG_LINE_LEFT)) < WHITE_THRESH;
}

void driveToLine(int speed, bool forwards) {
  speed = abs(speed) * (forwards ? 1 : -1);
  int speedLeft = speed, speedRight = speed;
  int rightSightings = 0, leftSightings = 0;
  int rightBrakeCount = 0, leftBrakeCount = 0;
  const int delayRate = 15;
  while(!(isWhite(true) && isWhite(false))) {
    if(isWhite(true)) {
      if(((sameSign(speedRight,speed) && ((rightSightings % 2==0))) || ((!sameSign(speedRight,speed)) && (rightSightings % 2==1)))){
        speedRight = speed * ((rightSightings % 2 == 0) ? -0.75 : 0.75);
        rightBrakeCount = delayRate;
        rightSightings++;
      } else if(rightBrakeCount >= LINE_BREAK_TIME) {
        speedRight = 0;
        rightBrakeCount = 0;
      } else {
        rightBrakeCount += delayRate;
      }
    } else {
      speedRight = ((rightSightings % 2==0) ? speed : speed*-1);
      speedRight = (rightSightings > 0 ? speedRight/2 : speedRight);
      if(rightBrakeCount >= LINE_BREAK_TIME) {
        speedRight = 0;
        rightBrakeCount = 0;
      }
      if(!(((sameSign(speedRight,speed) && ((rightSightings % 2==0))) || ((!sameSign(speedRight,speed)) && (rightSightings % 2==1)))))
        rightBrakeCount++;
    }

    if(isWhite(false)) {
      if(((sameSign(speedLeft,speed) && ((leftSightings % 2==0))) || ((!sameSign(speedLeft,speed)) && (leftSightings % 2==1)))){
        speedLeft = speed * ((leftSightings % 2 == 0) ? -0.75 : 0.75);
        leftBrakeCount = delayRate;
        leftSightings++;
      } else if(leftBrakeCount >= LINE_BREAK_TIME) {
        speedLeft = 0;
        leftBrakeCount = 0;
      } else {
        leftBrakeCount += delayRate;
      }
    } else {
      speedLeft = ((leftSightings % 2)==0 ? speed : speed*-1);
      speedLeft = (leftSightings > 0 ? speedLeft/2 : speedLeft);
      if(leftBrakeCount >= LINE_BREAK_TIME) {
        speedLeft = 0;
        leftBrakeCount = 0;
      }
      if(!(((sameSign(speedLeft,speed) && ((leftSightings % 2==0))) || ((!sameSign(speedLeft,speed)) && (leftSightings % 2==1)))))
        leftBrakeCount++;
    }

    driveRaw(speedLeft,speedLeft,speedRight,speedRight);

    delay(delayRate);
  }
  driveRaw(0, 0, 0, 0);
}

// Shuts off motors *without parameters* to be used with PID Loop
void shutDownMotors() { driveRaw(0, 0, 0, 0); }

void autonomous() {
  initPID();
  setClawMode(1); // Give auton claw control

  switch (getAutonMode()) {
  case 1:
    print("Ran auton one, so nothing is happening");
    break;
  case 2:
    print("Ran auton two!");
    //addPIDLoop(getEncoderValue, setMotorSpeedPID, shutDownMotors, KP, KI, KD,
    //           50.0, 12.0);
    //startPIDLoop(0, 500.0);
    pidDriveStraight(2000);
    waitForPid();
    break;
  case 3:
    print("Ran auton three!");
    setClawTarget(350);
    waitForClaw();
    clawClose(1000);
    break;
  case 4:
    print("Ran auton four!");
    driveToLine(60,true);
    break;
  default:
    print("Ran auton that wasn't given a case!");
  }

  shutdownPID();
  stopClaw(); // Set the claw to the current position
  setClawMode(0);
  // Should be run after all autons
  if (isOnline()) {
    gyroShutdown(getGyro()); // Disable our gyro
  }
}
