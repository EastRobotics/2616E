#include "main.h"
#include "math.h"

#define IN_TO_MM (25.4)
#define MM_TO_IN (1 / 25.4)
#define PI acos(-1.0)
#define TICKS_PER_REV 360

float scale = 0;
float turnScale = 0;
float posX = 0;
float posY = 0;
float posTheta = 0;

int getOdomPosX() { return posX; }

int getOdomPosY() { return posY; }

int getOdomTheta() { return posTheta; }

void setOdomPosX(int x) { posX = x; }

void setOdomPosY(int y) { posY = y; }

void odomReset() {
  posX = 0;
  posY = 0;
  posTheta = 0;
}

void initOdomScale(float wheelDiam, float driveCircum, float wheelEncRatio) {
  scale = (wheelDiam * PI * IN_TO_MM * wheelEncRatio) /
          TICKS_PER_REV; // 1 in = 25.4 mm
  turnScale = (hypot(11.5, 5) / wheelDiam) /**1.76*/;
}

// Based off https://github.com/VTOW/BCI/tree/master/Modules odometry
void trackRobotPosition(void *param) {
  // Reset encoders
  encoderReset(getEncoderBL());
  encoderReset(getEncoderBR());

  float leftMM, rightMM, mm;
  int leftCurr, rightCurr;
  long lastLeft, lastRight, leftTicks, rightTicks;

  while (true) {
    // Save current quads
    leftCurr = encoderGet(getEncoderBL());
    rightCurr = encoderGet(getEncoderBR());

    // print("----------------------------------------\n");
    // printf("LCurr: %d\n", leftCurr);
    // printf("RCurr: %d\n", rightCurr);
    // printf("LLast: %ld\n", lastLeft);
    // printf("RLast: %ld\n", lastRight);
    // printf("turnScale: %f\n", turnScale);

    // Get delta angle
    leftTicks = leftCurr - lastLeft;
    rightTicks = rightCurr - lastRight;

    // Save last vals
    lastLeft = leftCurr;
    lastRight = rightCurr;

    // Convert to mm
    leftMM = (float)leftTicks * scale;
    rightMM = (float)rightTicks * scale;

    // printf("leftMM: %f\n", leftMM);
    // printf("rightMM: %f\n", rightMM);

    // Get avg delta
    mm = (leftMM + rightMM) / 2.0;

    // Get theta
    // if ((rightMM-leftMM) != 0) {
    //  posTheta += (rightMM - leftMM) / turnScale; // May be broken
    //}
    posTheta = gyroGet(getGyro());
    // printf("posTheta: %f\n", posTheta);

    // Wrap theta
    // if (posTheta > 180)
    //  posTheta -= 360;
    // if (posTheta <= -180)
    //  posTheta += 360;

    float posThetaRad = posTheta * PI / 180;
    // Do the odom math
    // printf("xAdd: %f\n", mm * cos(posThetaRad));
    // printf("yAdd: %f\n", mm * sin(posThetaRad));
    posX -= mm * sin(posThetaRad);
    posY += mm * cos(posThetaRad);

    // print("----------------------------------------\n");

    delay(25); // Give some other tasks some time
  }
}
