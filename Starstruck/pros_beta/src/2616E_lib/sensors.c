#include "main.h"
#include "math.h"

unsigned int powerLevelExpander(unsigned char channel, bool newVersion = true){
  int returned = analogRead(channel);
  return newVersion ? returned/70 : returned/45.6;
}

float voltLevelExpander(unsigned char channel, bool newVersion = true){
  return powerLevelExpander(channel, newVersion)/1000;
}

float voltLevelMain(){
  return powerLevelMain()/1000;
}

float voltLevelBackup(){
  return powerLevelBackup()/1000;
}

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
