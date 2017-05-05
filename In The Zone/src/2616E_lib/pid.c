#include "main.h"

#define PID_LOOP_COUNT 1

// functions to be called upon starting the pid loop
initFunction initializers[PID_LOOP_COUNT];

// functions to get the current values
getCurrentValFunction valueGetters[PID_LOOP_COUNT];

// functions to send current values to do whatever with them
setCurrentValFunction valueSetters[PID_LOOP_COUNT];

// functions to run upon completing loop
PIDIteratorCallbackFunction callbacks[PID_LOOP_COUNT];

double targets[PID_LOOP_COUNT];        // target values
double pConsts[PID_LOOP_COUNT];        // P multipliers
double iConsts[PID_LOOP_COUNT];        // I multipliers
double dConsts[PID_LOOP_COUNT];        // D multipliers
double integralLimits[PID_LOOP_COUNT]; // max value for integrals
double lastIntegral[PID_LOOP_COUNT];   // last I value of loop
double lastError[PID_LOOP_COUNT];      // last error value of loop
double accThresh[PID_LOOP_COUNT];  // range of values accepted to end pid loop
unsigned int initializedLoops = 0; // how many loops have already been created
TaskHandle pidTask;

bool loopActive[PID_LOOP_COUNT]; // whether or not to run the loop

// calculates and executes PID control for one loop
// for use as helper function, not called outside
// PARAMETERS:
// int: the index of the loop to run
// RETURNS:
// bool: whether or not the loop is done
bool runPID(int index) {
  // get the necessary variables
  double current = (*valueGetters[index])();
  double target = targets[index];
  double Kp = pConsts[index];
  double Ki = iConsts[index];
  double Kd = dConsts[index];
  double maxIntegral = integralLimits[index];
  double lastI = lastIntegral[index];
  double lastErr = lastError[index];
  double thresh = accThresh[index];

  // calculate the error
  double error = target - current;

  // if close enough, break out
  if (abs(error) < thresh) {
    callbacks[index]();
    return true;
  }

  // Proportional -
  // just the error times a constant later

  // Integral -
  // Sum of the previous errors within reasonable conditions
  if (Ki != 0.0) {
    // only add if within range
    if (abs(error) < maxIntegral)
      lastI += error;
    else
      lastI = 0;
  } else
    lastI = 0;
  lastIntegral[index] = lastI;

  // Derivitive -
  // rate of change at current time
  double der = error - lastErr;
  lastError[index] = error;

  //                 P       +       I      +      D
  double calc = (error * Kp) + (lastI * Ki) + (der * Kd);
  printf("-----------------\ncalc: %lf\n\n", calc);

  // send the value to user defined function
  valueSetters[index](calc);

  // has to run again
  return false;
}

// Controls all pid loops in a task
void pidLoop(void *param) {
  while (true) {
    for (int i = 0; i < PID_LOOP_COUNT; i++) {
      if (loopActive[i]) {
        loopActive[i] = !runPID(i);
      }
    }
    delay(50);
  }
}

// Begin executing a specific pid loop
// To be called by user (public)
// PARAMETERS:
// unsigned int: index of loop to activate
// double: target for pid to strive for
void startPIDLoop(unsigned int index, double target) {
  // reset used variables
  lastError[index] = 0;
  lastIntegral[index] = 0;

  // set target value
  targets[index] = target;

  print("starting\n");
  delay(1000);
  // run initialization function
  initializers[index]();

  // activate loop
  loopActive[index] = true;
}

// Force terminate a specific pid loop
// To be called by user (public)
// PARAMETERS:
// unsigned int: index of loop to terminate
// bool: whether or not to run callback on termination
void stopPIDLoop(unsigned int index, bool runCallback) {
  // deactivate loop
  loopActive[index] = false;

  // reset used variables
  lastError[index] = 0;
  lastIntegral[index] = 0;

  // run callback
  if (runCallback) {
    // this could be useful say to shut off the motors
    callbacks[index]();
  }
}

