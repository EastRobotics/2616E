float currentPosition = 0;
bool ready = false;
int minMoveSpeed = 20;

int defaultAccuracy = 15.0;
int defaultThreshold = 30.0;

int fullCourtPos = 1000;
int halfCourtPos = 500;
int closeCourtPos = 100;
int hookPos = 200;

void resetBar() {
	motor[motorBar] = 60;
	wait1Msec(1000);
	motor[motorBar] = 0;
	currentPosition = 0;
	SensorValue[I2C_1] = 0;
}

task updateBar() {
	while (true) {
		int motorSpeed = motor[motorBar];
		int ticks = SensorValue[I2C_1];
		if (motorSpeed > 0) {
			currentPosition += (ticks);
		}
		else
			if (motorSpeed < 0) {
			currentPosition -= (ticks);
		}
		else
		{
			// Motor was dragged D:
			// TODO Try to figure out if we need to reset
		}
		SensorValue[I2C_1] = 0;
	}
}

int getBarPosition() {
	return currentPosition;
}

void setSpeedRelative(int speed, float distance) {
	if (speed > 127) {
		speed = 127;
	}
	if (distance > 0) {
		motor[motorBar] = -1*speed;
		return;
	}
	motor[motorBar] = speed;
}

void moveBar(float position, float accuracy, int threshold) {
	ready = false;
	float signedDistance = position-currentPosition;
	float distance = abs(signedDistance);
	while ( distance <= accuracy ) {
		if (distance <= threshold) { // We're in our thresh, set speed slow to get to our position
			setSpeedRelative(minMoveSpeed, signedDistance);
		}
		else
		{ // We're above our thresh, set speed relative to our distance to speed things up
			int speed = 0;
			speed = (int) ((distance/4.0)+minMoveSpeed);
			motor[motorBar] = speed;
		}
	}
	ready = true;
}

task barCloseShot() {
	moveBar(fullCourtPos, defaultAccuracy, defaultThreshold);
}

task barMediumShot() {
	moveBar(halfCourtPos, defaultAccuracy, defaultThreshold);
}

task barLongShot() {
	moveBar(closeCourtPos, defaultAccuracy, defaultThreshold);
}

task barHookPos() {
	moveBar(hookPos, defaultAccuracy, defaultThreshold);
}

bool isBarReady() {
	return ready;
}
