#include "main.h"

bool isManualControl = true; // Whether or not to use manual controls
bool clawClosed = true;      // Whether or not the claw is closed
bool fourBarUp = false;      // Was the four bar last up or down
bool runAuton = false;    // Should the robot run auton (used for remote start)
bool liftLastDir = false; // true: up, false: down
bool sevenDReleased = true;
bool sevenRReleased = true;
bool sevenLReleased = true;
bool eightUReleased = true;
bool eightDReleased = true;
bool justStacked = false;
bool holdDrive = false;

void setRunAuton(bool shouldRun) { runAuton = shouldRun; }

void swapControlState() {
  ensureLiftTaskSuspended();
  ensureIntakeTaskSuspended();
  if (isManualControl) {
    setIntakeTarget(getIntakePos());
  } else {
    setScoring(false);
  }
  isManualControl = !isManualControl;
}

// Manual control of the robot
void manualControl() {

  //  // Take intake to specific height
  //   if (joystickGetDigital(1, 8, JOY_LEFT)) {
  //     setIntakeTarget(INTAKE_POS_AVOID);
  //     if (!intakeTaskRunning) {
  //       taskResume(intakeCont);
  //       intakeTaskRunning = true;
  //     }
  //   }

  if (joystickGetDigital(1, 8, JOY_DOWN)) {
    if (eightDReleased) {
      setIntakeTarget(getIntakePos());
      ensureIntakeTaskRunning();
      motorSet(MOTOR_CLAW, 127);
      delay(200);
      setLiftSpeed(127);
      delay(200);
      setLiftSpeed(0);
      setIntakeTarget(3600);
      motorSet(MOTOR_CLAW, -25);
      waitForIntake();
      ensureIntakeTaskSuspended();
      setLiftTarget(0);
      ensureLiftTaskRunning();
      waitForLift();
      ensureLiftTaskSuspended();
    }
    eightDReleased = false;
  } else {
    eightDReleased = true;
  }

  if (joystickGetDigital(1, 7, JOY_RIGHT)) {
    if (sevenLReleased) {
      holdDrive = !holdDrive;
    }
    sevenLReleased = false;
  } else {
    sevenLReleased = true;
  }
}

void automaticControl() {
  /*
  ** Handle the main driver's controls
  */
  if (joystickGetDigital(1, 8, JOY_DOWN)) {
    if (isManipulatorReady()) {
      score();
      justStacked = true;
    }
  }

  if (joystickGetDigital(1, 8, JOY_LEFT)) {
    setConeCount(0);
    setIsOnLoader(false);
  }

  if (joystickGetDigital(1, 7, JOY_RIGHT)) {
    if (sevenRReleased) {
      setConeCount(getConeCount() - 1);
      setIsOnLoader(true);
    }
    sevenRReleased = false;
  } else {
    sevenRReleased = true;
  }

  if (joystickGetDigital(1, 7, JOY_UP)) {
    setLiftReturnHeight(0);
  }

  if (joystickGetDigital(1, 8, JOY_UP)) {
    if (eightUReleased) {
      setLiftReturnHeight(350);
      setLiftTarget(350);
    }
    eightUReleased = false;
  } else {
    eightUReleased = true;
  }
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

  ensureLiftTaskSuspended();
  ensureIntakeTaskSuspended();

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
    if (!holdDrive) {
      if ((abs(joystickGetAnalog(1, 3)) > 10) ||
          (abs(joystickGetAnalog(1, 1)) > 10)) {
        driveWithLogic(joystickGetAnalog(1, 3), joystickGetAnalog(1, 1), 0);
      } else {
        driveTank(-(joystickGetAnalog(2, 2) / 2),
                  -(joystickGetAnalog(2, 3) / 2));
      }
    } else {
      driveRaw(10, 10, 10, 10);
    }

    // Swap control state
    if (joystickGetDigital(1, 7, JOY_DOWN)) {
      if (sevenDReleased) {
        swapControlState();
      }
      sevenDReleased = false;
    } else {
      sevenDReleased = true;
    }

    // Run the proper control state
    if (isManualControl) {
      manualControl();
    } else {
      automaticControl();
    }

    // Run the claw
    if (isClawReady() && isManipulatorReady()) {
      if (!(joystickGetDigital(1, 6, JOY_UP) &&
            joystickGetDigital(1, 6, JOY_DOWN))) {
        if (joystickGetDigital(1, 6, JOY_UP)) {
          clawClosed = true;
          motorSet(MOTOR_CLAW, -127);
        } else if (joystickGetDigital(1, 6, JOY_DOWN)) {
          clawClosed = false;
          motorSet(MOTOR_CLAW, 127);
        } else {
          motorSet(MOTOR_CLAW, (clawClosed) ? -25 : 127);
        }
      }
    }

    // Run the mogo
    if (joystickGetDigital(1, 5, JOY_UP) || joystickGetDigital(2, 5, JOY_UP)) {
      motorSet(MOTOR_MOGO, 127);
    } else if (joystickGetDigital(1, 5, JOY_DOWN) ||
               joystickGetDigital(2, 5, JOY_DOWN)) {
      motorSet(MOTOR_MOGO, -80);
    } else {
      motorSet(MOTOR_MOGO, 0);
    }

    if (isManipulatorReady()) {
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
        motorSet(MOTOR_FOUR_BAR,
                 (joystickGetDigital(1, 7, JOY_RIGHT)) ? 50 : 127);
        fourBarUp = false;
        setShouldHoldIntake(false);
        ensureIntakeTaskSuspended();
      } else {
        if (!isIntakeTaskRunning()) {
          motorSet(MOTOR_FOUR_BAR, (!fourBarUp) ? 10 : 0);
        }
      }
    }
    if (isManipulatorReady() && !isLiftTaskRunning()) {
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
    }

    delay(20);
  }
}
