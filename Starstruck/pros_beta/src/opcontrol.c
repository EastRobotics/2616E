#include "main.h"

void operatorControl() {

	initClawControl();
	initLift();

	while (true) { // true cooler than 1
		// Drive holonomicly, using the joystick channels 3 (Forward), 1 (Turn),
		// and 4 (Srafe).
		driveHolonomicWithLogic(joystickGetAnalog (1,3), joystickGetAnalog (1,1),
			joystickGetAnalog (1,4));

		// Move the lift, using the buttons 5U (Up), 5D (Down), and 7D(Lock)
		if (joystickGetDigital(1, 5, JOY_UP)) {
			moveLiftWithLogic(127, true, true);
		} else if (joystickGetDigital(1, 5, JOY_DOWN)) {
			moveLiftWithLogic(-127, true, true);
		} else if (joystickGetDigital(1, 7, JOY_DOWN)) {
			lockLift();
		} else {
			unlockLift(); // If no buttons are pressed, simply unlock the lift
		}

		// Test auton, but only after pressing an immensly complex series of buttons (All of the 7 group)
		if (joystickGetDigital(1, 7, JOY_UP)){
			if(joystickGetDigital(1, 7, JOY_LEFT) && joystickGetDigital(1, 7, JOY_DOWN)
			  && joystickGetDigital(1, 7, JOY_RIGHT)){
				autonomous();
			}
		}

		delay(15); // Give other tasks time to run
	}
}
