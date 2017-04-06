#include "main.h"

bool lastLiftButtonDown = false;

void operatorControl() {
  shutdownPID(); // Make sure no PID is running

  initOdomScale(4, 15); // Set up odom for 4 inch wheels with 15 inch diam
  taskCreate(trackRobotPosition, TASK_DEFAULT_STACK_SIZE, NULL,
             (TASK_PRIORITY_DEFAULT)); // Start odometry tracking
  delay(100);                          // Give odom some time to start
  odomReset();                         // Clear it, leggo

  // setAutonMode(2);
  // autonomous(); // Run auton test
  // print("Done auton");
  // shutdownPID(); // Make sure auton PID isn't running

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
    printf("\nX:%d\n", getOdomPosX());
    printf("Y:%d\n\n", getOdomPosY());

    delay(20);
  }
}
