#include "main.h"
#include "string.h" // TODO Remove

// TODO Remove
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
  shutdownPID(); // Make sure no PID is running

  initOdomScale(4, 15, 1); // Set up odom for 4 inch wheels with 15 inch diam
  // taskCreate(trackRobotPosition, TASK_DEFAULT_STACK_SIZE, NULL,
  //           (TASK_PRIORITY_DEFAULT)); // Start odometry tracking
  delay(100);  // Give odom some time to start
  odomReset(); // Clear it, leggo

  // TODO Remove
  blisten(1, blueListen); // Listen to messages

  setAutonMode(4);
  autonomous(); // Run auton test
  print("Done auton");
  shutdownPID(); // Make sure auton PID isn't running

  while (true) { // true cooler than 1
    // Drive normally, using the joystick channels 3 (Forward), 1 (Turn),
    // and 0 for strafe
    driveHolonomicWithLogic(joystickGetAnalog(1, 3), joystickGetAnalog(1, 1),
                            0);

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
