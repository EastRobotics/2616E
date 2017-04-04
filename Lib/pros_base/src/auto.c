#include "main.h"
#include "math.h"

// KEEP IN MIND, AFTER AUTON, IF WE'RE PLUGGED IN SOME SENSORS WILL SHUT DOWN
// DON'T BE DUMB AND FORGET THIS UNTIL A COMPETITION. THAT WOULD BE BAD.
// From: Cameron, To: Cameron.

#define WHITE_THRESH 200 // Value to accept of the light sensor as the white line
#define LINE_BREAK_TIME 75 // Milliseconds for the brake to last for on line up

double KP = 2.0;
double KI = 0.04;
double KD = 0.0;

// Gets the encoder value of a specific motor *without parameters*
// To be used in PID
double getEncoderValue() { return encoderGet(getEncoderBR()); }

// Setter from the motor speed PID loop with *double parameter*
void setMotorSpeedPID(double speed) {
  int speedInt = round(speed);
  driveRaw(speedInt, speedInt, speedInt, speedInt);
}

bool isWhite(bool right){
  return (right ? analogRead(ANALOG_LINE_RIGHT) : analogRead(ANALOG_LINE_LEFT)) < WHITE_THRESH;
}

void driveToLine(int speed, bool forwards) {
    speed = abs(speed) * (forwards ? 1 : -1);
    int speedLeft = speed, speedRight = speed;
    int rightSightings = 0, leftSightings = 0;
    int lastSpeedLeft = speed, lastSpeedRight = speed; // Basically speedRight & speedLeft, but never set to 0
    while(!(isWhite(true) && isWhite(false))) {
        if(isWhite(true)) {
            if(speedRight!=0) {
                speedRight = 0;
                rightSightings++;
            }
        } else {
            speedRight = ((rightSightings % 2==0) ? speed : speed*-1);
            speedRight = (rightSightings > 0 ? speedRight/2 : speedRight);
            lastSpeedRight = speedRight;
        }

        if(isWhite(false)) {
            if(speedLeft!=0) {
                speedLeft = 0;
                leftSightings++;
            }
        } else {
            speedLeft = ((leftSightings % 2 == 0) ? speed : speed*-1);
            speedLeft = (leftSightings > 0 ? speedLeft/2 : speedLeft);
            lastSpeedLeft = speedLeft;
        }

        driveRaw(speedLeft,speedLeft,speedRight,speedRight);
    }
  driveRaw(lastSpeedLeft*-1,lastSpeedLeft*-1, lastSpeedRight*-1, lastSpeedRight*-1);
  delay(100);
  driveRaw(0, 0, 0, 0);
}

// Shuts off motors *without parameters* to be used with PID Loop
void shutDownMotors() { driveRaw(0, 0, 0, 0); }

void driveForTime(int speedLeft, int speedRight, int delayTime) {
  driveRaw(speedLeft, speedLeft, speedRight, speedRight);
  delay(delayTime);
  driveRaw(0,0,0,0);
}

// Waste of task, should only be initialized in a bad auton that uses timed driving
TaskHandle timeDrive;
bool timeDriveRunning = false;
int timeDriveSpeedLeft = 0, timeDriveSpeedRight = 0;
int timeDriveDelay = 0, timeDriveCount = 0;
const int timeDriveDelayRate = 15;
void timeDriveAsynch(void * params) {
  while(true) {
    if(timeDriveRunning) {
      if(timeDriveCount >= timeDriveDelay) {
        driveRaw(0,0,0,0);
        timeDriveRunning = false;
        timeDriveCount = 0;
      } else {
        driveRaw(timeDriveSpeedLeft, timeDriveSpeedLeft, timeDriveSpeedRight, timeDriveSpeedRight);
        timeDriveCount += timeDriveDelayRate;
      }
    }
    delay(timeDriveDelayRate);
  }
}

void waitForTimeDrive() {
  while(timeDriveRunning)
    delay(15);
}

