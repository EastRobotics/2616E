int x2 = 0, y1 = 0, x1 = 0, threshold = 15;
bool buttonReleased = true;
int highestCombination = 0;

int controllerLCDButtons() {
	int total = 0;
	if (!vexRT[Btn5U]) {
		buttonReleased = true;
		return 0;
	}
	if (vexRT[Btn8L])
		total += 1;
	if (vexRT[Btn8D])
		total += 2;
	if (vexRT[Btn8R])
		total += 4;
	return total;
}

void runCalculation() {
	// Driver motion control
	float driveMultiplier = -1;
	float turnMultiplier = -1.5;
	if (vexRT[Btn6D]) {
		driveMultiplier = -0.25;
		turnMultiplier =  -0.75;
		} else if (vexRT[Btn6U]) {
		driveMultiplier = -0.5;
		turnMultiplier = -1;
	}
	if(abs(vexRT[Ch3]) > threshold)
		y1 = floor(((float) vexRT[Ch3]) * driveMultiplier);
	else
		y1 = 0;
	if (abs(vexRT[Ch4]) > threshold)
		x1 = floor(((float) vexRT[Ch4]) * driveMultiplier);
	else
		x1 = 0;
	if (abs(vexRT[Ch1]) > threshold)
		x2 = floor(((float) vexRT[Ch1]) * turnMultiplier);
	else
		x2 = 0;

	motor[motorFrontRight] = y1 - x2 - x1;
	motor[motorBackRight] = y1-x2 + x1;
	motor[motorFrontLeft] = y1 + x2 + x1;
	motor[motorBackLeft] = y1 + x2 - x1;

	// Driver LCD Control
	int controllerButtons = controllerLCDButtons();
	if (controllerButtons == 0) { // A button wasn't pressed
		if (buttonReleased == false) {
			// Button was pressed then released, calculate what to do
			if (highestCombination == 1) { // Left button pressed
				lcdLastPage();
				buttonReleased = false;
			}
			else
				if (highestCombination == 2) { // Center button pressed
				lcdHome();
				buttonReleased = false;
			}
			else
				if (highestCombination == 4) { // Right button pressed
				lcdNextPage();
				buttonReleased = false;
			}
			else
				if (highestCombination == 3) { // Left & center pressed
				lcdNext();
			}
			else
				if (highestCombination == 6) { // Right & center pressed
				lcdBack();
			}
			buttonReleased = true;
			highestCombination = 0;
		} // Else: Nothing was pressed
	}
	else // A button was pressed
	{
		lcdResetAutoRefresh();
		if (buttonReleased) {
			buttonReleased = false;
			lcdResetHoldTime();
		}

		if (controllerButtons > highestCombination) {
			highestCombination = controllerButtons;
		}
	}

	// Intake control
	int intakeSpeed = 0;
	if (vexRT[Btn5U]) {
		intakeSpeed = 127;
	}
	else
		if (vexRT[Btn5D]) {
		intakeSpeed = -127;
	}
	motor[motorIntake] = intakeSpeed;


	// Arm control
	int armSpeed = 0;
	if (vexRT[Btn7D]) {
		armSpeed = 63;
	}
	motor[motorLauncherOne] = armSpeed;
	motor[motorLauncherTwo] = armSpeed;
	motor[motorLauncherThree] = armSpeed;
	motor[motorLauncherFour] = armSpeed;

	// Bar control
	int barSpeed = 0;
	if (vexRT[Btn7R]) {
		barSpeed = 127;
	}
	else
		if (vexRT[Btn7L]) {
		barSpeed = -127;
	}
	motor[motorBar] = barSpeed;
	if (vexRT[Btn7U]) {
		if (isBarReady()) {
			startTask( barLongShot );
		}
	}
}
