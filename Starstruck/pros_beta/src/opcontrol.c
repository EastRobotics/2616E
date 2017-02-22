#include "main.h"

bool lastLiftButtonDown = false;

void operatorControl() {
  setClawMode(0); // Make sure we have claw control
  initClawControl();
  initLift();
  shutdownPID(); // Make sure auton PID isn't running

  while (true) { // true cooler than 1
    // Drive normally, using the joystick channels 3 (Forward), 1 (Turn),
    // and 0 for strafe
    driveHolonomicWithLogic(joystickGetAnalog(1, 3), joystickGetAnalog(1, 1),
                            0);

    // Move the lift, using the buttons 5U (Up), 5D (Down), and 7D(Manual down)
    if (joystickGetDigital(1, 5, JOY_UP) ||
        joystickGetDigital(2, 5, JOY_UP)) { // 5U Held
      moveLiftWithLogic(127, true);
      lastLiftButtonDown = false;
    } else if (joystickGetDigital(1, 5, JOY_DOWN) ||
               joystickGetDigital(2, 5, JOY_DOWN)) { // 5D Held
      moveLiftWithLogic(-127, true);
      lastLiftButtonDown = true;
    } else { // Nothing held
      // If we didn't last press down, and are above 150 of start
      if ((!lastLiftButtonDown) &&
          (abs(getLiftStartAngle() - analogRead(ANALOG_POT_LIFT)) > 150)) {
        // lock the lift
        lockLift();
      } else { // Otherwise turn the lift off
        moveLiftWithLogic(0, true);
      }
    }

    // Lift autolift
    if (joystickGetDigital(1, 8, JOY_LEFT) ||
        joystickGetDigital(2, 8, JOY_LEFT)) { // 8L Held
      // TODO Make sure controller gets control if buttons are pressed
      // TODO  before the lift finishes
      // setLift(getLiftStartAngle()+200);
    }

    // Claw alignment buttons
    if (joystickGetDigital(1, 8, JOY_DOWN) ||
        joystickGetDigital(2, 8, JOY_DOWN)) { // 8D Held
      setClawTarget(0);                       // Tell claws to go back to 0
    }

    // Test auton, but only if pressing a complex series of buttons
    // (All of the 7 group)
    if (joystickGetDigital(1, 7, JOY_UP)) {
      // If the rest of the 7 group is pressed
      if (joystickGetDigital(1, 7, JOY_LEFT) &&
          joystickGetDigital(1, 7, JOY_DOWN) &&
          joystickGetDigital(1, 7, JOY_RIGHT)) { // Run auton
        moveLiftWithLogic(0, true);              // Disable lift
        autonomous();                            // Start auton
        setClawMode(0); // Give claw control back to controller
      }
    }

    // But if 7U isn't pressed, treat other 7 buttons as normal
    else { // Auton button isn't pressed
      // Check buttons 7 L, R, and D
      if (joystickGetDigital(1, 7, JOY_DOWN) ||
          joystickGetDigital(2, 7, JOY_DOWN)) { // 7D held
        moveLiftWithLogic(-40, false);
      }
    }

    delay(15); // Give other tasks time to run
  }
}
