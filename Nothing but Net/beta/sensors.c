// ##################
// # Battery Checks #
// ##################

// Statuses explained:
// -------------------------------------
// 0 = All good / Above 50%
// 1 = Okay / Below or equal to 50%
// 2 = Bad / Below or equal to 20%
// 3 = Dangerous / Overvolting

// Gets the cortex battery level as a percentage
float getCortexBatteryLevel()
{
	return ((nImmediateBatteryLevel/1000.0)/7.2)*100.0;
}

// Gets the cortex battery status (Defined above)
int getCortexBatteryStatus()
{
	float cortexBatteryPercentage = getCortexBatteryLevel();
	if (cortexBatteryPercentage >= 106) {
		return 3;
		} else {
		if (cortexBatteryPercentage <= 50) {
			if (cortexBatteryPercentage <= 20) {
				return 2; // 20% or below battery
				} else {
				return 1; // 50% or below battery
			}
			} else {
			return 0; // Above 50% battery
		}
	}
}

// Gets the backup battery level as a percentage
float getBackupBatteryLevel()
{
	return ((BackupBatteryLevel/1000.0)/9.4)*100.0;
}

// Gets the backup battery status (Defined above)
int getBackupBatteryStatus()
{
	float backupBatteryPercentage = getBackupBatteryLevel();
	if (backupBatteryPercentage >= 106) {
		return 3;
		} else {
		if (backupBatteryPercentage <= 50) {
			if (backupBatteryPercentage <= 20) {
				return 2; // 20% or below battery
				} else {
				return 1; // 50% or below battery
			}
			} else {
			return 0; // Above 50% battery
		}
	}
}

// Gets the expander battery level as a percentage
float getExpanderBatteryLevel()
{
	return (SensorValue[powerExpander]/2053.0)*100.0; // TODO Return actual value
}

// Gets the expander battery status (Defined above)
int getExpanderBatteryStatus()
{
	float expanderBatteryPercentage = getExpanderBatteryLevel();
	if (expanderBatteryPercentage >= 106) {
		return 3;
		} else {
		if (expanderBatteryPercentage <= 50) {
			if (expanderBatteryPercentage <= 20) {
				return 2; // 20% or below battery
				} else {
				return 1; // 50% or below battery
			}
			} else {
			return 0; // Above 50% battery
		}
	}
}
