#include "main.h"
#include "string.h" // TODO Remove

// TODO Remove
// Listen to bluetooth commands from an external controller and respond
void blueListen(char *message) {
  if (strcmp(message, "pos\r\n") == 0) { // Send position
    fprintf(uart1, "Robot pos: (%d,%d)\r\n", getOdomPosX(), getOdomPosY());
  } else if (strcmp(message, "gyro\r\n") == 0) { // Send gyro
    fprintf(uart1, "Robot gyro: %d\r\n", gyroGet(getGyro()));
  } else if (strcmp(message, "ryan\r\n") == 0) { // Send give complaint
    bprint(1, "OMG it has too much give! >:(\r\n");
  } else // Unknown command
    fprintf(uart1, "I don't know what \"%s\" means :(", message);
}

void operatorControl() {
  shutdownPID(); // Make sure no PID subtask is running

  initOdomScale(4, 15, 1); // Set up odom for 4 inch wheels with 15 inch diam
  // Start odometry
  // taskCreate(trackRobotPosition, TASK_DEFAULT_STACK_SIZE, NULL,
  //           (TASK_PRIORITY_DEFAULT)); // Start odometry tracking
  delay(50);   // Give odom some time to start
  odomReset(); // Clear it, leggo

  // TODO Remove
  // Initialize the bluetooth listener
  blisten(1, blueListen); // Listen to messages

  // setAutonMode(4);
  // autonomous(); // Run auton test
  // print("Done auton");
  // shutdownPID(); // Make sure auton PID isn't running

  // 0: none, up: 1, down: 2
  int rightBumperState = 0;

  while (true) { // true cooler than 1
    /*
    ** Handle the main driver's joysticks
    */
    // Drive normally, using the joystick channels 3 (Forward), 1 (Turn),
    // and 0 for strafe
    driveHolonomicWithLogic(joystickGetAnalog(1, 3), joystickGetAnalog(1, 1),
                            0);

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
          manipulatorIntakeWait(); // TODO Manipulator to offload base
        else                       //  Down: Wait
          manipulatorIntakeWait();
      } break;
      // *********************************************************************
      case ACTION_EXTAKING: {
        // Could hurt the robot if changing states here, so only act if done
        if (isManipulatorReady()) {
          // Currently in extaking pos
          if (rightBumperState == 1) // Up: Offload.
            manipulatorIntakeWait(); // TODO Manipulator to offload base
          else                       //  Down: Wait
            manipulatorIntakeWait();
        }
      }
        // ***********************************************************************
        // TODO TODO TODO TODO TODO Case for when offloading base
        // ***********************************************************************
      } // End of the switch
        // ---------------------------------------------------------------------
    }

    // =========================================================================

    /*
    ** Handle the alt driver's controls
    */

    /*
    ** Boiler
    */

    // TODO Add lcd page for these
    // lcdClear(uart2);
    // lcdPrint(uart2, 1, "X:%d T:%d", getOdomPosX(), getOdomTheta());
    // lcdPrint(uart2, 1, "X:%d", getOdomPosX());
    // lcdPrint(uart2, 2, "Y:%d G:%d", getOdomPosY(), gyroGet(getGyro()));
    // lcdPrint(uart2, 2, "Y:%d", getOdomPosY());

    // Print odom pos
    // printf("\nX:%d\n", getOdomPosX());
    // printf("Y:%d\n\n", getOdomPosY());

    delay(20);
  }
}
