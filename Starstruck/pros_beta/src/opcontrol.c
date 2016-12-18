#include "main.h"
void operatorControl() {
	while (true) { // true cooler than 1
		// Drive holonomicly, using the joystick channels 3 (Forward), 1 (Turn),
		// and 4 (Srafe).
		driveHolonomicWithLogic(joystickGetAnalog (1,3), joystickGetAnalog (1,1),
			joystickGetAnalog (1,4));

		// Temporary bang bang until we have claw.c done
		const int misal = 100;
		int clawLastPosLeft;
		int clawLastPosRight;
		int clawPosLeft = analogRead(ANALOG_POT_CLAW_L);
		int clawPosRight = analogRead(ANALOG_POT_CLAW_R);
		if (joystickGetDigital(1, 6, JOY_UP)) { // Opening, bigger is more open
			if (abs(clawPosLeft-clawPosRight) > misal) { // Misaligned
				motorSet(MOTOR_CLAW_L,(clawPosRight < clawPosLeft) ? 90 : 127);
				motorSet(MOTOR_CLAW_R,-1*(clawPosRight > clawPosLeft) ? 90 : 127);
			} else { // We good
				motorSet(MOTOR_CLAW_L,127);
				motorSet(MOTOR_CLAW_R,-127);
			}
			clawLastPosLeft = 0;
			clawLastPosRight = 0;
		} else if (joystickGetDigital(1, 6, JOY_DOWN)) { // Closing, smaller more closed
			if (abs(clawPosLeft-clawPosRight) > misal) { // Misaligned
				motorSet(MOTOR_CLAW_L,-1*(clawPosRight > clawPosLeft) ? 90 : 127);
				motorSet(MOTOR_CLAW_R,(clawPosRight < clawPosLeft) ? 90 : 127);
			} else { // We good
				motorSet(MOTOR_CLAW_L,-127);
				motorSet(MOTOR_CLAW_R,127);
			}
			clawLastPosLeft = 0;
			clawLastPosRight = 0;
		} else { // Sitting still
			if (clawLastPosLeft != 0) {
				clawLastPosLeft = clawPosLeft;
				clawLastPosRight = clawPosRight;
			}
			// If we're misaligning, counteract
			// Left claw
			if (abs(clawPosLeft-clawLastPosLeft) > misal)
				motorSet(MOTOR_CLAW_L,clawPosLeft-clawLastPosLeft > 0 ? -30 : 30);
			else
				motorSet(MOTOR_CLAW_L,0);
			// Right claw
			if (abs(clawPosRight-clawLastPosRight) > misal)
				motorSet(MOTOR_CLAW_R,clawPosRight-clawLastPosRight > 0 ? 30 : -30);
			else
				motorSet(MOTOR_CLAW_R,0);
		}
		delay(20); // Give other tasks time to run
	}
}
