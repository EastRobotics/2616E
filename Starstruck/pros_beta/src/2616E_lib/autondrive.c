#include "main.h"
#include "math.h"

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
