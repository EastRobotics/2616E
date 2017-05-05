#include "main.h"
#include "math.h"

bool initialized = false;
int pidLoopIndex = -1;

void autoGyroTurn(unsigned int angleDifference, unsigned int speed,
                  bool right) {
  Gyro gyro = getGyro();
  int currentAngle = gyroGet(gyro);
  int targetAngle =
      currentAngle + (right ? angleDifference : angleDifference * -1);
  drive(0, speed * (right ? 1 : -1));
  while (right ? (currentAngle < targetAngle) : (currentAngle > targetAngle)) {
    currentAngle = gyroGet(gyro);
    delay(20);
  }
  drive(0, 0);
}

void autoGyroTurnToAngle(int targetAngle, unsigned int speed) {
  Gyro gyro = getGyro();
  int currentAngle = gyroGet(gyro);
  int difference = targetAngle - currentAngle;
  autoGyroTurn(abs(difference), speed, difference > 0);
}

void autoGyroTurnToAngleBounded(int targetAngle, unsigned int speed) {
  Gyro gyro = getGyro();
  int currentAngle = gyroGet(gyro);
  targetAngle = targetAngle - (currentAngle % 360);
  autoGyroTurn(targetAngle, speed, targetAngle > currentAngle);
}

void autoDriveToPoint(int x, int y, int turnSpeed) {
  if(initialized) {
    // Turn to the desired point
    // when finding the angle it is important to remember 0 is forward
    //   (angle + 90) and right is negative (-1 * angle)
    int angle = round(-1 * (180 * atan2(y-getOdomPosY(),x-getOdomPosX()) / 3.1415926) + 90);
    printf("turn to angle: %d\n", angle);
    //autoGyroTurnToAngle(angle, turnSpeed);
    print("done turning\n");

    //Drive to specified point
    int dist = distance(x,getOdomPosX(),y,getOdomPosY());
    printf("dist:%d\n",dist);
    startPIDLoop(pidLoopIndex, dist);
    waitForPID(pidLoopIndex);
  } else {
    print("[ELib] Auto Drive was not yet initialized, please initialize before calling autoDriveToPoint");
  }
}

void initAutoDrive(int pidLoopIndex) {
  pidLoopIndex = pidLoopIndex;
  initialized = true;
}
