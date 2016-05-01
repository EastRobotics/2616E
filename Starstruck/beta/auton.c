bool position = false; // False for left, true for right
bool color = false; // False for red, true for blue
int minAutonomous = 1;
int maxAutonomous = 3;
int currentMode = minAutonomous;

bool setAutonMode(int mode) {
	if ((mode < minAutonomous) || (mode > maxAutonomous)) { // Number is invalid
		return false;
		} else { // Number is valid
		currentMode = mode;
		return true;
	}
}

int getAutonMode() {
	return currentMode;
}

int getAutonModeMin() {
	return minAutonomous;
}

int getAutonModeMax() {
	return maxAutonomous;
}

// False for left, true for right
void setAutonPosition(bool setPosition) {
	position = setPosition;
}

bool getAutonPosition() {
	return position;
}

// False for red, true for blue
void setAutonColor(bool setColor) {
	color = setColor;
}

bool getAutonColor() {
	return color;
}

// Auton modes should handle team color and positon
// (Thanks vex for making the pole opposite for each side)
void runAuton() {
	if (currentMode == 1) {// Mode 1. Do nothing, worst case scenario for conflicting autons
			return;
		} else if (currentMode == 2) { // Mode 2
		// TODO: Stuff
	}
	else if (currentMode == 3) { // Mode 3
		// TODO: Stuff
	}
}
