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

void initOdomScale(float wheelDiam, float driveCircum) {
  scale = (wheelDiam * PI * IN_TO_MM) / TICKS_PER_REV; // 1 in = 25.4 mm
  turnScale = driveCircum / wheelDiam;
}

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

    // Get delta angle
    leftTicks = leftCurr - lastLeft;
    rightTicks = rightCurr - lastRight;

    // Save last vals
    lastLeft = leftCurr;
    lastRight = rightCurr;

    // Convert to mm
    leftMM = (float)leftTicks * scale;
    rightMM = (float)rightTicks * scale;

    // Get avg delta
    mm = (leftMM + rightMM) / 2.0;

    // Get theta
    posTheta += (rightTicks - leftTicks) / turnScale; // May be broken 3/27

    // Wrap theta
    if (posTheta > 180)
      posTheta -= 360;
    if (posTheta <= -180)
      posTheta += 360;

    // Do the odom math
    posX += mm * cos(posTheta);
    posY += mm * sin(posTheta);

    delay(5); // Give some other tasks some time
  }
}
