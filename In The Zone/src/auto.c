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
  int sideMult = (getAutonPosition() ? 1 : -1);

  /*
  ** Run auton
  */
  switch (getAutonMode()) {
  // This is probably our most complex auton - it strategically does nothing
  case 1:
    print("Ran auton one!\n");
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
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
  case 3: {
    //
    // /*
    //
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127); // Intake Preload
    runLiftAsync(300, true);    // Raise the lift
    runMogoAsync(127, 1075);    // Put out the mogo intake
    delay(200);
    pLoopDriveStraightAsync(1250 * 2, false, true); // Drive to the mogo
    delay(150);
    motorSet(MOTOR_CLAW, -25); // Set goliath to stall speed
    waitForDriveStraight();
    int tickDiff = encoderGet(getEncoderBR());
    driveRaw(60, 60, 60, 60);
    delay(50);
    runMogoAsync(-127, 1075); // Intake Mogo
    delay(150);
    driveRaw(0, 0, 0, 0);
    tickDiff -= encoderGet(getEncoderBR());
    waitForMogoAsync();
    //
    // */
    //
    //
    // /*
    //
    runLiftSync(0, true);      // Lower the lift
    motorSet(MOTOR_CLAW, 127); // Extake the cone
    delay(100);
    runLiftSync(150, true);  // Raise the lift
    motorSet(MOTOR_CLAW, 0); // Stop dropping the cone
    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraight((-1300 * 2) + tickDiff, false,
                       true); // Approach the zones
    // Kick off the turn
    driveRaw(-127 * sideMult, -127 * sideMult, 127 * sideMult, 127 * sideMult);
    delay(250);
    pLoopTurnPoint(45 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    driveRaw(-127, -127, -127, -127);                   // kick the movment
    delay(75);
    pLoopDriveStraight(-380 * 2, false, true);           // Line up with cones
    pLoopTurnPoint(180 * (getAutonPosition() ? 1 : -1)); // Line up with cones
    runIntakeAsync(3550);
    runLiftSync(0, true);       // Lift down
    motorSet(MOTOR_CLAW, -127); // Intake cones
    delay(200);
    // tickDiff = encoderGet(getEncoderBR());
    driveRaw(-127, -127, -127, -127);
    delay(150);                             // Kick the motion
    pLoopDriveStraight(-820, false, false); // Go for cone
    delay(200);
    motorSet(MOTOR_CLAW, -20); // Hold cones
    runLiftAsync(200, true);
    runIntakeAsync(2000);
    // tickDiff -= encoderGet(getEncoderBR());
    // pLoopDriveStraight(tickDiff, false, false);
    driveRaw(127, 127, 127, 127); // Kick the motion
    delay(150);
    pLoopDriveStraight(960, false, false);              // Go back
    pLoopTurnPoint(45 * (getAutonPosition() ? 1 : -1)); // Turn back
    driveRaw(-127, -127, -127, -127);
    delay(75);
    pLoopDriveStraight(-150 * 2, false, true); // Line up with zone and goal
    pLoopTurnPoint(135 * (getAutonPosition() ? 1 : -1)); // Turn to 20pt zone
    runIntakeSync(3550);                                 // Stack cone
    //
    // */
    //
    //
    // /*
    //
    // Let go of cone
    motorSet(MOTOR_CLAW, 127); // Release cones
    runLiftAsync(350, true);
    delay(200);
    runIntakeAsync(1200);
    runLiftAsync(100, true);
    driveRaw(127, 127, 127, 127); // Drive crazily towards the 20pt zone
    delay(500);
    runMogoAsync(127, 500); // Extake the mogo
    delay(300);
    driveRaw(-10, -10, -10, -10); // Slam on the breaks to get rid of the mogo
    delay(100);
    driveRaw(0, 0, 0, 0); // Stop the robot
    // delay(200);
    /// driveRaw(-127, -127, -127, -127); // Back out of the zone
    // delay(800);
    // driveRaw(0, 0, 0, 0);
    //
    // */
  } break;

  // 20 point
  case 4: {
    /*
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127); // Intake Preload
    runLiftAsync(300, true);    // Raise the lift
    runMogoAsync(127, 1075);    // Put out the mogo intake
    delay(200);
    pLoopDriveStraightAsync(1250 * 2, false, true); // Drive to the mogo
    delay(100);
    motorSet(MOTOR_CLAW, -25); // Set goliath to stall speed
    waitForDriveStraight();
    int tickDiff = encoderGet(getEncoderBR());
    driveRaw(60, 60, 60, 60);
    delay(50);
    runMogoAsync(-127, 1075); // Intake Mogo
    delay(150);
    driveRaw(0, 0, 0, 0);
    tickDiff -= encoderGet(getEncoderBR());
    waitForMogoAsync();
    if (abs(gyroGet(getGyro())) <= 5 && abs(gyroGet(getGyro())) >= 3) {
      if ((gyroGet(getGyro()) > 0)) {
        driveRaw(60 * sideMult, 60 * sideMult, -60 * sideMult, -60 * sideMult);
      } else {
        driveRaw(-60 * sideMult, -60 * sideMult, 60 * sideMult, 60 * sideMult);
      }
      delay(50);
      driveRaw(0, 0, 0, 0);
    }

    //
    // /
    //
    //
    // /*
    //
    pLoopDriveStraightAsync((-1225 * 2) + tickDiff, false,
                            true); // Approach the zones
    runLiftSync(50, true);         // Lower the lift
    motorSet(MOTOR_CLAW, 127);     // Extake the cone
    delay(100);
    runLiftAsync(200, true); // Raise the lift
    //
    //
    //
    waitForDriveStraight();
    motorSet(MOTOR_CLAW, 0); // Stop dropping the cone
    breakpoint();
    */
    pLoopTurnPointSync(-145 * sideMult); // Turn to cone
    delay(100);
    runIntakeSync(3600);
    motorSet(MOTOR_CLAW, -127); // Grab cone
    runLiftSync(50, true);
    driveRaw(-60, -60, -60, -60); // Drive into cone
    delay(250);
    driveRaw(0, 0, 0, 0);
    motorSet(MOTOR_CLAW, -25); // Stall speed goliath
    breakpoint();
    delay(50);
    driveRaw(60, 60, 60, 60); // Backup
    runLiftAsync(200, true);  // Pick up cone
    delay(250);
    driveRaw(0, 0, 0, 0);
    waitForLift();
    // Kick off the turn
    driveRaw(127 * sideMult, 127 * sideMult, -127 * sideMult, -127 * sideMult);
    delay(250);
    pLoopTurnPointAsync(-45 *
                        (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    // SPECIFIC FOR SCORING ON NOGO
    runIntakeSync(1000);   // Pull in cone
    runLiftSync(50, true); // Lower lift
    waitForTurnPoint();
    pLoopDriveStraightAsync(-700 * 2, false,
                            true); // Line up with zone and goal
    motorSet(MOTOR_CLAW, 127);
    delay(100);
    runLiftAsync(200, true); // Raise lift
    waitForDriveStraight();
    pLoopTurnPoint(-135 * (getAutonPosition() ? 1 : -1)); // Turn to 20pt zone
    motorSet(MOTOR_CLAW, 0);
    breakpoint();
    //
    //
    //
    driveRaw(127, 127, 127, 127); // Drive crazily towards the 20pt zone
    delay(750);
    runMogoAsync(127, 500); // Extake the mogo
    delay(300);
    driveRaw(-10, -10, -10, -10); // Slam on the breaks to get rid of the mogo
    delay(100);
    driveRaw(0, 0, 0, 0); // Stop the robot
    // delay(200);
    /// driveRaw(-127, -127, -127, -127); // Back out of the zone
    // delay(800);
    // driveRaw(0, 0, 0, 0);
    //
    // */
  } break;
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
  case 6: {
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127); // Intake Preload
    runLiftAsync(300, true);    // Raise the lift
    runMogoAsync(127, 1075);    // Put out the mogo intake
    delay(200);
    pLoopDriveStraightAsync(1250 * 2, false, true); // Drive to the mogo
    delay(100);
    motorSet(MOTOR_CLAW, -25); // Set goliath to stall speed
    waitForDriveStraight();
    int tickDiff = encoderGet(getEncoderBR());
    driveRaw(60, 60, 60, 60);
    delay(50);
    runMogoAsync(-127, 1075); // Intake Mogo
    delay(150);
    driveRaw(0, 0, 0, 0);
    tickDiff -= encoderGet(getEncoderBR());
    waitForMogoAsync();
    if (abs(gyroGet(getGyro())) <= 5 && abs(gyroGet(getGyro())) >= 3) {
      if ((gyroGet(getGyro()) > 0)) {
        driveRaw(60 * sideMult, 60 * sideMult, -60 * sideMult, -60 * sideMult);
      } else {
        driveRaw(-60 * sideMult, -60 * sideMult, 60 * sideMult, 60 * sideMult);
      }
      delay(50);
      driveRaw(0, 0, 0, 0);
    }

    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraightAsync((-1225 * 2) + tickDiff, false,
                            true); // Approach the zones
    runLiftSync(50, true);         // Lower the lift
    motorSet(MOTOR_CLAW, 127);     // Extake the cone
    delay(100);
    runLiftAsync(200, true); // Raise the lift
    //
    // */
    //
    //
    // /*
    //
    waitForDriveStraight();
    // Kick off the turn
    driveRaw(127 * sideMult, 127 * sideMult, -127 * sideMult, -127 * sideMult);
    delay(250);
    pLoopTurnPoint(-180 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    //
    // */
    //
    //
    // /*
    //
    driveRaw(127, 127, 127, 127); // Drive crazily towards the 20pt zone
    delay(700);
    // driveRaw(60, 60, 100, 100); // Swerve into line with 10pt zone
    // delay(400);
    driveRaw(0, 0, 0, 0);
    runMogoSync(127, 1200); // Extake the mogo
    delay(100);
    driveRaw(-80, -80, -80, -80); // Cack out after scoring
    delay(300);
    driveRaw(0, 0, 0, 0); // Stop the robot
    // delay(200);
    /// driveRaw(-127, -127, -127, -127); // Back out of the zone
    // delay(800);
    // driveRaw(0, 0, 0, 0);
    //
    // */
  } break;

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
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    pLoopTurnPointSync(90);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    runMogoSync(127, 500);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    runIntakeSync(3600);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    runLiftSync(300, true);
    breakpoint(); // --------------------- BREAK POINT -------------------------
    break;

  // 20 pt - 1 cone
  case 10: {
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127); // Intake Preload
    runLiftAsync(300, true);    // Raise the lift
    runMogoAsync(127, 1075);    // Put out the mogo intake
    delay(200);
    pLoopDriveStraightAsync(1250 * 2, false, true); // Drive to the mogo
    delay(100);
    motorSet(MOTOR_CLAW, -25); // Set goliath to stall speed
    waitForDriveStraight();
    int tickDiff = encoderGet(getEncoderBR());
    driveRaw(60, 60, 60, 60);
    delay(50);
    runMogoAsync(-127, 1075); // Intake Mogo
    delay(150);
    driveRaw(0, 0, 0, 0);
    tickDiff -= encoderGet(getEncoderBR());
    waitForMogoAsync();
    if (abs(gyroGet(getGyro())) <= 5 && abs(gyroGet(getGyro())) >= 3) {
      if ((gyroGet(getGyro()) > 0)) {
        driveRaw(60 * sideMult, 60 * sideMult, -60 * sideMult, -60 * sideMult);
      } else {
        driveRaw(-60 * sideMult, -60 * sideMult, 60 * sideMult, 60 * sideMult);
      }
      delay(50);
      driveRaw(0, 0, 0, 0);
    }

    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraightAsync((-1225 * 2) + tickDiff, false,
                            true); // Approach the zones
    runLiftSync(50, true);         // Lower the lift
    motorSet(MOTOR_CLAW, 127);     // Extake the cone
    delay(100);
    runLiftAsync(200, true); // Raise the lift
    //
    // */
    //
    //
    // /*
    //
    waitForDriveStraight();
    motorSet(MOTOR_CLAW, 0); // Stop dropping the cone
    // Kick off the turn
    driveRaw(127 * sideMult, 127 * sideMult, -127 * sideMult, -127 * sideMult);
    delay(250);
    pLoopTurnPoint(-45 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    pLoopDriveStraight(-700 * 2, false, true); // Line up with zone and goal
    pLoopTurnPoint(-135 * (getAutonPosition() ? 1 : -1)); // Turn to 20pt zone
    //
    // */
    //
    //
    // /*
    //
    driveRaw(127, 127, 127, 127); // Drive crazily towards the 20pt zone
    delay(750);
    runMogoAsync(127, 500); // Extake the mogo
    delay(300);
    driveRaw(-10, -10, -10, -10); // Slam on the breaks to get rid of the mogo
    delay(100);
    driveRaw(0, 0, 0, 0); // Stop the robot
    // delay(200);
    /// driveRaw(-127, -127, -127, -127); // Back out of the zone
    // delay(800);
    // driveRaw(0, 0, 0, 0);
    //
    // */
  } break;
  // 10 point single cone
  case 11: {
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127); // Intake Preload
    runLiftAsync(300, true);    // Raise the lift
    runMogoAsync(127, 1075);    // Put out the mogo intake
    delay(200);
    pLoopDriveStraightAsync(1250 * 2, false, true); // Drive to the mogo
    delay(100);
    motorSet(MOTOR_CLAW, -25); // Set goliath to stall speed
    waitForDriveStraight();
    int tickDiff = encoderGet(getEncoderBR());
    driveRaw(60, 60, 60, 60);
    delay(50);
    runMogoAsync(-127, 1075); // Intake Mogo
    delay(150);
    driveRaw(0, 0, 0, 0);
    tickDiff -= encoderGet(getEncoderBR());
    waitForMogoAsync();
    if (abs(gyroGet(getGyro())) <= 5 && abs(gyroGet(getGyro())) >= 3) {
      if ((gyroGet(getGyro()) > 0)) {
        driveRaw(60 * sideMult, 60 * sideMult, -60 * sideMult, -60 * sideMult);
      } else {
        driveRaw(-60 * sideMult, -60 * sideMult, 60 * sideMult, 60 * sideMult);
      }
      delay(50);
      driveRaw(0, 0, 0, 0);
    }

    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraightAsync((-1225 * 2) + tickDiff, false,
                            true); // Approach the zones
    runLiftSync(50, true);         // Lower the lift
    motorSet(MOTOR_CLAW, 127);     // Extake the cone
    delay(100);
    runLiftAsync(200, true); // Raise the lift
    //
    // */
    //
    //
    // /*
    //
    waitForDriveStraight();
    // Kick off the turn
    driveRaw(127 * sideMult, 127 * sideMult, -127 * sideMult, -127 * sideMult);
    delay(250);
    pLoopTurnPoint(-180 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    //
    // */
    //
    //
    // /*
    //
    driveRaw(127, 127, 127, 127); // Drive crazily towards the 20pt zone
    delay(700);
    // driveRaw(60, 60, 100, 100); // Swerve into line with 10pt zone
    // delay(400);
    driveRaw(0, 0, 0, 0);
    runMogoSync(127, 1200); // Extake the mogo
    delay(100);
    driveRaw(-80, -80, -80, -80); // Cack out after scoring
    delay(300);
    driveRaw(0, 0, 0, 0); // Stop the robot
    // delay(200);
    /// driveRaw(-127, -127, -127, -127); // Back out of the zone
    // delay(800);
    // driveRaw(0, 0, 0, 0);
    //
    // */
  } break;
  // 5 pt single cone
  case 12: {
    ensureLiftTaskSuspended();
    ensureIntakeTaskSuspended();
    motorSet(MOTOR_CLAW, -127); // Intake Preload
    runLiftAsync(300, true);    // Raise the lift
    runMogoAsync(127, 1075);    // Put out the mogo intake
    delay(200);
    pLoopDriveStraightAsync(1250 * 2, false, true); // Drive to the mogo
    delay(100);
    motorSet(MOTOR_CLAW, -25); // Set goliath to stall speed
    waitForDriveStraight();
    int tickDiff = encoderGet(getEncoderBR());
    driveRaw(60, 60, 60, 60);
    delay(50);
    runMogoAsync(-127, 1075); // Intake Mogo
    delay(150);
    driveRaw(0, 0, 0, 0);
    tickDiff -= encoderGet(getEncoderBR());
    waitForMogoAsync();
    if (abs(gyroGet(getGyro())) <= 5 && abs(gyroGet(getGyro())) >= 3) {
      if ((gyroGet(getGyro()) > 0)) {
        driveRaw(60 * sideMult, 60 * sideMult, -60 * sideMult, -60 * sideMult);
      } else {
        driveRaw(-60 * sideMult, -60 * sideMult, 60 * sideMult, 60 * sideMult);
      }
      delay(50);
      driveRaw(0, 0, 0, 0);
    }

    //
    // */
    //
    //
    // /*
    //
    pLoopDriveStraightAsync((-1225 * 2) + tickDiff, false,
                            true); // Approach the zones
    runLiftSync(50, true);         // Lower the lift
    motorSet(MOTOR_CLAW, 127);     // Extake the cone
    delay(100);
    runLiftAsync(200, true); // Raise the lift
    //
    // */
    //
    //
    // /*
    //
    waitForDriveStraight();
    // Kick off the turn
    driveRaw(127 * sideMult, 127 * sideMult, -127 * sideMult, -127 * sideMult);
    delay(250);
    pLoopTurnPoint(-180 * (getAutonPosition() ? 1 : -1)); // Turn to stat. goal
    //
    // */
    //
    //
    // /*
    //
    runMogoSync(127, 1200); // Extake the mogo
    delay(100);
    driveRaw(-80, -80, -80, -80); // Cack out after scoring
    delay(300);
    driveRaw(0, 0, 0, 0); // Stop the robot
    // delay(200);
    /// driveRaw(-127, -127, -127, -127); // Back out of the zone
    // delay(800);
    // driveRaw(0, 0, 0, 0);
    //
    // */
  } break;
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
