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
    // Lift
    setLiftSpeed(127);
    delay(100);
    setLiftSpeed(0);
    // Mogo Out
    motorSet(MOTOR_MOGO, 127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Drive forward
    // pLoopDriveStraight(imperialToTick(55.0), false, true);
    driveStraightRaw(127, 2000);
    // Take in mogo
    motorSet(MOTOR_MOGO, -127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Realign
    pLoopTurnPoint(0);
    // Back up
    driveStraightRaw(-127, 1500);
    // Rest
    delay(250);
    // Turn
    pLoopTurnPoint(180 * (getAutonPosition() ? 1 : -1));
    // Approach Zone
    driveRaw(80, 80, 80, 80);
    delay(250);
    driveRaw(0, 0, 0, 0);
    // Mogo Out
    motorSet(MOTOR_MOGO, 127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Back up
    driveRaw(-80, -80, -80, -80);
    delay(100);
    driveRaw(0, 0, 0, 0);
    // wait a sec
    delay(500);
    // Ram the goal in just in case
    driveRaw(127, 127, 127, 127);
    delay(200);
    driveRaw(0, 0, 0, 0);
    // wait
    delay(50);
    // Back up
    driveStraightRaw(-127, 2000);
    break;

  case 4:
    pLoopDriveStraight(inchesToTicks(24, 2), true, true);
    // Will be more likely used, however above should be tuned first
    // pLoopDriveStraight(inchesToTicks(24, 1), false, true);
    break;

  case 5:
    pLoopTurnPoint(180);
    break;

  case 6:
    // Drive to 10,10, aka 6",6"
    autoDriveToPoint(152.4, 152.4, false, true);
    // Drive "forward" 2 feet
    autoDriveToPoint(152.4, 152.4 + 609.6, false, true);
    // Drive to 1.5 feet right from 6"6" start
    autoDriveToPoint(152.4 + 457.2, 152.4, false, true);
    // Drive back to 6"6" start
    autoDriveToPoint(152.4, 152.4, false, true);
    // Turn back to original angle
    pLoopTurnPoint(0);
    break;

  case 7:
    setIntakeTarget(1000); // FIX Extend Four Bar
    waitForIntake();
    setLiftTarget(1000); // FIX Raise Lift
    waitForLift();
    // CHECK Extend the Mogo
    motorSet(MOTOR_MOGO, 127);
    delay(750);
    motorSet(MOTOR_MOGO, 0);
    odomDriveForward(1500, false, true); // Ram the Mogo
    // CHECK Intake Mogo
    motorSet(MOTOR_MOGO, -127);
    delay(750);
    motorSet(MOTOR_MOGO, 0);
    odomDriveForward(-1450, false, true); // Back up
    pLoopTurnPoint(-200);                 // Turn to Zones
    // CHECK Release Mogo
    motorSet(MOTOR_MOGO, -127);
    delay(750);
    motorSet(MOTOR_MOGO, 0);
    break;
  case 8:
    // Lift
    setLiftSpeed(127);
    delay(100);
    setLiftSpeed(0);
    // Mogo Out
    motorSet(MOTOR_MOGO, 127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Drive forward
    // pLoopDriveStraight(imperialToTick(55.0), false, true);
    driveStraightRaw(127, 2000);
    // Take in mogo
    motorSet(MOTOR_MOGO, -127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Realign
    pLoopTurnPoint(0);
    // Back up
    driveStraightRaw(-127, 750);
    // Rest
    delay(250);
    // Turn
    pLoopTurnPoint(180 * (getAutonPosition() ? 1 : -1));
    // Wait for 3815C
    delay(2500);
    // Drive in a bit
    driveStraightRaw(60, 500);
    // Mogo Out
    motorSet(MOTOR_MOGO, 127);
    delay(1000);
    motorSet(MOTOR_MOGO, 0);
    // Back up
    driveStraightRaw(-127, 2000);
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