// Sets PID loop at specific index
// Should be used to replace an existing loop,
//  to create new use addPIDLoop
// To be called by user (public)
// PARAMETERS:
// unsigned int: index of the loop to add
// getCurrentValFunction: function determining how to get the current value (EX.
// Encoder reading)
// setCurrentValFunction: function determining what to do with pid output (EX.
// power motors)
// PIDIteratorCallbackFunction: function that runs when the loop is done
// executing (EX. turn off motors)
// double: proportional multiplier
// double: integral multiplier
// double: derivitive multiplier
// double: max integral limit before reset
// double: range of accepted values to terminate loop, should be relatively low,
// depending on what you are measuring
int setPIDLoop(unsigned int index, initFunction init,
               getCurrentValFunction valueGetter,
               setCurrentValFunction valueSetter,
               PIDIteratorCallbackFunction PIDIteratorCallback, double kP,
               double kI, double kD, double integralLimit, double threshold) {
  if (index < PID_LOOP_COUNT) { // still can create loops (under max)
    // initialize "instance data" (I really like OOP)
    // probably should have used structs, but this should work
    initializers[index] = init;
    valueGetters[index] = valueGetter;
    valueSetters[index] = valueSetter;
    callbacks[index] = PIDIteratorCallback;
    pConsts[index] = kP;
    iConsts[index] = kI;
    dConsts[index] = kD;
    integralLimits[index] = integralLimit;
    accThresh[index] = threshold;
    return index;
  } else { // tried to create loop, but already too many
    print("[ELib] ERROR: Tried to create another PID loop, but already too "
          "many created\n");
    print("[ELib] Try increasing the PID_LOOP_COUNT in pid.c if you did this "
          "on purpose\n");
    return -1;
  }
}

// Adds PID loop to existing loops
// To be called by user (public)
// PARAMETERS:
// getCurrentValFunction: function determining how to get the current value (EX.
// Encoder reading)
// setCurrentValFunction: function determining what to do with pid output (EX.
// power motors)
// PIDIteratorCallbackFunction: function that runs when the loop is done
// executing (EX. turn off motors)
// double: proportional multiplier
// double: integral multiplier
// double: derivitive multiplier
// double: max integral limit before reset
// double: range of accepted values to terminate loop, should be relatively low,
// depending on what you are measuring
int addPIDLoop(initFunction init, getCurrentValFunction valueGetter,
               setCurrentValFunction valueSetter,
               PIDIteratorCallbackFunction PIDIteratorCallback, double kP,
               double kI, double kD, double integralLimit, double threshold) {
  int index =
      setPIDLoop(initializedLoops, init, valueGetter, valueSetter,
                 PIDIteratorCallback, kP, kI, kD, integralLimit, threshold);
  if (index != -1) {
    initializedLoops++;
  }
  return index;
}

// Resets PID loop to have all zero values
// To be called by user (public)
// PARAMETERS:
//  int: the index of the loop to be reset
void resetPIDLoop(int index) {
  targets[index] = 0.0;
  pConsts[index] = 2.0;
  iConsts[index] = 0.04;
  dConsts[index] = 0.0;
  integralLimits[index] = 50.0;
  lastIntegral[index] = 0.0;
  lastError[index] = 0.0;
  accThresh[index] = 12.0;
  loopActive[index] = false;
}

void waitForPID(int index) {
  while (loopActive[index])
    delay(100);
}

// Starts up the PID task, and initializes some variables
void initPID() {
  print("[ELib] Initializing PID control task\n");
  // initialize some values
  for (int i = 0; i < PID_LOOP_COUNT; i++) {
    lastError[i] = 0;
    lastIntegral[i] = 0;
    loopActive[i] = false;
    resetPIDLoop(i);
  }
  if (pidTask == NULL)
    pidTask = taskCreate(pidLoop, TASK_DEFAULT_STACK_SIZE, NULL,
                         TASK_PRIORITY_DEFAULT); // start the pid loop
}

void shutdownPID() {
  print("[ELib] Killing PID control task\n");
  if (pidTask != NULL) {
    taskDelete(pidTask); // Kill the PID task
    pidTask = NULL;
  }
}
