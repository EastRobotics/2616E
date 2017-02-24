#include "main.h"
#include "math.h"
#define MISAL_THRESH 15 // Difference we consider as misaligned

TaskHandle claw;

// 0 = no buttons pressed
// 1 = open
// 2 = close
unsigned char getButtonDirection() {
  // Opening
  if (joystickGetDigital(1, 6, JOY_DOWN) || joystickGetDigital(2, 6, JOY_DOWN))
    return 1;
  // Closing
  else if (joystickGetDigital(1, 6, JOY_UP) || joystickGetDigital(2, 6, JOY_UP))
    return 2;
  return 0;
}

void initClawControl() {
  claw = taskCreate(manageClaw, TASK_DEFAULT_STACK_SIZE, NULL,
                    TASK_PRIORITY_DEFAULT);
}

void suspendClawControl() { taskSuspend(claw); }

void resumeClawControl() { taskResume(claw); }

int clawMode = 0;
bool clawTargetAchieved = true;
int clawLastPosLeft = 0;
int clawLastPosRight = 0;

// 0 = user control
// 1 = target control
void setClawMode(int mode) {
  clawMode = mode;
  // Reset the claw target
  clawTargetAchieved = true;
}

void setClawTarget(int target) {
  clawLastPosLeft = clawLastPosRight = target;
  clawTargetAchieved = false;
}

// Wait for the claw to get to it's target
void waitForClaw() {
  while (!clawTargetAchieved) { // While we haven't reached target
    delay(20);                  // Wait
  }
}

// TODO: Adjust encoder init so both sides go up when out
void manageClaw(void *ignored) {
  while (true) {
    int clawPosLeft = encoderGet(getEncoderClawL());
    int clawPosRight = encoderGet(getEncoderClawR());
    // NOTE: Old clawLastPosLeft & clawLastPosRight location

    // Determine if target is achieved
    if (!clawTargetAchieved) // If target isn't achieved
      // Set achieved to !(left misal || right misal), so if both not misal,
      //   clawTargetAchieved will be true
      clawTargetAchieved =
          !((abs(clawPosRight - clawLastPosRight) > MISAL_THRESH) ||
            (abs(clawPosLeft - clawLastPosRight) > MISAL_THRESH));

    // Holding button and in user control
    int buttonDirection = getButtonDirection();
    printf("%i",buttonDirection);
    if (clawMode == 0 && buttonDirection != 0) {
      // Opening, bigger is more open
      if (buttonDirection == 1) {
        if (abs(clawPosLeft - clawPosRight) > MISAL_THRESH) { // Misaligned
          motorSet(MOTOR_CLAW_L, (clawPosRight < clawPosLeft) ? -90 : -127);
          motorSet(MOTOR_CLAW_R, (clawPosRight > clawPosLeft) ? 90 : 127);
        } else { // We good
          motorSet(MOTOR_CLAW_L, -127);
          motorSet(MOTOR_CLAW_R, 127);
        }
        // Reset position so when we hold nothing we know to hold claw pos
        clawLastPosLeft = 0;
        clawLastPosRight = 0;
      }
      // Closing, smaller more closed
      else if (buttonDirection == 2) {
        if (abs(clawPosLeft - clawPosRight) > MISAL_THRESH) { // Misaligned
          motorSet(MOTOR_CLAW_L, (clawPosRight > clawPosLeft) ? 90 : 127);
          motorSet(MOTOR_CLAW_R, (clawPosRight < clawPosLeft) ? -90 : -127);
        } else { // We good
          motorSet(MOTOR_CLAW_L, 127);
          motorSet(MOTOR_CLAW_R, -127);
        }
        // Reset position so when we hold nothing we know to hold claw pos
        clawLastPosLeft = 0;
        clawLastPosRight = 0;
      }
    }

    // Sitting still or autonomous control
    if (clawMode == 1 || buttonDirection == 0) {
      // If there's no position set yet, grab the current
      if (clawLastPosLeft == 0) {
        clawLastPosLeft = clawPosLeft;
        clawLastPosRight = clawPosRight;
      }

      // If we're misaligning, counteract --------------------------------------

      // Set the claw speed based on holding or reaching target, or if close
      int clawSpeed =
          clawTargetAchieved || (abs(clawPosLeft - clawLastPosLeft) < 100 ||
                                 abs(clawPosRight - clawLastPosRight) < 100)
              ? 50
              : 100;
      int negClawSpeed = -1 * clawSpeed;

      // Left claw
      if (abs(clawPosLeft - clawLastPosLeft) > MISAL_THRESH) {
        motorSet(MOTOR_CLAW_L,
                 clawPosLeft - clawLastPosLeft > 0 ? clawSpeed : negClawSpeed);
      } else {
        motorSet(MOTOR_CLAW_L, 0);
      }
      // Right claw
      if (abs(clawPosRight - clawLastPosRight) > MISAL_THRESH) {
        motorSet(MOTOR_CLAW_R, clawPosRight - clawLastPosRight > 0
                                   ? negClawSpeed
                                   : clawSpeed);
      } else {
        motorSet(MOTOR_CLAW_R, 0);
      }
    }

    // Give other things time to run
    delay(20);
  }
}

void stopClaw() {
  clawLastPosLeft = encoderGet(getEncoderClawL());
  clawLastPosRight = encoderGet(getEncoderClawR());
}

// Used for when we want to clamp an unknown orientation, and can't guarentee a
// certain close value
void clawClose(int ms) {
  setClawTarget(-50); // Try and close the claw
  int timeGone = 0;
  while (!(
      timeGone > ms ||
      clawTargetAchieved)) { // While we still have time or haven't reached 0ms
    delay(10);
    timeGone += 10;
  }
  stopClaw();
}

// Method to reset the claw to original position
void resetClaw() {
    setClawTarget(-10);

}
