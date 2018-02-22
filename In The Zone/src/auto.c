#include "main.h"
#include "math.h"

// KEEP IN MIND, AFTER AUTON, IF WE'RE PLUGGED IN SOME SENSORS WILL SHUT DOWN
// DON'T BE DUMB AND FORGET THIS UNTIL A COMPETITION. THAT WOULD BE BAD.
// From: Cameron, To: Cameron.

bool endBP = false; // End break point

double imperialToTick(double inches) {
  double conversion = (4 * acos(-1.0)) / (360);
  return inches / conversion;
}

///////////////////////// PID DUMMY FUNCTIONS //////////////////////////////////

// Setter from the motor speed PID loop with *double parameter*
void setMotorSpeedPID(double speed) {
  int speedInt = round(speed);
  driveRaw(speedInt, speedInt, speedInt, speedInt);
}

// Shuts off motors *without parameters* to be used with PID Loop
void shutDownMotors() { driveRaw(0, 0, 0, 0); }

// Gets the encoder value of a specific motor *without parameters*
// To be used in PID
double getEncoderValue() { return encoderGet(getEncoderBR()); }

void setEndBP(bool value) { endBP = value; }

void breakpoint() {
  while (!endBP)
    delay(15);
}

///////////////////////// PID DUMMY FUNCTIONS //////////////////////////////////

