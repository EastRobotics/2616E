// This is a basic sketch of a concept I had. It's an assortment of functions for a drive, however it follows the idea of failsafe.
// It passively disables certain features based on conditions. Doing this lets us achieve some nice things, like...
//   a) Compensating the drive for a failed/unplugged motor
//   b) Preventing damage if something is malfunctioning
//        - (For example, we got a rubber band wrapped around our mecanum wheel, and it ended up pulling the axle out of it's socket)
// Currently, it just has the ability to disable motors and try to drive without that one. This would allow people to do things like disable a motor from their
// -> controller if they realized that motor is malfunctioning. This could also be used though in situations like...
//   a) Each motor has an encoder, and if one is acting up, disable it
//   b) in autonomous, if a sensor is giving a funky value, disable the drive until it gets back to normal or autonomous runs out
//     -> This is useful so you don't waste any assets if you're bound to fail. For example, don't misfire preloads in auton.

// Booleans for if a motor is disabled or not
bool motorFrontLeftActive = true;
bool motorBackLeftActive = true;
bool motorFrontRightActive = true;
bool motorBackRightActive = true;

// ###########################
// # Condition check methods #
// ###########################

// Get how many motors on the left side are disabled
int getDisabledCountLeft() {
	int disabled = 0;
	if (!motorFrontLeftActive) disabled += 1;
	if (!motorBackLeftActive) disabled += 1;
	return disabled;
}

// Get how many motors on the right side are disabled
int getDisabledCountRight() {
	int disabled = 0;
	if (!motorFrontRightActive) disabled += 1;
	if (!motorBackRightActive) disabled += 1;
	return disabled;
}

// Get how many motors in the front are disabled
int getDisabledCountFront() {
	int disabled = 0;
	if (!motorFrontRightActive) disabled += 1;
	if (!motorFrontLeftActive) disabled += 1;
	return disabled;
}

// Get how many motors in the back are disabled
int getDisabledCountBack() {
	int disabled = 0;
	if (!motorBackRightActive) disabled += 1;
	if (!motorBackLeftActive) disabled += 1;
	return disabled;
}

// Get how many motors in total are disabled
int getDisabledCount() {
	return getDisabledCountLeft() + getDisabledCountRight();
}

// #################
// # Drive methods #
// #################

/*
General
*/

// Raw input to motors. Nothing special, just cleaner than defining each.
void driveRaw(int motorFL, int motorBL, int motorFR, int motorBR) {
	// TODO Set motor speeds
}

/*
Omni drive
*/

// Decide front right motor speed based on input
int driveFR(int forwardPower, int turnPower) {
	// TODO Normal omni-wheel drive from controller input here.
	return 0;
}

// Decide front left motor speed based on input
int driveFL(int forwardPower, int turnPower) {
	// TODO Normal omni-wheel drive from controller input here.
	return 0;
}

// Decide back right motor speed based on input
int driveBR(int forwardPower, int turnPower) {
	// TODO Normal omni-wheel drive from controller input here.
	return 0;
}

// Decide back left motor speed based on input
int driveBL(int forwardPower, int turnPower) {
	// TODO Normal omni-wheel drive from controller input here.
	return 0;
}

// Run the drive function for each motor
void drive(int forwardPower, int turnPower) {
	// TODO Normal omni-wheel drive from controller input here.
	return;
}

// Run our condition checks and then set the motors accordingly
void driveWithCheck(int forwardPower, int turnPower) {
	int disabledCount = getDisabledCount();
	// Are we working fine?
	if (disabledCount < 1) {
		// Great, working fine. Run our plain drive method.
		drive(forwardPower, turnPower);
		return;
	}
	else {
		// Some motors are disabled.. let's figure out what to do

		if (disabledCount == 4) {
			// All motors are disabled. Set everything to 0.
			driveRaw(0,0,0,0);
			return;
		}

		if (disabledCount == 3) {
			// Going to be some pretty interesting driving here, but let's do our best...
			// TODO Set all but working to 0, set working to either positive or negative 127 based on sign of forward?
			return;
		}

		if (disabledCount == 2) {
			// Disable the 2 motors that are working, run the 2 that are.
			// If the 2 disabled are on the same side, there's no way to help the user drive.
			int motorFrontRightPower = 0;
			int motorBackRightPower = 0;
			int motorFrontLeftPower = 0;
			int motorBackLeftPower = 0;
			if (motorFrontRightActive)
				motorFrontRightPower = driveFR(forwardPower, turnPower);
			if (motorBackRightActive)
				motorBackRightPower = driveBR(forwardPower, turnPower);
			if (motorFrontLeftActive)
				motorFrontLeftPower = driveFL(forwardPower, turnPower);
			if (motorBackLeftActive)
				motorBackLeftPower = driveBL(forwardPower, turnPower);
			driveRaw(motorFrontLeftPower, motorBackLeftPower, motorFrontRightPower, motorBackRightPower);
			return;
		}

		if (disabledCount == 1) {
			// Easy fix. Set disabled to 0, set the 2 on the working side to half? power, set broken side to full power.
			// Front right motor disabled
			if (!motorFrontRightActive)
				driveRaw((int) driveFL(forwardPower, turnPower)/2,(int) driveBL(forwardPower, turnPower)/2,0,driveBR(forwardPower, turnPower));
			// Back right motor disabled
			if (!motorBackRightActive)
				driveRaw((int) driveFL(forwardPower, turnPower)/2,(int) driveBL(forwardPower, turnPower)/2,driveFR(forwardPower, turnPower),0);
			// Front left motor disabled
			if (!motorFrontLeftActive)
				driveRaw(0,driveBL(forwardPower, turnPower),(int) driveFR(forwardPower, turnPower)/2, (int) driveBR(forwardPower, turnPower)/2);
			// Back left motor disabled
			if (!motorBackLeftActive)
				driveRaw(driveFL(forwardPower, turnPower),0,(int) driveFR(forwardPower, turnPower)/2, (int) driveBR(forwardPower, turnPower)/2);
			return;
		}
	}
}

/*
Mecanum drive (and x-drive?)
(Currently not supported)
*/
void drive (int forwardPower, int turnPower, int strafePower) {
	// TODO The following condition block..
	// 1) If no strafepower, run omni check and return
	// 2) If strafepower, see if strafe is possible. If not, run omni check and return.
}
