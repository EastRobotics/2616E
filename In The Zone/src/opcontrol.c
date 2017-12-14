#include "main.h"
#include "string.h" // TODO Remove

bool isManualControl = true; // Whether or not to use manual controls
bool clawClosed = false;     // Whether or not the claw is closed
bool fourBarUp = false;
bool runAuton = false;
int rightBumperState = 0; // 0: none, up: 1, down: 2
TaskHandle intakeCont;    // intake control task
TaskHandle liftCont;      // lift control task

// Listen to bluetooth commands from an external controller and respond
void blueListen(char *message) {
  if (strcmp(message, "pos\r\n") == 0) { // Send position
    fprintf(uart1, "Robot pos: (%d,%d) >\r\n", getOdomPosX(), getOdomPosY());
  } else if (strcmp(message, "gyro\r\n") == 0) { // Send gyro
    fprintf(uart1, "Robot gyro: %d\r\n >", gyroGet(getGyro()));
  } else if (strcmp(message, "rpm\r\n") == 0) {
    rpmTest();
  } else if (strcmp(message, "sensortest\r\n") == 0) {
    fprintf(uart1, "Lift  Enc: %d\r\n", encoderGet(getEncoderLift()));
    fprintf(uart1, "Left  Enc: %d\r\n", encoderGet(getEncoderBL()));
    fprintf(uart1, "Right Enc: %d\r\n", encoderGet(getEncoderBR()));
    fprintf(uart1, "Claw  Lim: %d\r\n", digitalRead(DIGITAL_LIM_CLAW));
    fprintf(uart1, "Cart  Pot: %d\r\n", analogRead(ANALOG_POT_FOUR_BAR));
  } else if (strcmp(message, "startauton\r\n") == 0) {
    runAuton = true;
  } else if (strcmp(message, "ryan\r\n") == 0) { // Send give complaint
    bprint(1, "OMG it has too much give! >:(\r\n");
  } else if (strcmp(message, "cherisse\r\n") ==
             0) { // Send a sign of disinterest
    bprint(1, "*SIGH*\r\n");
  } else if (strcmp(message, "cameron\r\n") ==
             0) { // Send a signal of comradery
    bprint(1, "oh meine Bruter\r\n");
  } else if (strcmp(message, "ian\r\n") == 0) { // Send a message of IDE loyalty
    bprint(1, "*opens android studio*\r\n");
  } else if (strcmp(message, "michael\r\n") == 0) { // Send an apology
    bprint(1, "sorry\r\n");
  } else // Unknown command
    fprintf(uart1, "I don't know what \"%s\" means :(", message);
}

// Manual control of the robot
void manualControl() {
  if (joystickGetDigital(1, 8, JOY_UP)) {
    motorSet(MOTOR_FOUR_BAR,
             ((digitalRead(DIGITAL_LIM_CLAW))
                  ? ((joystickGetDigital(1, 7, JOY_DOWN)) ? 50 : 127)
                  : ((joystickGetDigital(1, 7, JOY_DOWN)) ? 50 : 10)));
    fourBarUp = true;
  } else if (joystickGetDigital(1, 8, JOY_RIGHT)) {
    motorSet(MOTOR_FOUR_BAR, (joystickGetDigital(1, 7, JOY_DOWN)) ? -50 : -127);
    fourBarUp = false;
  } else {
    motorSet(MOTOR_FOUR_BAR, (fourBarUp) ? 10 : 0);
  }

  // Test other things
  if (joystickGetDigital(1, 7, JOY_UP)) {
    motorSet(MOTOR_LIFT_1, 127);
    motorSet(MOTOR_LIFT_2, -127);
  } else if (joystickGetDigital(1, 7, JOY_LEFT) &&
             (encoderGet(getEncoderLift()) > 0)) {
    int liftSpeed = encoderGet(getEncoderLift()) * 0.2;
    motorSet(MOTOR_LIFT_1, -liftSpeed);
    motorSet(MOTOR_LIFT_2, liftSpeed);
  } else {
    motorSet(MOTOR_LIFT_1, 0);
    motorSet(MOTOR_LIFT_2, 0);
  }
}

