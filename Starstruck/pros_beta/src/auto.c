#include "main.h"
#include "math.h"

// KEEP IN MIND, AFTER AUTON, IF WE'RE PLUGGED IN SOME SENSORS WILL SHUT DOWN
// DON'T BE DUMB AND FORGET THIS UNTIL A COMPETITION. THAT WOULD BE BAD.
// From: Cameron, To: Cameron.

double KP = 2.0;
double KI = 0.04;
double KD = 0.0;

// Gets the encoder value of a specific motor *without parameters*
// To be used in PID
double getEncoderValue(){
  return encoderGet(getEncoderBR());
}

// Setter from the motor speed PID loop with *double parameter*
void setMotorSpeedPID(double speed) {
  int speedInt = round(speed);
  driveRaw(speedInt,speedInt,speedInt,speedInt);
}

// Shuts off motors *without parameters* to be used with PID Loop
void shutDownMotors() {
  driveRaw(0,0,0,0);
}


void autonomous() {
  initPID();
  setClawMode(1); // Give auton claw control

  switch (getAutonMode()) {
    case 1:
      print("Ran auton one, so nothing is happening");
      break;
    case 2:
      print("Ran auton two!");
      addPIDLoop(getEncoderValue,setMotorSpeedPID,
        shutDownMotors,KP,KI,KD,50.0,12.0);
      startPIDLoop(0, 500.0);
      break;
    case 3:
      print("Ran auton three!");
      setClawTarget(350);
      waitForClaw();
      clawClose(1000);
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
