#include "main.h"

#define MANIPULATOR_AVOID_THRESH 15

bool isManualControl = true; // Whether or not to use manual controls
bool clawClosed = false;     // Whether or not the claw is closed
bool fourBarUp = false;
bool runAuton = false;
bool canAutoScore = false;
bool driverSetLiftStart = false;
bool sevenDReleased = true;
bool eightRReleased = true;
bool eightDReleased = true;
bool intakeTaskRunning = false;
bool liftLastDir = false;   // true: up, false: down
int rightBumperState = 0;   // 0: none, up: 1, down: 2
TaskHandle intakeCont;      // intake control task
TaskHandle liftCont;        // lift control task
TaskHandle manipulatorCont; // manipulator control task

// For autostacking preloading purposes
int gameloadLiftVal = 100;
int gameloadIntakeVal = 1600;
bool gameloadStacking = false;
bool hasDelayedLiftDrop =
    false; // If the lift has already waited to drop in autostack
bool hasDelayedConeDrop = false;
int autostackingBreakoutTime = 0;

void setRunAuton(bool shouldRun) { runAuton = shouldRun; }

void swapControlState() {
  if (isManualControl) {
    taskResume(liftCont);
    taskResume(intakeCont);
    if (!driverSetLiftStart) {
      encoderReset(getEncoderLift());
      driverSetLiftStart = true;
    }
  } else {
    taskSuspend(liftCont);
    taskSuspend(intakeCont);
  }
  if (!gameloadStacking) {
    motorSet(MOTOR_LIFT_1, 0);
    motorSet(MOTOR_LIFT_2, 0);
    motorSet(MOTOR_FOUR_BAR, 0);
  }
  isManualControl = !isManualControl;
}

// Manual control of the robot
void manualControl() {
  if (joystickGetDigital(1, 8, JOY_UP)) {
    motorSet(MOTOR_FOUR_BAR,
             ((digitalRead(DIGITAL_LIM_CLAW))
                  ? ((joystickGetDigital(1, 7, JOY_RIGHT)) ? -50 : -127)
                  : ((joystickGetDigital(1, 7, JOY_RIGHT)) ? -50 : -10)));
    fourBarUp = true;
    setShouldHoldIntake(false);
    if (intakeTaskRunning) {
      taskSuspend(intakeCont);
      intakeTaskRunning = false;
    }
  } else if (joystickGetDigital(1, 8, JOY_RIGHT)) {
    motorSet(MOTOR_FOUR_BAR, (joystickGetDigital(1, 7, JOY_RIGHT)) ? 50 : 127);
    fourBarUp = false;
    setShouldHoldIntake(false);
    if (intakeTaskRunning) {
      taskSuspend(intakeCont);
      intakeTaskRunning = false;
    }
  } else {
    motorSet(MOTOR_FOUR_BAR, (!fourBarUp) ? 10 : 0);
  }

  // Test other things
  if (joystickGetDigital(1, 7, JOY_UP)) {
    setLiftSpeed(127);
    liftLastDir = true;
  } else if (joystickGetDigital(1, 7, JOY_LEFT)) {
    //  && (encoderGet(getEncoderLift()) > 0)
    int liftSpeed = -127; // en coderGet(getEncoderLift()) * 0.2;
    setLiftSpeed(liftSpeed);
    liftLastDir = false;
  } else {
    int liftSpeed = (liftLastDir) ? 15 : 0;
    setLiftSpeedRaw(-liftSpeed, liftSpeed);
  }

  if (joystickGetDigital(1, 8, JOY_LEFT)) {
    setIntakeTarget(INTAKE_POS_AVOID);
    if (!intakeTaskRunning) {
      taskResume(intakeCont);
      intakeTaskRunning = true;
    }
  }
}

void automaticControl() {
  /*
  ** Handle the main driver's controls
  */

  if (joystickGetDigital(1, 8, JOY_UP)) {
    setConeCount(0);
  }

  if (joystickGetDigital(1, 8, JOY_RIGHT)) {
    if (eightRReleased) {
      setConeCount(getConeCount() - 1);
    }
    eightRReleased = false;
  } else {
    eightRReleased = true;
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

  // Start chainbar task
  intakeCont = taskCreate(intakeControl, TASK_DEFAULT_STACK_SIZE, NULL,
                          (TASK_PRIORITY_DEFAULT));
  liftCont = taskCreate(liftControl, TASK_DEFAULT_STACK_SIZE, NULL,
                        (TASK_PRIORITY_DEFAULT));

  if (isManualControl) {
    taskSuspend(intakeCont);
    taskSuspend(liftCont);
  } else {
    manipulatorCont = taskCreate(manipulatorControl, TASK_DEFAULT_STACK_SIZE,
                                 NULL, (TASK_PRIORITY_DEFAULT));
  }

  while (true) { // true cooler than 1

    if (runAuton) {
      setAutonMode(4);

      if (isManualControl) {
        taskResume(intakeCont);
        taskResume(liftCont);
        manipulatorCont =
            taskCreate(manipulatorControl, TASK_DEFAULT_STACK_SIZE, NULL,
                       (TASK_PRIORITY_DEFAULT));
      }

      delay(250);
      autonomous();

      if (isManualControl) {
        taskSuspend(intakeCont);
        taskSuspend(liftCont);
        taskDelete(manipulatorCont);
      }

      runAuton = false;
    }

    /*
    ** Handle the main driver's joysticks
    */
    // Drive normally, using the joystick channels 3 (Forward), 1 (Turn),
    // and 0 for strafe
    driveWithLogic(joystickGetAnalog(1, 3), joystickGetAnalog(1, 1), 0);

    if (joystickGetDigital(1, 7, JOY_DOWN)) {
      if (sevenDReleased) {
        // swapControlState();
      }
      sevenDReleased = false;
    } else {
      sevenDReleased = true;
    }

    if (isManualControl) {
      manualControl();
    } else {
      automaticControl();
    }

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
          motorSet(MOTOR_CLAW, (clawClosed) ? -10 : 0);
        }
      }
    }

    if (joystickGetDigital(1, 5, JOY_UP)) {
      motorSet(MOTOR_MOGO, 127);
    } else if (joystickGetDigital(1, 5, JOY_DOWN)) {
      motorSet(MOTOR_MOGO, -127);
    } else {
      motorSet(MOTOR_MOGO, 0);
    }

    delay(20);
  }
}
