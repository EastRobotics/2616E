#include "main.h"
#include "math.h"

#define KP_DRIVE 0.1
#define KD_DRIVE 0.1
#define KP_DRIVE_TURN 0.80
#define KD_DRIVE_TURN 0.1
#define KP_GYRO_CORRECT 10
#define P_LOOP_DRIVE_THRESHOLD 10
#define P_LOOP_DRIVE_TURN_THRESHOLD 3
#define P_LOOP_STOP_COUNT 25
#define P_LOOP_TURN_STOP_COUNT 25
#define P_LOOP_GYRO_STAIGHTEN_SPEED_MODIF 10
#define P_LOOP_GYRO_CORRECTION_ANGLE 2

// KEEP IN MIND, AFTER AUTON, IF WE'RE PLUGGED IN SOME SENSORS WILL SHUT DOWN
// DON'T BE DUMB AND FORGET THIS UNTIL A COMPETITION. THAT WOULD BE BAD.
// From: Cameron, To: Cameron.

void breakpoint() {
  while (digitalRead(DIGITAL_BREAKPOINT))
    delay(20);
}

void initDrivePID() {
  // TODO Implement
}

void deploy() {
  // Everything beautiful must die - Michael
  // TODO REMOVE in next update.  I just wanted a memorial for the deploy
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

void pLoopDriveStraight(int tickDiff, bool correctBackwards) {
  int leftInit = encoderGet(getEncoderBL());  // Initial left value
  int rightInit = encoderGet(getEncoderBR()); // Initial right value
  int errorL;                                 // Error in the left side
  int errorR;                                 // Error in the right side
  int error;                                  // Averaged Error
  int lastError = 0;                 // The Error from the Previous Iteration
  int speed;                         // Calculated speed to drive at
  int stopCount = 0;                 // Amount of time spent within threshold
  int initGyro = gyroGet(getGyro()); // Initial value of the gyro
  int speedModif = 0;                // How much to modify the speed for angle
  int angleOffset; // How much the robot is curving in its motion

  while (true) {
    errorL = tickDiff - (encoderGet(getEncoderBL()) - leftInit);
    errorR = tickDiff - (encoderGet(getEncoderBR()) - rightInit);
    error = errorL; // round((errorL + errorR) / 2);
    speed = error * KP_DRIVE + ((error - lastError) * KD_DRIVE);
    speed = (abs(speed) > 127) ? (speed < 0) ? -127 : 127 : speed;
    speed = (abs(speed) < 25) ? (speed < 0) ? -25 : 25 : speed;

    angleOffset = gyroGet(getGyro()) - initGyro;
    speedModif = angleOffset * KP_GYRO_CORRECT;

    driveRaw(speed + speedModif, speed + speedModif, speed - speedModif,
             speed - speedModif);

    if (abs(error) < P_LOOP_DRIVE_THRESHOLD) {
      stopCount++;
      if (stopCount >= P_LOOP_STOP_COUNT || !correctBackwards)
        break;
    }

    lastError = error;
    delay(20);
  }

  driveRaw(-speed, -speed, -speed, -speed); // Slam the breaks
  delay(10);
  driveRaw(0, 0, 0, 0);
}

void pLoopTurnPoint(int angleTarget) {
  int error;         // error in current position
  int lastError = 0; // error from last iteration of the loop
  int speed;         // speed for the motors to run at
  int stopCount = 0; // Amount of time spent within threshold
  while (true) {
    error = angleTarget - gyroGet(getGyro());
    speed = (error * KP_DRIVE_TURN) + ((error - lastError) * KD_DRIVE_TURN);
    speed = (abs(speed) > 127) ? (speed < 0) ? -127 : 127 : speed;
    speed = (abs(speed) < 30) ? (speed < 0) ? -25 : 25 : speed;

    driveRaw(-speed, -speed, speed, speed);
    fprintf(uart1, "speed: %d\r\n", speed);
    fprintf(uart1, "error: %d\r\n", error);

    if (abs(error) < P_LOOP_DRIVE_TURN_THRESHOLD) {
      stopCount++;
      if (stopCount >= P_LOOP_TURN_STOP_COUNT)
        break;
    }

    lastError = error;
    delay(20);
  }

  driveRaw(speed, speed, -speed, -speed); // Slam the breaks
  delay(10);
  driveRaw(0, 0, 0, 0);
}

void autonomous() {
  /*
  ** Set up
  */
  initPID(); // Start PID

  /*
  ** Run auton
  */
  switch (getAutonMode()) {
  case 1:
    print("Ran auton one!\n");

    break;
  case 2:
    print("Ran auton two!\n");
    bprint(1, "Ran auton two!");
    setLiftTarget(213); // Get lift out of way
    waitForLift();
    motorSet(MOTOR_MOGO, 127); // Send out mogo
    delay(1070);
    motorSet(MOTOR_MOGO, 0);
    pLoopDriveStraight(2590, false); // Drive to mogo
    motorSet(MOTOR_MOGO, -127);      // Grab mogo while driving forward
    driveRaw(127, 127, 127, 127);
    delay(200);
    driveRaw(0, 0, 0, 0);
    delay(1150);
    pLoopDriveStraight(-2120, true); // Drive back to zone
    pLoopTurnPoint(180);             // Turn to face zone
    pLoopDriveStraight(449, true);   // Drive closer to zone
    pLoopTurnPoint(225);             // Turn to face zone better
    motorSet(MOTOR_MOGO, 127);       // Extend mogo outward
    delay(1070);
    motorSet(MOTOR_MOGO, 0);
    delay(100);
    driveRaw(-127, -127, -127, -127); // Back up
    delay(300);
    driveRaw(0, 0, 0, 0);
    motorSet(MOTOR_MOGO, 127); // Lower mogo a bit more
    delay(200);
    motorSet(MOTOR_MOGO, 0);
    driveRaw(127, 127, 127, 127); // drive forwards a tad
    delay(400);
    driveRaw(0, 0, 0, 0);
    delay(100);
    driveRaw(-127, -127, -127, -127); // back away to not touch goal
    delay(500);
    driveRaw(0, 0, 0, 0);
    break;
  case 4:
    print("Ran auton four!\n");
    /*addPIDLoop(initDrivePID, getEncoderValue, setMotorSpeedPID,
    shutDownMotors,
               0.5, 0.0, 0.0, 50.0, 12.0);
    startPIDLoop(0, 500.0);
    delay(5000);
    waitForPID(0);*/
    // INTAKE TEST
    // setIntakeTarget(INTAKE_POS_AVOID);
    // waitForIntake();
    // delay(5000);
    // LIFT TEST
    /*setLiftTargetSmart(0);
    waitForLift();
    delay(1000);
    setLiftTargetSmart(1);
    waitForLift();
    delay(1000);
    setLiftTargetSmart(2);
    */
    // CLAW TEST
    /*closeClaw();
    waitForClaw();
    delay(1000);
    openClaw();
    waitForClaw();
    delay(1000);
    closeClaw();
    waitForClaw();
    delay(1000);
    openClaw();
    */
    // waitForIntake();
    // waitForLift();
    // waitForClaw();
    motorSet(MOTOR_CLAW, -127);
    delay(200);
    motorSet(MOTOR_CLAW, 0);
    driveRaw(127, 127, 127, 127);
    delay(5000);
    driveRaw(50, 50, 50, 50);
    delay(2000);
    driveRaw(0, 0, 0, 0);
    delay(1000);

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
  shutdownPID(); // Kill PID
}
