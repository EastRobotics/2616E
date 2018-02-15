#include "main.h"
#include "math.h"

// KEEP IN MIND, AFTER AUTON, IF WE'RE PLUGGED IN SOME SENSORS WILL SHUT DOWN
// DON'T BE DUMB AND FORGET THIS UNTIL A COMPETITION. THAT WOULD BE BAD.
// From: Cameron, To: Cameron.

double imperialToTick(double inches) {
  double conversion = (4 * acos(-1.0)) / (360);
  return inches / conversion;
}

///////////////////////// PID DUMMY FUNCTIONS //////////////////////////////////

// Setter from the motor speed PID loop with *double parameter*
void setMotorSpeedPID(double speed) {
  int speedInt = round(speed);
  driveRaw(speedInt, speedInt, speedInt, speedInt);
}

// Shuts off motors *without parameters* to be used with PID Loop
void shutDownMotors() { driveRaw(0, 0, 0, 0); }

// Gets the encoder value of a specific motor *without parameters*
// To be used in PID
double getEncoderValue() { return encoderGet(getEncoderBR()); }

///////////////////////// PID DUMMY FUNCTIONS //////////////////////////////////

void autonomous() {
  /*
  ** Set up
  */
  initPID(); // Start PID

  ensureLiftTaskRunning();
  ensureIntakeTaskRunning();

  /*
  ** Run auton
  */
  switch (getAutonMode()) {
  // This is probably our most complex auton - it strategically does nothing
  case 1:
    print("Ran auton one!\n");
    break;

  // "Light" (RAM)
  case 2: {
      // TODO Tune and check sides
      int speedLeftCurve = getAutonPosition() ? 60 : 127;
      int speedRightcurve = getAutonPosition() ? 127 : 60;
      driveRaw(speedLeftCurve, speedLeftCurve, speedRightcurve, speedRightcurve);
      delay(1000); // TODO Maybe do some gyro check instead?
      driveRaw(127, 127, 127, 127);
      delay(4000);
      driveRaw(0, 0, 0, 0);
    }
    break;

  // 20 point & stationary goal
  case 3:
    break;

  // 20 point
  case 4:
    break;

  // 10 point & stationary goal
  case 5:
    break;

  // 10 point
  case 6:
    break;

  // Programming Skills
  case 7:
    break;

  // Custom
  case 8:
    setLiftTarget(500);
    waitForLift();
    break;

  default:
    print("Ran auton that wasn't given a case!");
  }

  /*
  ** Clean up
  */
  if (isOnline()) {          // Shut down sensors if at a comp
    gyroShutdown(getGyro()); // Disable our gyro
  }
  // taskDelete(liftCont);
  // taskDelete(intakeCont);
  // taskDelete(manipulatorCont);

  shutdownPID(); // Kill PID
}