void autonomous() {
  /*
  ** Set up
  */
  initPID(); // Start PID

  ensureLiftTaskRunning();
  ensureIntakeTaskRunning();

  /*
  ** Run auton
  */
  switch (getAutonMode()) {
  // This is probably our most complex auton - it strategically does nothing
  case 1:
    print("Ran auton one!\n");
    break;

  // "Light" (RAM)
  case 2: {
    // TODO Tune and check sides
    int speedLeftCurve = getAutonPosition() ? 60 : 127;
    int speedRightcurve = getAutonPosition() ? 127 : 60;
    driveRaw(speedLeftCurve, speedLeftCurve, speedRightcurve, speedRightcurve);
    delay(1000); // TODO Maybe do some gyro check instead?
    driveRaw(127, 127, 127, 127);
    delay(4000);
    driveRaw(0, 0, 0, 0);
  } break;

  // 20 point & stationary goal
  case 3:
    //
    // /*
    //
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127);                 // Intake Preload
    runLiftAsync(300, true);                    // Raise the lift
    runMogoAsync(127, 500);                     // Put out the mogo intake
    pLoopDriveStraightAsync(1200, false, true); // Drive to the mogo
    delay(150);
    motorSet(MOTOR_CLAW, -25);
    waitForDriveStraight();
    runMogoSync(-127, 500); // Intake Mogo
    // Test this part first
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    pLoopTurnPointAsync(45 *
                        (getAutonPosition() ? 1 : -1)); // Turn to grab cone
    runLiftSync(0, true);                               // Lower the lift
    motorSet(MOTOR_CLAW, 127);                          // Extake the cone
    delay(100);
    runLiftSync(300, true);  // Raise the lift
    motorSet(MOTOR_CLAW, 0); // Stop dropping the cone
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    waitForTurnPoint();
    motorSet(MOTOR_CLAW, -25);  // Stall speed the goliath
    runIntakeSync(3600);        // Swing out the intake
    runLiftAsync(0, true);      // Lower the lift
    motorSet(MOTOR_CLAW, -127); // Intake the cone
    waitForLiftAsync();
    delay(100);
    motorSet(MOTOR_CLAW, -25); // Hold the cone in
    runLiftAsync(300, true);   // Lift the cone
    delay(100);                // Lift the cone
    pLoopTurnPointAsync(0);    // Turn the robot after grabbing cone
    waitForLiftAsync();        // Wait for the lift to raise the cone
    runIntakeAsync(1000);      // Pull in the intake
    waitForTurnPoint();
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraight(-1000, true, true);              // Approach the zones
    pLoopTurnPoint(45 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    pLoopDriveStraight(-850, true, true); // Line up with zone and goal
    runLiftAsync(600, true);              // Raise lift to stationary height
    pLoopTurnPoint(180 * (getAutonPosition() ? 1 : -1)); // Turn to 20pt zone
    runIntakeSync(3600); // Swing intake over stationary
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    waitForLiftAsync();
    setLiftSpeed(-80); // Lower lift onto stationary
    delay(200);
    setLiftSpeed(0);           // Stop the lift
    motorSet(MOTOR_CLAW, 127); // Extake the cone
    delay(200);
    setLiftSpeed(80); // Raise the lift
    delay(200);
    setLiftSpeed(0);         // Stop raising the lift
    motorSet(MOTOR_CLAW, 0); // Stop extaking the cone
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    driveRaw(127, 127, 127, 127); // Drive crazily towards the 20pt zone
    delay(500);
    runMogoAsync(127, 500); // Extake the mogo
    delay(500);
    driveRaw(-10, -10, -10, -10); // Slam on the breaks to get rid of the mogo
    delay(100);
    driveRaw(0, 0, 0, 0); // Stop the robot
    delay(200);
    driveRaw(-127, -127, -127, -127); // Back out of the zone
    delay(1000);
    driveRaw(0, 0, 0, 0);
    //
    // */
    //
    break;

  // 20 point
  case 4:
    //
    // /*
    //
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127);                 // Intake Preload
    runLiftAsync(300, true);                    // Raise the lift
    runMogoAsync(127, 500);                     // Put out the mogo intake
    pLoopDriveStraightAsync(1200, false, true); // Drive to the mogo
    delay(150);
    motorSet(MOTOR_CLAW, -25); // Set goliath to stall speed
    waitForDriveStraight();
    runMogoSync(-127, 500); // Intake Mogo
    // Test this part first
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    runLiftSync(0, true);      // Lower the lift
    motorSet(MOTOR_CLAW, 127); // Extake the cone
    delay(100);
    runLiftSync(300, true);  // Raise the lift
    motorSet(MOTOR_CLAW, 0); // Stop dropping the cone
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraight(-1000, true, true);              // Approach the zones
    pLoopTurnPoint(45 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    pLoopDriveStraight(-850, true, true); // Line up with zone and goal
    pLoopTurnPoint(180 * (getAutonPosition() ? 1 : -1)); // Turn to 20pt zone
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    driveRaw(127, 127, 127, 127); // Drive crazily towards the 20pt zone
    delay(500);
    runMogoAsync(127, 500); // Extake the mogo
    delay(500);
    driveRaw(-10, -10, -10, -10); // Slam on the breaks to get rid of the mogo
    delay(100);
    driveRaw(0, 0, 0, 0); // Stop the robot
    delay(200);
    driveRaw(-127, -127, -127, -127); // Back out of the zone
    delay(1000);
    driveRaw(0, 0, 0, 0);
    //
    // */
    //
    break;
  // 10 point & stationary goal
  case 5:
    //
    // /*
    //
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127);                 // Intake Preload
    runLiftAsync(300, true);                    // Raise the lift
    runMogoAsync(127, 500);                     // Put out the mogo intake
    pLoopDriveStraightAsync(1200, false, true); // Drive to the mogo
    delay(150);
    motorSet(MOTOR_CLAW, -25);
    waitForDriveStraight();
    runMogoSync(-127, 500); // Intake Mogo
    // Test this part first
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    pLoopTurnPointAsync(45 *
                        (getAutonPosition() ? 1 : -1)); // Turn to grab cone
    runLiftSync(0, true);                               // Lower the lift
    motorSet(MOTOR_CLAW, 127);                          // Extake the cone
    delay(100);
    runLiftSync(300, true);  // Raise the lift
    motorSet(MOTOR_CLAW, 0); // Stop dropping the cone
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    waitForTurnPoint();
    motorSet(MOTOR_CLAW, -25);  // Stall speed the goliath
    runIntakeSync(3600);        // Swing out the intake
    runLiftAsync(0, true);      // Lower the lift
    motorSet(MOTOR_CLAW, -127); // Intake the cone
    waitForLiftAsync();
    delay(100);
    motorSet(MOTOR_CLAW, -25); // Hold the cone in
    runLiftAsync(300, true);   // Lift the cone
    delay(100);                // Lift the cone
    pLoopTurnPointAsync(0);    // Turn the robot after grabbing cone
    waitForLiftAsync();        // Wait for the lift to raise the cone
    runIntakeAsync(1000);      // Pull in the intake
    waitForTurnPoint();
    // Test this next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraight(-1000, true, true);              // Approach the zones
    pLoopTurnPoint(45 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    runLiftAsync(600, true);              // Raise lift to stationary height
    pLoopDriveStraight(-350, true, true); // Line up with goal
    pLoopTurnPoint(90 * (getAutonPosition() ? 1 : -1)); // Turn to face goal
    pLoopDriveStraight(-300, false, true);              // Drive to goal
    runIntakeSync(3600); // Swing intake over stationary
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    waitForLiftAsync();
    setLiftSpeed(-80); // Lower lift onto stationary
    delay(200);
    setLiftSpeed(0);           // Stop the lift
    motorSet(MOTOR_CLAW, 127); // Extake the cone
    delay(200);
    setLiftSpeed(80); // Raise the lift
    delay(200);
    setLiftSpeed(0);         // Stop raising the lift
    motorSet(MOTOR_CLAW, 0); // Stop extaking the cone
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraight(300, false, true);
    pLoopTurnPoint(180 * (getAutonPosition() ? 1 : -1));
    driveRaw(80, 80, 80, 80); // Drive into zone. but only line up
    delay(1000);
    driveRaw(0, 0, 0, 0);
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    runMogoSync(-127, 500);       // Drop off mogo and cone
    driveRaw(100, 100, 100, 100); // Push in mogo
    delay(200);
    driveRaw(0, 0, 0, 0);
    delay(100);
    driveRaw(-127, -127, -127, -127); // Back off
    delay(500);
    driveRaw(0, 0, 0, 0);
    //
    // */
    //
    break;

  // 10 point
  case 6:
    //
    // /*
    //
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127);                 // Intake Preload
    runLiftAsync(300, true);                    // Raise the lift
    runMogoAsync(127, 500);                     // Put out the mogo intake
    pLoopDriveStraightAsync(1200, false, true); // Drive to the mogo
    delay(150);
    motorSet(MOTOR_CLAW, -25);
    waitForDriveStraight();
    runMogoSync(-127, 500); // Intake Mogo
    // Test this part first
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    runLiftSync(0, true);      // Lower the lift
    motorSet(MOTOR_CLAW, 127); // Extake the cone
    delay(100);
    runLiftSync(300, true);  // Raise the lift
    motorSet(MOTOR_CLAW, 0); // Stop dropping the cone
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraight(-1000, false, true);               // Approach the zones
    pLoopTurnPoint(-180 * (getAutonPosition() ? 1 : -1)); // Turn to 10pt zone
    pLoopDriveStraight(200, false, true);                 // Approach zone
    driveRaw(60, 60, 80, 80); // Drive into zone. but only line up
    delay(1000);
    driveRaw(0, 0, 0, 0);
    // Test this part next
    breakpoint(); // --------------------- BREAK POINT -------------------------
    //
    // */
    //
    //
    // /*
    //
    runMogoSync(-127, 500);       // Drop off mogo and cone
    driveRaw(100, 100, 100, 100); // Push in mogo
    delay(200);
    driveRaw(0, 0, 0, 0);
    delay(100);
    driveRaw(-127, -127, -127, -127); // Back off
    delay(500);
    driveRaw(0, 0, 0, 0);
    //
    // */
    //
    break;

  // Programming Skills
  case 7:
    break;

  // Stationary only
  case 8:
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127); // Pull in preload
    runLiftAsync(600, true);    // Run lift to stationary height
    delay(150);
    motorSet(MOTOR_CLAW, -25);            // Hold preload
    pLoopDriveStraight(500, false, true); // Drive to stationary
    runIntakeSync(3600);                  // Send intake over stationary
    motorSet(MOTOR_CLAW, 127);            // Extake cone
    delay(100);
    setLiftSpeed(127); // Raise lift off the stationary
    delay(300);
    setLiftSpeed(0);
    motorSet(MOTOR_CLAW, 0);
    driveRaw(-127, -127, -127, -127); // Back away from the stationary
    delay(150);
    driveRaw(0, 0, 0, 0);

  // Custom
  case 9:
    pLoopDriveStraightSync(1000, false, true);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    pLoopTurnPointSync(90);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    runMogoSync(127, 500);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    runIntakeSync(3600);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    runLiftSync(300, true);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    break;

  default:
    print("Ran auton that wasn't given a case!");
  }

  /*
  ** Clean up
  */
  if (isOnline()) {          // Shut down sensors if at a comp
    gyroShutdown(getGyro()); // Disable our gyro
  }

  shutdownPID(); // Kill PID
}
