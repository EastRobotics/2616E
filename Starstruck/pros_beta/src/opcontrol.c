#include "main.h"
void operatorControl() {
	taskCreate(manageClaw, TASK_DEFAULT_STACK_SIZE,
    NULL, TASK_PRIORITY_DEFAULT);
	while (true) { // true cooler than 1
		// Drive holonomicly, using the joystick channels 3 (Forward), 1 (Turn),
		// and 4 (Srafe).
		driveHolonomicWithLogic(joystickGetAnalog (1,3), joystickGetAnalog (1,1),
			joystickGetAnalog (1,4));
			// Normal handling (right pad)
			if(joystickGetDigital(1, 7, JOY_DOWN)){ // If override is pressed
				if(joystickGetDigital(1, 8, JOY_RIGHT)) { // Backdrive launcher
					motor[chooR1] = motor[chooR2] = motor[chooR3] = motor[chooL1] = motor[chooL2] = motor[chooL3] = 127;
					} else if(vexRT[Btn8U]){ // Pull launcher down
					motor[chooR1] = motor[chooR2] = motor[chooR3] = motor[chooL1] = motor[chooL2] = motor[chooL3] = -127;
					} else { // Stop launcher
					motor[chooR1] = motor[chooR2] = motor[chooR3] = motor[chooL1] = motor[chooL2] = motor[chooL3] = 0;
				}
			}
			// Launch using our automated tasks
			if(canLaunch && vexRT[Btn8L]){
				launch();
			}
			// Special handling (left pad)
			if(vexRT[Btn8D]) { // Emergency launcher stopping
				stopAllLaunching();
			}
			if (vexRT[Btn7U]) { // If test auton button is pressed
				if (vexRT[Btn7D] && vexRT[Btn7L] && vexRT[Btn7R]) {
					runAuton();
				}
				} else { // Not pressing testing auton
				if(vexRT[Btn7R]){ // Reset launcher
					stopTask(taskLaunch);
					stopTask(taskLauncherReset);
					canLaunch = false;
					startTask( taskLauncherReset );
				}
				if(vexRT[Btn7L] && vexRT[Btn7D]){ // Force reset startingAngle
					startingAngle = SensorValue[potLauncher];
				}
			}

		delay(15); // Give other tasks time to run
	}
}
