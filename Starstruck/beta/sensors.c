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
//                          			 Gyroscope Methods
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

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          			 Error escapes
//
// Since this is a lib, we aren't going to use every method. Have a never called function to eliminate the warnings.
//
/////////////////////////////////////////////////////////////////////////////////////////

// THIS SHOULD NEVER BE CALLED.
void sensorsErrorEscape() {
		getMainBatteryVoltage();
		getMainBatteryStatus();
		getBackupBatteryVoltage();
		getBackupBatteryStatus();
		getExpanderBatteryVoltage();
		getExpanderBatteryStatus();
		simplifyGyro(0, false);
}
