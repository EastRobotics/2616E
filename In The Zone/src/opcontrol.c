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
  bool bumperRightPressed = false;

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
    // Right bumpers ===========================================================
    // Since action is state based, only act once per press
    //// Down
    if (joystickGetDigital(1, 6, JOY_DOWN)) {
      if (!bumperRightPressed) { // Only act if first time this press

        switch (getAction()) { // Start of switch ------------------------------
        case ACTION_INTAKING: {
          // Currently in intaking pos, so next up is waiting
          manipulatorIntakeWait();
        } break;
        case ACTION_WAITING: {
          // Currently in waiting pos, so next up is scoring
          manipulatorScore();
        } break;
        case ACTION_SCORING: {
          // Currently scoring, so next up is offloading mobile
          // TODO Set manipulators to allow for pushing out mobile base
        } break;
        case ACTION_EXTAKING: {
          // Currently extaking, so next up is offloading mobile
          // Could hurt the robot if changing states here, so only act if done
          if (isManipulatorReady()) {
            // TODO Set manipulators to allow for pushing out mobile base
          }
        }
        } // End of the switch -------------------------------------------------
      }
    } else
        //// Up
        if (joystickGetDigital(1, 6, JOY_UP)) {

    } else
      bumperRightPressed = false;
    //==========================================================================

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
