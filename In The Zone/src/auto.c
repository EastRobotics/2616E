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
  /*TaskHandle intakeCont;      // intake control task
  TaskHandle liftCont;        // lift control task
  TaskHandle manipulatorCont; // manipulator control task

  intakeCont = taskCreate(intakeControl, TASK_DEFAULT_STACK_SIZE, NULL,
                          (TASK_PRIORITY_DEFAULT));*/
  /*liftCont = taskCreate(liftControl, TASK_DEFAULT_STACK_SIZE, NULL,
                        (TASK_PRIORITY_DEFAULT));*/
  /*manipulatorCont = taskCreate(manipulatorControl, TASK_DEFAULT_STACK_SIZE,
                               NULL, (TASK_PRIORITY_DEFAULT));*/

  /*
  ** Run auton
  */
  switch (getAutonMode()) {
  // This is probably our most complex auton - it strategically does nothing
  case 1:
    print("Ran auton one!\n");
    break;

  // 20 point & stationary goal
  case 2:
    break;

  // 20 point
  case 3:
    break;

  // 10 point & stationary goal
  case 4:
    break;

  // 10 point
  case 5:
    break;

  // Programming Skills
  case 6:
    break;

  // Backup - I think that means ram
  case 7:
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
