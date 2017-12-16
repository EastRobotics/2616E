#include "main.h"
#include "math.h"

#define KP_DRIVE 0.1
#define KP_DRIVE_TURN 0.80
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
  motorSet(MOTOR_CLAW, 100);
  delay(150);
  motorSet(MOTOR_FOUR_BAR, 100);
  delay(300);
  motorSet(MOTOR_CLAW, -100);
  delay(150);
  motorSet(MOTOR_FOUR_BAR, 10);
  delay(100);
  motorSet(MOTOR_CLAW, 0);
  motorSet(MOTOR_FOUR_BAR, -100);
  delay(350);
  motorSet(MOTOR_FOUR_BAR, 10);
  driveRaw(50, 50, 50, 50);
  delay(150);
  motorSet(MOTOR_CLAW, -100);
  delay(200);
  driveRaw(0, 0, 0, 0);
  delay(350);
  motorSet(MOTOR_FOUR_BAR, 100);
  delay(350);
  motorSet(MOTOR_CLAW, -20);
  motorSet(MOTOR_FOUR_BAR, 10);
  motorSet(MOTOR_MOGO_L, 127);
  motorSet(MOTOR_MOGO_R, -127);
  delay(750);
  motorSet(MOTOR_MOGO_L, 0);
  motorSet(MOTOR_MOGO_R, 0);
  delay(250);
  motorSet(MOTOR_LIFT_1, 127);
  motorSet(MOTOR_LIFT_2, -127);
  delay(250);
  motorSet(MOTOR_LIFT_1, 0);
  motorSet(MOTOR_LIFT_2, 0);
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
  int speed;                                  // Calculated speed to drive at
  int stopCount = 0;                 // Amount of time spent within threshold
  int initGyro = gyroGet(getGyro()); // Initial value of the gyro
  int speedModif = 0;                // How much to modify the speed for angle
  int angleOffset; // How much the robot is curving in its motion

  while (true) {
    errorL = tickDiff - (encoderGet(getEncoderBL()) - leftInit);
    errorR = tickDiff - (encoderGet(getEncoderBR()) - rightInit);
    error = errorL; // round((errorL + errorR) / 2);
    speed = error * KP_DRIVE;
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

    delay(20);
  }

  driveRaw(-speed, -speed, -speed, -speed); // Slam the breaks
  delay(10);
  driveRaw(0, 0, 0, 0);
}

void pLoopTurnPoint(int angleTarget) {
  int error;         // error in current position
  int speed;         // speed for the motors to run at
  int stopCount = 0; // Amount of time spent within threshold
  while (true) {
    error = angleTarget - gyroGet(getGyro());
    speed = error * KP_DRIVE_TURN;
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
    deploy();
    motorSet(MOTOR_FOUR_BAR, 100);
    delay(100);
    motorSet(MOTOR_MOGO_L, -127);
    motorSet(MOTOR_MOGO_R, 127);
    delay(500);
    motorSet(MOTOR_MOGO_L, 0);
    motorSet(MOTOR_MOGO_R, 0);
    // while (digitalRead(DIGITAL_LIM_CLAW))
    //  delay(10);
    // TODO UNCOMMENT ABOVE CODE!!!
    motorSet(MOTOR_FOUR_BAR, 10);
    motorSet(MOTOR_LIFT_1, 127);
    motorSet(MOTOR_LIFT_2, -127);
    // while (encoderGet(getEncoderLift()) < 1500)
    //  delay(10);
    // TODO UNCOMMENT ABOVE CODE
    motorSet(MOTOR_LIFT_1, 0);
    motorSet(MOTOR_LIFT_2, 0);
    motorSet(MOTOR_FOUR_BAR, 100);
    // while (digitalRead(DIGITAL_LIM_CLAW))
    //  delay(10);
    // TODO UNCOMMENT ABOVE CODE
    motorSet(MOTOR_FOUR_BAR, 10);
    motorSet(MOTOR_MOGO_L, -127);
    motorSet(MOTOR_MOGO_R, 127);
    delay(150);
    motorSet(MOTOR_MOGO_L, 0);
    motorSet(MOTOR_MOGO_R, 0);
    pLoopDriveStraight(550, true);
    delay(250);
    motorSet(MOTOR_CLAW, 100);
    delay(100);
    motorSet(MOTOR_CLAW, 0);
    // Drive half way back
    pLoopDriveStraight((50 - (encoderGet(getEncoderBL()))) / 2, true);
    pLoopTurnPoint(135);
    pLoopDriveStraight(650, true);
    pLoopTurnPoint(90);
    break;
  case 3:
    print("Ran auton three!\n");
    motorSet(MOTOR_FOUR_BAR, 20);
    setLiftTarget(800);
    waitForLift();
    delay(1000);
    break;
  case 4:
    print("Ran auton four!\n");
    addPIDLoop(initDrivePID, getEncoderValue, setMotorSpeedPID, shutDownMotors,
               0.5, 0.0, 0.0, 50.0, 12.0);
    startPIDLoop(0, 500.0);
    delay(5000);
    waitForPID(0);
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
