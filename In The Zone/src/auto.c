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

void pLoopDriveStraight(int tickDiff, bool correctBackwards, bool correctDir) {
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
    speedModif = (correctDir) ? angleOffset * KP_GYRO_CORRECT : 0;

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

  driveRaw(-10, -10, -10, -10); // Slam the breaks
  delay(10);
  driveRaw(0, 0, 0, 0);
}

void pLoopTurnPoint(int angleTarget) {
  int error;         // error in current position
  int lastError = 0; // error from last iteration of the loop
  int speed;         // speed for the motors to run at
  int stopCount = 0; // Amount of time spent within threshold
  int iterations = 0;
  while (iterations++ < 263) {
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
