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
  case 1:
    print("Ran auton one!\n");
    break;

  case 2:
    // Mogo Out
    // Drive forward
    // pLoopDriveStraight(imperialToTick(55.0), false, true);
    driveRaw(127, 127, 127, 127);
    delay(5000);
    driveRaw(0, 0, 0, 0);
    break;

  case 3:
    // Mogo Out
    motorSet(MOTOR_MOGO, 127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Drive forward
    // pLoopDriveStraight(imperialToTick(55.0), false, true);
    driveRaw(127, 127, 127, 127);
    delay(3300);
    driveRaw(0, 0, 0, 0);
    // Take in mogo
    motorSet(MOTOR_MOGO, -127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Realign
    pLoopTurnPoint(0);
    // Back up
    driveRaw(-127, -127, -127, -127);
    delay(2300);
    driveRaw(0, 0, 0, 0);
    // Rest
    delay(500);
    // Turn
    pLoopTurnPoint(180);
    // Mogo Out
    motorSet(MOTOR_MOGO, 127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Back up
    driveRaw(-127, -127, -127, -127);
    delay(2000);
    driveRaw(0, 0, 0, 0);
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
