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
// NOTE: If using simplifyTo360, please read NOTE of gyroToFloat below.
int simplifyGyro(int degree, bool simplifyTo360) {
	int simplified = abs(degree);
	if (simplifyTo360)
		simplified = floor((((float)simplified)/10.0) + 0.5);
	// Make sure we don't round up to 360, a value gyro can't make
	if (simplified == 360) simplified = 0;
	return simplified;
}

// Simplifies raw gyro input (-3599 to 3599) to 0.0-359.99999...
// PARAMETERS:
// 	int: The raw data from the gyro, retrieved like SensorValue[sensor]
//	bool: Whether or not to make from 3600 degrees (1/10th) to 360 degreess (single)
// RETURNS:
//	float: The simplified degree
// NOTE:
//  - While similar to simplifyGyro, this method doesn't cut off the remainder of the division.
//  - For example, gyroToFloat(3137) = 313.7, but simplifyGyro(3137) returns 314
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
	// When using with a controller, sideOne should be the X axis
	// and side two should be the y axis
	// If the value is 0,0, meaning joystick not moved,
	// return -1 to avoid errors, and allow caller to handle accordingly
	if(sideOne==0.0&&sideTwo==0.0){
		return -1.0;
	}

	// Handle angles that fall on axis, to again avoid errors later
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

	// Determine the quadrant of the angle, because of atan's range restrictions
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

	// CD sstands for Cardinal Direction
	float nearbyCD = (90.0*(quadrant-1.0));
	float finalAngle = nearbyCD;
	// Place the angle in the proper place, besides just in atans range of quadrants 1 and 2
	if(fabs(sideOne)>=fabs(sideTwo)){
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
const long autonTime = 15000; // Auton time in millis
const long controlTime = 105000; // User control time in millis
const long skillsTime = 60000; // Skills time in millis
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
long getGameTimeRunning() {
	return time1[T3];
}

// Gets how long is left in the game
// RETURNS:
// 	int: Time the game has left, in milliseconds
long getGameTime() {
	return time1[T3] - (gamemode ? controlTime : autonTime);
}

// Gets how long is left in the game, considering it's skills
// RETURNS:
// 	int: Time the skills match has left, in milliseconds
long getSkillsTime() {
	return time1[T3]-skillsTime;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          RPM and Motor Speed Conversions
//
/////////////////////////////////////////////////////////////////////////////////////////

//Converts an RPM value to a motor speed using the function f(x) = 11.431e^(0.0217x)
//For use with a torque geared motor
//PARAMETERS:
// float: The RPM to be converted to a motor speed
//RETURNS:
// int: The motor speed that would get the RPM given
int RPMToMotor(float RPM){
	RPM = 11.431 * ((float)pow(2.71828,(RPM*0.0217)));
	RPM = (RPM > 127.0) ? 127.0 : ((RPM < -127.0) ? -127.0 : RPM);
	return ((int)round(RPM));
}

//Converts a motor speed to an RPM value using the function f(x) = 44.486ln(x) - 105.47
//For use with a torque geared motor
//PARAMETERS:
// int: The motor speed to be converted to an RPM value
//RETURNS:
// float: The RPM that a motor at the given speed should spin at
float motorToRPM(int motorSpeed){
	return (motorSpeed<0) ? ((log(((abs(motorSpeed))))*44.486) - 105.47) : -1*((log(((abs(motorSpeed))))*44.486) - 105.47);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Linear Interpolation Functions
//
/////////////////////////////////////////////////////////////////////////////////////////

//Calculates a slope using the equation m = (y2-y1)/(x2-x1)
//PARAMETERS:
// float: the first x value
// float: the first y value
// float: the second x value
// float: the second y value
//RETURNS
// float: the slope of the line with the given points
float getSlope(float x1 = 15.0, float y1 = 25.0, float x2 = 127.0, float y2 = 105.0){
	return ((y2-y1)/(x2-x1));
}

//Calculates the y-intercept of a line given by the equation y = mx + b
//rewritten for these purposes as b = y - mx
//PARAMETERS:
// float: the slope of the line
// float: the x coordinate of a given point
// float: the y coordinate of a given point
//RETURNS:
// float: the y-intercept of the line with the given point and slope
float getYInt(float slope = 0.714, float x = 127.0, float y = 105.0){
	return  (y - (slope * x));
}

//Gives a direct relationship between two points and the values inbetween by
//finding the equation of the line between the min and max values and plugging
//the value into the equation
//PARAMETERS:
// float: the minimum value of the dependent variable
// float: the minimum value of the independent variable
// float: the maximum value of the dependent variable
// float: the maximum value of the independent variable
// float: a decimal between 0 and 1, indicating the amount
//        the current value is accross the line (like a
//        percent completed)
//RETURNS:
// float: the result of the linear interpolation function with the given progress
float lerp(float x1, float y1, float x2, float y2, float progress){
		return ((progress*x2)*getSlope(x1,y1,x2,y2))+getYInt(getSlope(x1,y1,x2,y2),x2,y2);
}

//Performs linear interpolation specific to the motors
//PARAMETERS:
// int: the power that the motor should be run at (possibly directly from joystick)
// int: the minimum speed that the motor should run at
// int: the minimum power that the motors should be allowed to move with
//				(sort of like a joystick threshold)
//RETURNS:
// int: the motor speed with the linear interpolation performed on it
int getLerpedSpeed(int power = 0, int minSpeed = 25, int minPower = 15){
	if(power < minPower)
		return 0;
	int speed = RPMToMotor(lerp((float) minPower, (float) minSpeed, 127.0, 105.0, (float) power/127.0));
	return (speed > 127) ? 127 : ((speed<0) ? 0 : speed);
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
	degreeToRad(0.0);
	gyroToFloat(100);
	motorToRPM(1);
}
