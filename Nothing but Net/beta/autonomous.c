int minAutonomous = 1;
int maxAutonomous = 3;

bool setAutonomousMode(int number) {
	if ((number < minAutonomous) || (number > maxAutonomous)) { // Number is invalid
		return false;
		} else { // Number is valid
		// TODO Set number
		return true;
	}
}

int getAutonomousMode() {
	return 1; // TODO Return number
}

void runAutonomous() {
	int autoMode = 0;
	if (autoMode == 1) {// Mode 1
		// TODO: Stuff
		} else if (autoMode == 2) { // Mode 2
		// TODO: Stuff
	}
	else if (autoMode == 3) { // Mode 3
		// TODO: Stuff
	}
}
