#include "main.h"
#include "math.h"

#define COCKED_POT_DIFFERENCE 1260  // How far from the starting value the arm should pull down
#define COCKED_POT_THRESHOLD 95 // How close we need to be to the target difference value to pass
#define LAUNCH_ACCURACY_COUNT 2 // How many times we need to see an upward change to count it as a launch
#define LAUNCH_STOP_COUNT 5 // How many times we need to see a minimal change over 15 milliseconds to count it as stopped launching
#define LAUNCH_INTAKE_THRESHOLD 100 // Within how much of the COCKED_POT_DIFFERENCE the intake is allowed to move again
#define LAUNCH_RESET_THRESHOLD 50 // Within how many ticks the launcher must be of the stating value to reset automatically
#define LAUNCH_STOP_THRESHOLD 5 // How much the launcher has to not move
#define LAUNCH_RELEASE_DIFFERENCE 750 // When the claw should release
#define CLAW_OPEN_AMOUNT 500 // How many encoder ticks that the claw opens

int startingAngle = 0;
int clawStartingAngle = 0;
bool canLaunch = true;
TaskHandle launchTask;
TaskHandle launchResetTask;

void startupLauncher(){
  startingAngle = analogRead(ANALOG_POT_LAUNCH);
  imeGet (IME_CLAW_L, &clawStartingAngle);
}

void setLauncherMotors(int speed){
  motorSet(MOTOR_CHOO_R1,speed);
  motorSet(MOTOR_Y_CHOO_R23,speed);
  motorSet(MOTOR_Y_CHOO_L23,speed);
  motorSet(MOTOR_CHOO_L1,speed);
}

void taskLauncherReset(void * ignored) {
	// Run launcher motors until we can see we're cocked
  setLauncherMotors(-127);
	int lastDifference = COCKED_POT_DIFFERENCE-digitalRead(ANALOG_POT_LAUNCH)+startingAngle;
	int lastValue = digitalRead(ANALOG_POT_LAUNCH);
	int stoppedCount = 0;
	while(true) {
		int currentAngle = digitalRead(ANALOG_POT_LAUNCH);
		// If we are close enough to target or seem to have launched, stop resetting
		// || (COCKED_POT_DIFFERENCE-SensorValue[potLauncher]) < lastDifference-15
		if (abs((COCKED_POT_DIFFERENCE+startingAngle)-currentAngle) <= COCKED_POT_THRESHOLD/*||SensorValue[limitLauncher]*/) // If the new difference is 100 in the back direction from the last
			break;
		if (abs((COCKED_POT_DIFFERENCE+startingAngle)-currentAngle) <= (COCKED_POT_THRESHOLD*3)) // If we're nearing the endpoint slow down
			setLauncherMotors(-90);
		lastDifference = COCKED_POT_DIFFERENCE-digitalRead(ANALOG_POT_LAUNCH)+startingAngle;
		//wait1Msec(15);
	}
	// Stop motors now that we have cocked the launcher arm
	setLauncherMotors(0);
	canLaunch = true;
}

void taskLaunch(void * ignored) {
	canLaunch = false;
  // Disable claw control
  suspendClawControl();

	// Run launcher motors until we can see that we've launched
	setLauncherMotors(-127);

	// Watch pot value until we can see it has definetly started going back up
	short changeCount = 0;
	int lastAngle = digitalRead(ANALOG_POT_LAUNCH);
	int launchPoint = LAUNCH_RELEASE_DIFFERENCE + startingAngle;
	digitalWrite(DIGITAL_LED, HIGH);
	delay(10);
	while(true) {
		int currentAngle = digitalRead(ANALOG_POT_LAUNCH);
		if (currentAngle-lastAngle<-5)
			changeCount+=1;
		if(changeCount == LAUNCH_ACCURACY_COUNT)
			break;
		lastAngle = digitalRead(ANALOG_POT_LAUNCH);
	}

	// Stop motors now that we have launched
  setLauncherMotors(0);
  digitalWrite(DIGITAL_LED,LOW);

	// Wait until we can see that the arm has stopped moving, then start resetting
	changeCount = 0;
  lastAngle = digitalRead(ANALOG_POT_LAUNCH);
	delay(10);
	while(true) {
		int currentAngle = digitalRead(ANALOG_POT_LAUNCH);
		if (digitalRead(ANALOG_POT_LAUNCH)<launchPoint)
      motorSet(MOTOR_CLAW_L,-127);
      motorSet(MOTOR_CLAW_R,127);
		if (abs(currentAngle-lastAngle) < 3)
			changeCount+=1;
		if(changeCount == LAUNCH_STOP_COUNT)
			break;
		lastAngle = digitalRead(ANALOG_POT_LAUNCH);
		delay(10);
	}

	//Don't reset the launcher if it is not up
	if(abs(startingAngle - digitalRead(ANALOG_POT_LAUNCH))<LAUNCH_RESET_THRESHOLD){
		//reset launcher
		startTask(taskLauncherReset);
	}

  int clawPosLeft;
  imeGet (IME_CLAW_L, &clawPosLeft);
  // left claw goes in negative direction when opening
  while(clawPosLeft > (startingAngle-CLAW_OPEN_AMOUNT)){
    imeGet (IME_CLAW_L, &clawPosLeft);
    delay(10);
  }

  motorSet(MOTOR_CLAW_L,0);
  motorSet(MOTOR_CLAW_R,0);

  //Reenable claw control
  resumeClawControl();
}

// Launch if we are currently allowed
void launch() {
	if (canLaunch)
  launchTask = taskCreate(taskLaunch, TASK_DEFAULT_STACK_SIZE,
    NULL, TASK_PRIORITY_DEFAULT);
	else
		print("Tried to launch but we aren't currently able to!");
}

// Cancel any launcher task, disallow controller launching, and stop launcher
void stopAllLaunching() {
	suspendTask(launchTask);
	suspendTask(launchResetTask);
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
      suspendTask(launchTask);
      suspendTask(launchResetTask);
      canLaunch = false;
      launchResetTask = taskCreate(taskLauncherReset, TASK_DEFAULT_STACK_SIZE,
        NULL, TASK_PRIORITY_DEFAULT);
    }
    if(joystickGetDigital(1, 7, JOY_LEFT) && joystickGetDigital(1, 7, JOY_DOWN)){ // Force reset startingAngle
      startingAngle = analogRead(ANALOG_POT_LAUNCH);
    }
  }
}
