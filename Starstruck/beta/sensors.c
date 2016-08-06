/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			 Battery Methods
//
// We have a standard of status for all batteries, as a simple way of knowing how to handle them.
// They are a system of ints, and are defined as follows:
//   - 0: All good / Above 50%
//   - 1: Okay / Above 20% but lower/equal to 50%
//   - 2: Bad / Lower/equal to 20%
// It does not need to be in a sense of precentage, however it should follow ~ the same guidelines.
//
/////////////////////////////////////////////////////////////////////////////////////////

// TODO Modify these standards
// Voltage standards for vex 7.2v batteries, in volts
float okayVoltage = 7.2; // If above, 0.
float badVoltage = 6.5; // If above, 1. If below, 2.

// Gets the cortex battery voltage
// RETURNS:
//	float: The voltage of the battery
float getMainBatteryVoltage()
{
	return nAvgBatteryLevel/1000.0;
}

// Gets the cortex battery status based on above defined standards
// RETURNS:
//	int: Status of the battery
int getMainBatteryStatus()
{
	float cortexVoltage = getMainBatteryVoltage();
	if (cortexVoltage > okayVoltage) return 0;
	if (cortexVoltage > badVoltage) return 1;
	return 2;
}

// Gets the backup battery voltage
// RETURNS:
//	float: The voltage of the battery
float getBackupBatteryVoltage()
{
	return BackupBatteryLevel/1000.0;
}

// Gets the cortex battery status based on above defined standards
// RETURNS:
//	int: Status of the battery
int getBackupBatteryStatus()
{
	float backupVoltage = getBackupBatteryVoltage();
	if (backupVoltage > okayVoltage) return 0;
	if (backupVoltage > badVoltage) return 1;
	return 2;
}

// Gets the expander battery voltage
// RETURNS:
//	float: The voltage of the battery
float getExpanderBatteryVoltage()
{
	// TODO Figure this out. I've read a billion different things. The above was found out through
	// -> manual math by figuring out the voltage on the cortex port, then comparing to the value
	// -> returned by the expander
	return (SensorValue[powerExpander]/2053.0)*100.0;
}

// Gets the expander battery status (Defined above)
// RETURNS:
//	int: Status of the battery
int getExpanderBatteryStatus()
{
	float expanderVoltage = getExpanderBatteryVoltage();
	if (expanderVoltage > okayVoltage) return 0;
	if (expanderVoltage > badVoltage) return 1;
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          	Gyroscope/Angle Methods
//
/////////////////////////////////////////////////////////////////////////////////////////

// Simplifies raw gyro input (-3599 to 3599) to either 0-3599 or 0-359
// PARAMETERS:
// 	int: The raw data from the gyro, retrieved like SensorValue[sensor]
//	bool: Whether or not to make from 3600 degrees (1/10th) to 360 degreess (single)
// RETURNS:
//	int: The simplified degree
int simplifyGyro(int degree, bool simplifyTo360) {
	int simplified = abs(degree);
	if (simplifyTo360)
		simplified = floor((simplified/10.0) + 0.5);
	return simplified;
}

float gyroToFloat(int degree) {
	return ((float) abs(degree))/10.0;
}

// A simple function for degree to radian conversions
// PARAMETERS:
// 	float: A simplified degree like 180.7 (use gyroToFloat to simplify a gyro value)
// RETURNS:
// 	float: Provided degree in radians
float degreeToRad(float degree){
	return (degree*3.14)/180.0;
}

// Takes in the sides of a triangle and returns the angle made
// PARAMETERS:
//  float: The x value of the joystick, or just x value of the triangle
//  float: The y value of the joystick, or just y value of the triangle
// RETURNS:
//  float: The angle calculated from the joystick values
float sidesToAngle(float sideOne, float sideTwo){
	//when using with a controller, sideOne should be the X axis
	//and side two should be the y axis
	//if the value is 0,0, meaning joystick not moved,
	//return -1 to avoid errors, and allow caller to handle accordingly
	if(sideOne==0.0&&sideTwo==0.0){
		return -1.0;
	}

	//Handle angles that fall on axis, to again avoid errors later
	if(sideOne==0.0||sideTwo==0.0){
		if(sideOne==0.0){
			if(sideTwo>0.0){
				return 90.0;
				} else {
				return 270.0;
			}
			} else {
			if(sideOne>0.0){
				return 0.0;
				} else {
				return 180.0;
			}
		}
	}

	float angle = atan(sideTwo/sideOne);
	angle = ((angle*180.0)/3.14);

	//Determine the quadrant of the angle, because of atan's range restrictions
	float quadrant = 0.0;
	if(angle>0.0){
		if(sideOne<0.0&&sideTwo<0.0){
			quadrant = 3.0;
			} else {
			quadrant = 1.0;
		}
		} else {
		if(sideOne<0.0){
			quadrant = 2.0;
			} else {
			quadrant = 4.0;
		}
	}

	//CD sstands for Cardinal Direction
	float nearbyCD = (90.0*(quadrant-1.0));
	float finalAngle = nearbyCD;
	//Place the angle in the proper place, besides just in atans range of quadrants 1 and 2
	if(abs(sideOne)>=abs(sideTwo)){
		finalAngle += abs(angle);
		} else {
		if(quadrant==1.0||quadrant==4.0){
			if(quadrant==4.0){
				finalAngle+=90.0;
			}
			finalAngle+=angle;
			} else {
			return (angle+180.0);
		}
	}
	return finalAngle;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          	Game time tracking Methods
//
/////////////////////////////////////////////////////////////////////////////////////////

// TODO: Change the following values to correct ones in millis
const int autonTime = 1000; // Auton time in millis
const int controlTime = 1000; // User control time in millis
const int skillsTime = 1000; // Skills time in millis
bool gamemode = false; // False = auton control, true = user control

// Sets up the timer for game time tracking
// PARAMETERS:
// 	bool: What gamemode to use. False = auton control, true = user control
void setupGameTimer(bool _gamemode) {
		gamemode = _gamemode;
		clearTimer(T3);
}

// Gets how long the game has been running
// RETURNS:
// 	int: Time the game has been running, in milliseconds
int getGameTimeRunning() {
	return time1[T3];
}

// Gets how long is left in the game
// RETURNS:
// 	int: Time the game has left, in milliseconds
int getGameTime() {
	return time1[T3] - (gamemode ? controlTime : autonTime);
}

// Gets how long is left in the game, considering it's skills
// RETURNS:
// 	int: Time the skills match has left, in milliseconds
int getSkillsTime() {
	return time1[T3]-skillsTime;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			 Error escapes
//
// Since this is a lib, we aren't going to use every method. Have a never called function to eliminate the warnings.
//
/////////////////////////////////////////////////////////////////////////////////////////

// THIS SHOULD NEVER BE CALLED.
void sensorsErrorEscape() {
	sidesToAngle(0,0);
	getMainBatteryVoltage();
	getMainBatteryStatus();
	getBackupBatteryVoltage();
	getBackupBatteryStatus();
	getExpanderBatteryVoltage();
	getExpanderBatteryStatus();
	setupGameTimer(false);
	getGameTimeRunning();
	getGameTime();
	getSkillsTime();
	simplifyGyro(0, false);
}