void automaticControl() {
  /*
  ** Handle the main driver's controls
  */
  // Right bumpers
  // =========================================================================
  // If nothing is pressed
  if (!joystickGetDigital(1, 6, JOY_DOWN) &&
      !joystickGetDigital(1, 6, JOY_UP)) {
    rightBumperState = 0; // Set state to 0
  } // IF we are just pressing something for the first time
  else if (rightBumperState == 0 && (joystickGetDigital(1, 6, JOY_DOWN) ||
                                     joystickGetDigital(1, 6, JOY_UP))) {
    // Set the state
    //// Up (dominant)
    if (joystickGetDigital(1, 6, JOY_UP))
      rightBumperState = 1;
    //// Down (recessive)
    else // Since we know something's pressed, has to be down then
      rightBumperState = 2;

    // Start of action handling switch
    // ---------------------------------------------------------------------
    switch (getAction()) {
    // *********************************************************************
    case ACTION_INTAKING: {
      // Currently in intaking pos
      if (rightBumperState == 1) // Up: Wait.
        manipulatorIntakeWait();
      //  Down: Nothing (Nowhere to go)
    } break;
    // *********************************************************************
    case ACTION_WAITING: {
      // Currently in waiting pos
      if (rightBumperState == 1) // Up: Score.
        manipulatorScore();
      else //  Down: Intake
        manipulatorIntake();
    } break;
    // *********************************************************************
    case ACTION_SCORING: {
      // Currently in scoring pos
      if (rightBumperState == 1) // Up: Offload.
        manipulatorOffload();
      else //  Down: Wait
        manipulatorIntakeWait();
    } break;
    // *********************************************************************
    case ACTION_EXTAKING: {
      // Could hurt the robot if changing states here, so only act if done
      if (isManipulatorReady()) {
        // Currently in extaking pos
        if (rightBumperState == 1) // Up: Offload.
          manipulatorOffload();
        else //  Down: Wait
          manipulatorIntakeWait();
      }
    } break;
    // *********************************************************************
    case ACTION_OFFLOADING: {
      // Currently in offloading pos
      if (rightBumperState == 2)
        manipulatorIntake();
    }
      // ***********************************************************************
      // TODO TODO TODO TODO TODO Case for when offloading base
      // ***********************************************************************
    } // End of the switch
      // ---------------------------------------------------------------------
  }
}

void swapControlState() {
  if (isManualControl) {
    taskResume(liftCont);
  } else {
    taskSuspend(liftCont);
  }

  isManualControl = !isManualControl;
}

void operatorControl() {
  // setAutonMode(4);
  // autonomous(); // Run auton test
  // print("Done auton");
  // shutdownPID(); // Make sure auton PID isn't running

  // Cleanup
  shutdownPID(); // Make sure no PID subtask is running

  // Setup & start odometry
  initOdomScale(4, 15, 1); // Set up odom for 4 inch wheels with 15 inch diam
  taskCreate(trackRobotPosition, TASK_DEFAULT_STACK_SIZE, NULL,
             (TASK_PRIORITY_DEFAULT)); // Start odometry tracking
  delay(50);                           // Give odom some time to start
  odomReset();                         // Clear it, leggo

  // TODO Disable during comp
  // Initialize the bluetooth listener
  blisten(1, blueListen); // Listen to messages

  // TODO REMOVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Start chainbar task
  // intakeCont = taskCreate(intakeControl, TASK_DEFAULT_STACK_SIZE, NULL,
  //                        (TASK_PRIORITY_DEFAULT));
  // liftCont = taskCreate(liftControl, TASK_DEFAULT_STACK_SIZE, NULL,
  //                      (TASK_PRIORITY_DEFAULT));
  // TODO REMOVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  while (true) { // true cooler than 1

    if(joystickGetDigital(1, 8, JOY_DOWN) && joystickGetDigital(1, 8, JOY_LEFT)) {
      runAuton = true;
      setAutonMode(2);
    }

    if (runAuton) {
      setAutonMode(2);
      delay(250);
      autonomous();
      runAuton = false;
    }

    if (joystickGetDigital(1, 8, JOY_DOWN) &&
        joystickGetDigital(1, 8, JOY_LEFT)) {
      deploy();
    }

    /*
    ** Handle the main driver's joysticks
    */
    // Drive normally, using the joystick channels 3 (Forward), 1 (Turn),
    // and 0 for strafe
    driveWithLogic(joystickGetAnalog(1, 3), joystickGetAnalog(1, 1), 0);

    // fprintf(uart1, "Lift Position: %d\r\n", getLiftHeight());

    // if (joystickGetDigital(1, 7, JOY_DOWN))
    //  swapControlState();

    if (isManualControl) {
      manualControl();
    } else {
      automaticControl();
    }

    if (isClawReady()) {
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

    if (joystickGetDigital(1, 5, JOY_UP)) {
      motorSet(MOTOR_MOGO_L, 127);
      motorSet(MOTOR_MOGO_R, -127);
    } else if (joystickGetDigital(1, 5, JOY_DOWN)) {
      motorSet(MOTOR_MOGO_L, -127);
      motorSet(MOTOR_MOGO_R, 127);
    } else {
      motorSet(MOTOR_MOGO_L, 0);
      motorSet(MOTOR_MOGO_R, 0);
    }

    // update the position on any external trackers
    // blueListen("pos\r\n");
    // blueListen("gyro\r\n");

    delay(20);
  }
}
