#include "main.h"

int startingAngle = 0;

void startupLauncher(){
  startingAngle = analogRead(ANALOG_POT_LAUNCH);
}

void setLauncherMotors(int speed){
  motorSet(MOTOR_CHOO_R1,speed);
  motorSet(MOTOR_Y_CHOO_R23,speed);
  motorSet(MOTOR_Y_CHOO_L23,speed);
  motorSet(MOTOR_CHOO_L1,speed);
}

// Cancel any launcher task, disallow controller launching, and stop launcher
void stopAllLaunching() {
	stopTask(taskLaunch);
	stopTask(taskLauncherReset);
	setLauncherMotors(0);
	canLaunch = false;
}

void manageLauncher(){
  // Normal handling (right pad)
  if(joystickGetDigital(1, 7, JOY_DOWN)){ // If override is pressed
    if(joystickGetDigital(1, 8, JOY_RIGHT)) { // Backdrive launcher
        setLauncherMotors(127);
      } else if(joystickGetDigital(1, 8, JOY_UP)){ // Pull launcher down
        setLauncherMotors(-127);
      } else { // Stop launcher
        setLauncherMotors(0);
    }
  }
  // Launch using our automated tasks
  if(canLaunch && joystickGetDigital(1, 8, JOY_LEFT)){
    launch();
  }
  // Special handling (left pad)
  if(joystickGetDigital(1, 8, JOY_DOWN)) { // Emergency launcher stopping
    stopAllLaunching();
  }
  if (joystickGetDigital(1, 7, JOY_UP)) { // If test auton button is pressed
    if(joystickGetDigital(1, 7, JOY_RIGHT)){ // Reset launcher
      stopTask(taskLaunch);
      stopTask(taskLauncherReset);
      canLaunch = false;
      startTask( taskLauncherReset );
    }
    if(joystickGetDigital(1, 7, JOY_LEFT) && joystickGetDigital(1, 7, JOY_DOWN)){ // Force reset startingAngle
      startingAngle = analogRead(ANALOG_POT_LAUNCH);
    }
  }
}
