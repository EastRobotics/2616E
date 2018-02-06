#include "main.h"
#include "math.h"
// TODO Add parameters for sensor input -- currently our-code specific!

int linSpeed(int speedUnlin) { return getLerpedSpeed(speedUnlin, 0, 0); }

// Turn to a point based on gyroscopic target
// Test status:
//  Tested, works great.
void pLoopTurnPointRaw(int angleTarget, double p, double d, int thresh,
                       int threshCount) {
  int error;         // error in current position
  int lastError = 0; // error from last iteration of the loop
  int speed = 0;     // speed for the motors to run at
  int stopCount = 0; // Amount of time spent within threshold
  int iterations = 0;
  bprintf(1, "---\r\nang:%d\r\np:%f\r\nd:%f\r\nthresh%d\r\ncount:%d\r\n",
          angleTarget, p, d, thresh, threshCount);
  while (iterations++ < 263) {
    error = angleTarget - gyroGet(getGyro());
    speed = (error * p) + ((error - lastError) * d);
    speed = (abs(speed) > 127) ? (speed < 0) ? -127 : 127 : speed;
    speed = (abs(speed) < 30) ? (speed < 0) ? -25 : 25 : speed;
    speed = (speed); // Linearize speed

    driveRaw(-speed, -speed, speed, speed);
    fprintf(uart1, "speed: %d\r\n", speed);
    fprintf(uart1, "error: %d\r\n", error);

    if (abs(error) < thresh) {
      stopCount++;
      if (stopCount >= threshCount)
        break;
    }

    lastError = error;
    bprintf(1, "speed:%d\r\n", speed);
    delay(20);
  }

  driveRaw(speed, speed, -speed, -speed); // Slam the breaks
  delay(10);
  driveRaw(0, 0, 0, 0);
}

// Drive an amount of ticks as straight as possible
// Test status:
//  completely untested
void pLoopDriveStraightRaw(int tickDiff, bool correctBackwards, bool correctDir,
                           double pSpeed, double dSpeed, double pCorrect,
                           int thresh, int threshCount) {
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
    error = round((errorL + errorR) / 2); // errorL;
    speed = error * pSpeed + ((error - lastError) * dSpeed);
    speed = (abs(speed) > 127) ? (speed < 0) ? -127 : 127 : speed;
    speed = (abs(speed) < 25) ? (speed < 0) ? -25 : 25 : speed;

    angleOffset = gyroGet(getGyro()) - initGyro;
    speedModif = (correctDir) ? angleOffset * pCorrect : 0;

    // Set motors to linearized version of input speeds
    driveRaw(linSpeed(speed + speedModif), linSpeed(speed + speedModif),
             linSpeed(speed - speedModif), linSpeed(speed - speedModif));

    if (abs(error) < thresh) {
      stopCount++;
      if (stopCount >= threshCount || !correctBackwards)
        break;
    }

    lastError = error;
    delay(20);
  }

  driveRaw(-speed, -speed, -speed, -speed); // Slam the breaks
  delay(10);
  driveRaw(0, 0, 0, 0);
}

// Drive to a point based on odometric values
// Test status:
//  completely untested, however has tested method foundation
// So many parameters... rip
void autoDriveToPointRaw(int x, int y, bool driveCorrectBackwards,
                         bool driveCorrectDir, double drivePSpeed,
                         double driveDSpeed, double drivePCorrect,
                         int driveThresh, int driveThreshCount, double turnP,
                         double turnD, int turnThresh, int turnThreshCount) {
  // Turn to the desired point
  // when finding the angle it is important to remember 0 is forward
  //   (angle + 90) and right is negative (-1 * angle)
  int angle = round(
      -1 * (180 * atan2(y - getOdomPosY(), x - getOdomPosX()) / acos(-1.0)) +
      90);
  printf("turn to angle: %d\n", angle);
  pLoopTurnPointRaw(angle, turnP, turnD, turnThresh, turnThreshCount);
  print("done turning\n");

  // Drive to specified point
  double distMM = distance(x, getOdomPosX(), y, getOdomPosY());
  int distTicks = inchesToTicks(distMM / 25.4, 1);
  printf("dist:%d\n", distTicks);
  pLoopDriveStraightRaw(distTicks, driveCorrectBackwards, driveCorrectDir,
                        drivePSpeed, driveDSpeed, drivePCorrect, driveThresh,
                        driveThreshCount);
}