void initTimeDriveTask() {
  if(timeDrive == NULL)
    timeDrive = taskCreate(timeDriveAsynch, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void startTimeDriveTask() {
  if(timeDrive==NULL)
    initTimeDriveTask();
  if(taskGetState(timeDrive) != TASK_RUNNING)
    taskResume(timeDrive);
}

void stopTimeDriveTask() {
  if(timeDrive==NULL)
    initTimeDriveTask();
  if(taskGetState(timeDrive) != TASK_SUSPENDED)
    taskSuspend(timeDrive);
}

void deleteTimeDriveTask() {
  if(timeDrive!=NULL)
    taskDelete(timeDrive);
}

void setTimeDrive(int speedL, int speedR, int delay) {
  timeDriveSpeedLeft = speedL;
  timeDriveSpeedRight = speedR;
  timeDriveDelay = delay;
  timeDriveRunning = true;
  startTimeDriveTask();
}

void breakpoint() {
  while(digitalRead(DIGITAL_BREAKPOINT))
    delay(20);
}

void autonomous() {
  initPID();
  setClawMode(1); // Give auton claw control
  initLiftTask();
  int sideMult = getAutonPosition() ? -1 : 1;

  switch (getAutonMode()) {
  case 1:
    print("Ran auton one, so nothing is happening");
    break;
  case 2:
    print("Ran auton two!");
    addPIDLoop(getEncoderValue, setMotorSpeedPID, shutDownMotors, KP, KI, KD,
               50.0, 12.0);
    //startPIDLoop(0, 500.0);
    //pidDriveStraight(2000);
    //waitForPid();
    break;
  case 3:
    print("Ran auton three!");
    setClawTarget(350);
    waitForClaw();
    clawClose(1000);
    break;
  case 4:
    print("Ran auton four!");
    driveToLine(60,true);
    break;
  case 5:
    print("Ran auton five!");
    initTimeDriveTask();
    setTimeDrive(127, 127, 500);
    waitForTimeDrive();
    deleteTimeDriveTask();
    break;
  case 6:
    print("Ran auton six!");
    setHoldUp(true);
    driveForTime(100,100,300);
    setClawTarget(250);
    //delay(100);
    //driveForTime(100,100,300);
    //driveForTime(-100,-100,300);
    waitForClaw();
    delay(200);
    setLiftMotors(-40);
    delay(50);
    setLiftMotors(0);
    driveForTime(100, 100, 500);
    clawClose(1000);
    setLift(1200, 95);
    waitForLift();
    driveForTime(-100*sideMult, 100*sideMult, 500);
    driveForTime(-80,-80,300);
    delay(500);
    driveToLine(60,true);
    driveForTime(-127,-127,500);
    setLift(2200,127);
    waitForLift();
    setHoldUp(false);
    setClawTarget(300);
    waitForClaw();
    // Lower lift and go for stars
    setLift(getLiftStartAngle(),127);
    driveForTime(60 * sideMult,-60 * sideMult, 100);
    driveToLine(60,true);
    setClawTarget(400);
    waitForLift();
    setClawTarget(-50);
    driveForTime(100,100,300);
    delay(1000);
    stopClaw();
    setLift(1200,127);
    delay(100);
    driveForTime(-100,-100,800);
    delay(100);
    setLift(2200,127);
    waitForLift();
    setClawTarget(300);
    waitForClaw();
    break;
  case 7:
    print("Ran auton seven!");
    setHoldUp(true);
    // Back up and deploy the claw
    driveForTime(-100,-100,200);
    setClawTarget(200);
    waitForClaw();
    driveForTime(100,100,100);
    driveForTime(-100,-100,50);
    breakpoint(); //////////////////////////////////////////////////////////////
    delay(200);
    // Drive to and grab corner star
    driveForTime(100,100,500);
    delay(200);
    clawClose(750);
    breakpoint(); //////////////////////////////////////////////////////////////
    delay(350);
    setLift(1500,90);
    waitForLift();
    breakpoint(); //////////////////////////////////////////////////////////////
    delay(1000);
    // Turn and drive to fence
    driveForTime(-127*sideMult,127*sideMult,300);
    driveForTime(-60,-60,200);
    breakpoint(); //////////////////////////////////////////////////////////////
    driveToLine(80,false);
    delay(100);
    breakpoint(); //////////////////////////////////////////////////////////////
    driveForTime(-127,-127,500);
    breakpoint(); //////////////////////////////////////////////////////////////
    // Dump over fence
    setLift(2200,127);
    waitForLift();
    setClawTarget(300);
    setHoldUp(false);
    breakpoint(); //////////////////////////////////////////////////////////////
    break;
  case 8:
    print("Ran auton eight!");
    setHoldUp(true);
    // Drive to line and raise lift
    driveForTime(-100, -100, 300);
    setClawTarget(325);
    waitForClaw();
    setLift(1400, 80);
    driveToLine(70,false);
    delay(1000);
    waitForLift();
    setLift(1400, 30);
    // Finish raising lift and driving to fence
    setLift(2400,80);
    waitForLift();
    driveForTime(-100,-100,600);
    delay(2000);
    clawClose(1000);
    delay(1000);
    setClawTarget(300);
    waitForClaw();
    setHoldUp(false);
    break;
  case 9:
    print("Ran auton nine!");
    setHoldUp(true);
    // Prepare to loop grabbing and dumping game objects
    driveToLine(60,false);
    setClawTarget(250);
    waitForClaw();
    delay(500);
    // loop grabbing and dumping game objects
    for(int i = 0; i < 3; i++) {
      driveForTime(100,100,((i==0) ? 275 : 225));
      clawClose(500);
      delay(800);
      setLift(1400,100);
      waitForLift();
      delay(500);
      driveToLine(60,false);
      waitForLift();
      if(i==2) {
        driveForTime(-100,100,200);
      }
      delay(300);
      driveForTime(-100, -100, ((i!=2) ? 400 : 500));
      delay(300);
      setLift(2200, 127);
      waitForLift();
      setClawTarget(250);
      waitForClaw();
      setLift(getLiftStartAngle(), 127);
      if(i==2) {
        driveForTime(100,-100,500);
      } else {
        driveToLine(75,true);
      }
    }
    // Go for cube then stars
    driveForTime(100, 100, 300);
    delay(1000);
    clawClose(500);
    driveForTime(-100,-100,200);
    delay(100);
    driveForTime(100 * sideMult,-100 * sideMult,500);
    delay(100);
    driveForTime(-100,-100,200);
    setLift(2000,127);
    waitForLift();
    setClawTarget(300);
    waitForClaw();
    setLift(getLiftStartAngle(),127);
    waitForLift();
    driveForTime(100,100,200);
    delay(100);
    driveForTime(-100,100,500);
    delay(100);
    driveToLine(60, true);
    delay(300);
    // Go for the stars
    breakpoint(); //////////////////////////////////////////////////////////////
    setClawTarget(-50);
    driveForTime(100,100,300);
    delay(1000);
    stopClaw();
    setLift(1200,127);
    breakpoint(); //////////////////////////////////////////////////////////////
    delay(100);
    driveForTime(-100,-100,800);
    delay(100);
    breakpoint(); //////////////////////////////////////////////////////////////
    setLift(2200,127);
    waitForLift();
    setClawTarget(300);
    waitForClaw();
    setHoldUp(false);
    break;
  default:
    print("Ran auton that wasn't given a case!");
  }

  shutdownPID();
  stopClaw(); // Set the claw to the current position
  setClawMode(0);
  deleteLiftTask();
  // Should be run after all autons
  if (isOnline()) {
    gyroShutdown(getGyro()); // Disable our gyro
  }
}
