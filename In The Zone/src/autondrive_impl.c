#include "main.h"
#include "math.h"

// File to reduce arguments in calls to autondrive.c in elib
#define KP_DRIVE 0.1
#define KD_DRIVE 0.1
#define KP_GYRO_CORRECT 10
#define P_LOOP_DRIVE_THRESHOLD 10
#define P_LOOP_STOP_COUNT 25
// Appear to be unused:
// #define P_LOOP_GYRO_STAIGHTEN_SPEED_MODIF 10
// #define P_LOOP_GYRO_CORRECTION_ANGLE 2

void pLoopDriveStraight(int tickDiff, bool correctBackwards, bool correctDir) {
  pLoopDriveStraightRaw(tickDiff, correctBackwards, correctDir, KP_DRIVE,
                        KD_DRIVE, KP_GYRO_CORRECT, P_LOOP_DRIVE_THRESHOLD,
                        P_LOOP_STOP_COUNT);
}

#define KP_DRIVE_TURN 0.80
#define KD_DRIVE_TURN 0.1
#define P_LOOP_DRIVE_TURN_THRESHOLD 3
#define P_LOOP_TURN_STOP_COUNT 25

void pLoopTurnPoint(int angleTarget) {
  pLoopTurnPointRaw(angleTarget, KP_DRIVE_TURN, KD_DRIVE_TURN,
                    P_LOOP_DRIVE_TURN_THRESHOLD, P_LOOP_TURN_STOP_COUNT);
}

void autoDriveToPoint(int x, int y, bool driveCorrectBackwards,
                      bool driveCorrectDir) {
  autoDriveToPointRaw(x, y, driveCorrectBackwards, driveCorrectDir, KP_DRIVE,
                      KD_DRIVE, KP_GYRO_CORRECT, P_LOOP_DRIVE_THRESHOLD,
                      P_LOOP_STOP_COUNT, KP_DRIVE_TURN, KD_DRIVE_TURN,
                      P_LOOP_DRIVE_TURN_THRESHOLD, P_LOOP_TURN_STOP_COUNT);
}

void odomDriveForward(int millimeters, bool driveCorrectBackwards,
                      bool driveCorrectDir) {
  autoDriveToPoint(getOdomPosX() + (millimeters * cos(getOdomTheta())),
                   getOdomPosY() + (millimeters * sin(getOdomTheta())),
                   driveCorrectBackwards, driveCorrectDir);
}

void odomTurn(int degrees) { pLoopTurnPoint(getOdomTheta() + degrees); }

void odomTurnSB(int degrees) {
  pLoopTurnPoint(getOdomTheta() + (degrees * (getAutonPosition() ? 1 : -1)));
}

void driveStraightRaw(int speed, int timeToDrive) {
  int initGyro = gyroGet(getGyro());
  int angleOffset = 0;
  int speedModif = 0;
  int timeSpent = 0;
  while (timeSpent <= timeToDrive) {
    angleOffset = gyroGet(getGyro()) - initGyro;
    speedModif = angleOffset * KP_GYRO_CORRECT;
    driveRaw((speed + speedModif), (speed + speedModif), (speed - speedModif),
             (speed - speedModif));
    delay(50);
    timeSpent += 50;
  }
  driveRaw(0, 0, 0, 0);
}
