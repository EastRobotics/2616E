#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           testMotor,     tmotorVex393_HBridge, openLoop, encoderPort, I2C_3)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

float rpm = 0.0;
long lastTicks = nMotorEncoder[testMotor];
float RPMReadRate = 20.0;
float totalTrial = 0;
float trials = 200;
bool shortData = true; // If you only want data from trial # 200

// Michael's code for rpm, not 100% what each number is for
task readRPM() {
	while(true) {
		long currentTicks = nMotorEncoder[testMotor];
		rpm = (((((float)currentTicks)-((float)lastTicks))*(60000.0/RPMReadRate))/627.2); // Made for torque geared motors
		lastTicks = currentTicks;
		wait1Msec(((int)round(RPMReadRate)));
	}
}

task main()
{
	startTask ( readRPM );

	datalogClear();

	// Data looks like following (Each row is 1 speed):
	// Motor setting | Trial count | Current speed | Average speed | Minimum speed | Maximum speed
	for (int multiplier = 1; multiplier >= -1; multiplier-=2) {
		motor[testMotor] = 0;
		sleep(10000); // Sleep 10 seconds to ensure stopped motor
		for (int i = 0; i <= 125; i+=5) { // For each speed
			motor[testMotor] = i*multiplier;
			float maximumSpeed = 0;
			float minimumSpeed = 150;
			float averageSpeed = 0;
			totalTrial += 1;
			for (int i1 = 1; i1 <= trials; i1++) {
				if (abs(rpm) > abs(maximumSpeed))
					maximumSpeed = rpm;
				if (abs(rpm) < abs(minimumSpeed))
					minimumSpeed = rpm;
				averageSpeed += rpm;
				if (shortData ? i1 == trials-1 : true) {
					datalogDataGroupStart();
					datalogAddValue(0, i*multiplier); // Motor speed
					datalogAddValue(1, i1); // Trial count
					datalogAddValue(2, totalTrial); // Total trial count
					datalogAddValue(3, rpm); // Current speed
					datalogAddValue(4, averageSpeed/i1); // Average speed
					datalogAddValue(5, minimumSpeed); // Minimum speed
					datalogAddValue(6, maximumSpeed); // Max speed
					datalogDataGroupEnd();
					wait1Msec(25);
				}
			}
		}
	}

	stopTask ( readRPM );
}
