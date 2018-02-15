#include "main.h"

bool isManualControl = true; // Whether or not to use manual controls
bool clawClosed = false;     // Whether or not the claw is closed
bool fourBarUp = false;      // Was the four bar last up or down
bool runAuton = false;    // Should the robot run auton (used for remote start)
bool liftLastDir = false; // true: up, false: down

void setRunAuton(bool shouldRun) { runAuton = shouldRun; }

void swapControlState() {
  if (isManualControl) {
    setIntakeTarget(getIntakePos());
    ensureLiftTaskRunning();
    ensureIntakeTaskRunning();
  } else {
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
  }
  isManualControl = !isManualControl;
}

// Manual control of the robot
void manualControl() {

  // Run the four bar
  if (joystickGetDigital(1, 8, JOY_UP)) {
    motorSet(MOTOR_FOUR_BAR,
             ((digitalRead(DIGITAL_LIM_CLAW))
                  ? ((joystickGetDigital(1, 7, JOY_RIGHT)) ? -50 : -127)
                  : ((joystickGetDigital(1, 7, JOY_RIGHT)) ? -50 : -10)));
    fourBarUp = true;
    setShouldHoldIntake(false);
    ensureIntakeTaskSuspended();
  } else if (joystickGetDigital(1, 8, JOY_RIGHT)) {
    motorSet(MOTOR_FOUR_BAR, (joystickGetDigital(1, 7, JOY_RIGHT)) ? 50 : 127);
    fourBarUp = false;
    setShouldHoldIntake(false);
    ensureIntakeTaskSuspended();
  } else {
    if (!isIntakeTaskRunning()) {
      motorSet(MOTOR_FOUR_BAR, (!fourBarUp) ? 10 : 0);
    }
  }

  // Run the lift
  if (joystickGetDigital(1, 7, JOY_UP)) {
    setLiftSpeedRaw(-127, 127);
    liftLastDir = true;
  } else if (joystickGetDigital(1, 7, JOY_LEFT)) {
    setLiftSpeedRaw(127, -127);
    liftLastDir = false;
  } else {
    int liftSpeed = (liftLastDir) ? 15 : 0;
    setLiftSpeedRaw(-liftSpeed, liftSpeed);
  }
  //  // Take intake to specific height
  //   if (joystickGetDigital(1, 8, JOY_LEFT)) {
  //     setIntakeTarget(INTAKE_POS_AVOID);
  //     if (!intakeTaskRunning) {
  //       taskResume(intakeCont);
  //       intakeTaskRunning = true;
  //     }
  //   }
}

void automaticControl() {
  /*
  ** Handle the main driver's controls
  */
}

// NOTE This is probably broken...
void operatorControl() {
  setLiftStartAsNow();

  // Cleanup
  shutdownPID(); // Make sure no PID subtask is running

  // Setup & start odometry
  initOdomScale(4, 15, 1); // Set up odom for 4 inch wheels with 15 inch diam
  // taskCreate(trackRobotPosition, TASK_DEFAULT_STACK_SIZE, NULL,
  //           (TASK_PRIORITY_DEFAULT)); // Start odometry tracking
  delay(50);   // Give odom some time to start
  odomReset(); // Clear it, leggo

  // TODO Disable during comp
  // Initialize the bluetooth listener
  blisten(1, blueListen); // Listen to messages

  getIntakeTask(); // Make sure we have an intake task created
  getLiftTask();   // Make sure we have a lift task created

  if (isManualControl) {
    ensureIntakeTaskSuspended();
    ensureLiftTaskSuspended();
  }

  while (true) { // true cooler than 1

    if (runAuton) {
      delay(250);
      autonomous();
      runAuton = false;
    }

    /*
    ** Handle the main driver's joysticks
    */
    // Drive normally, using the joystick channels 3 (Forward), 1 (Turn),
    // and 0 for strafe
    driveWithLogic(joystickGetAnalog(1, 3), joystickGetAnalog(1, 1), 0);

    // Swap control state
    // if (joystickGetDigital(1, 7, JOY_DOWN)) {
    //   if (sevenDReleased) {
    //     swapControlState();
    //   }
    //   sevenDReleased = false;
    // } else {
    //   sevenDReleased = true;
    // }

    // Run the proper control state
    if (isManualControl) {
      manualControl();
    } else {
      automaticControl();
    }

    // Run the claw
    if (isClawReady()) {
      if (!(joystickGetDigital(1, 6, JOY_UP) &&
            joystickGetDigital(1, 6, JOY_DOWN))) {
        if (joystickGetDigital(1, 6, JOY_UP)) {
          clawClosed = true;
          motorSet(MOTOR_CLAW, -127);
        } else if (joystickGetDigital(1, 6, JOY_DOWN)) {
          clawClosed = false;
          motorSet(MOTOR_CLAW, 127);
        } else {
          motorSet(MOTOR_CLAW, (clawClosed) ? -25 : 0);
        }
      }
    }

    // Run the mogo
    if (joystickGetDigital(1, 5, JOY_UP)) {
      motorSet(MOTOR_MOGO, 127);
    } else if (joystickGetDigital(1, 5, JOY_DOWN)) {
      motorSet(MOTOR_MOGO, -80);
    } else {
      motorSet(MOTOR_MOGO, 0);
    }

    delay(20);
  }
}
