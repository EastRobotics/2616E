#define VERSION "0.0.1"

/** @file main.h
 * @brief Header file for global functions
 *
 * Any experienced C or C++ programmer knows the importance of header files. For those who
 * do not, a header file allows multiple files to reference functions in other files without
 * necessarily having to see the code (and therefore causing a multiple definition). To make
 * a function in "opcontrol.c", "auto.c", "main.c", or any other C file visible to the core
 * implementation files, prototype it here.
 *
 * This file is included by default in the predefined stubs in each VEX Cortex PROS Project.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#ifndef MAIN_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define MAIN_H_

#include <API.h>

#include <ports.h> // Include our port defines
#include <2616E.h> // Include the 2616E library

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#define LIFT_THRESH_AVOID 20

//------------------------------------------------------------------------------

#define INTAKE_POS_AVOID  2040
#define INTAKE_POS_SCORE  3747
#define INTAKE_POS_SIT    2040

//------------------------------------------------------------------------------

#define INTAKE_TYPE_PASSTHROUGH false
#define ROBOT_CONE_LIMIT 14

/*
** Prototypes for initialization, operator control and autonomous
*/

void deploy();

void autonomous();

void initializeIO();

void initialize();

void operatorControl();

void setRunAuton(bool shouldRun);

void implUpdateLCD(bool userCaused, int page);
void implMenuNext(int page);
void implMenuBack(int page);

//------------------------------------------------------------------------------

/*
** Sensor accessor methods
*/

Gyro getGyro(); // Holds gyro from the init.c file
Encoder getEncoderBR();
Encoder getEncoderBL();
Encoder getEncoderChain();
Encoder getEncoderLift();
void killDriveEncoders();
void initDriveEncoders();

TaskHandle getLiftCont();
TaskHandle getIntakeCont();

//------------------------------------------------------------------------------

/*
** Lift control methods (lift.c)
*/

// Directly sets lift motor speeds
// Shouldn't be used unless making control loops
//void setLiftSpeedRaw(int speed);

// Sets lift speed using bias correction
// Positive speed upward, negative downward
void setLiftSpeed(int speed);

// I know it shouldn't be used, but at a comp now and need this change - Michael
void setLiftSpeedRaw(int speedLeft, int speedRight);

// Sets the value for the lift to try and reach
void setLiftTarget(int target);

int getLiftPos();

int getLiftPosLeft();

int getLiftPosRight();

// Sets the starting position for the lift potentiometers
void setLiftStart(int posLeft, int posRight);

// Sets the lift starting positions to be the current values
void setLiftStartAsNow();

// Transforms a goal constant into a height constant
int getGoalHeight(int goal);

// Sets the lift target to the right height for the goal type and cone count
void setLiftTargetSmart(int cones);

// Gets the lift target
int getLiftTarget();

// Determines speed at which the lift should run to reach target value
int pLoopDetermineLiftSpeed();

// Task to handle the control of the lift
void liftControl(void *ignored);

// Whether or not the lift is at it's target
bool isLiftReady();

// Wait until the lift is at it's desired target
void waitForLift();

int getLiftTarget();

void setShouldOvershootLift(bool shouldOvershootLift);


//------------------------------------------------------------------------------

/*
** Intake control methods (intake.c)
*/

// Returns the encoder position of the chain bar

int getIntakePos();

// Directly sets intake motor speeds
// Shouldn't be used unless making control loops
void setIntakeSpeedRaw(int speed);

// Sets intake speed using bias correction
// Positive speed upward, negative downward
void setIntakeSpeed(int speed);

// Sets the value for the intake to try and reach
void setIntakeTarget(int target);

// Transforms a goal constant into a pos constant
int getGoalPos(int goal);

// Sets the intake target to the right pos for the goal type
void setIntakeTargetSmart(int goal);

// Gets the intake target
int getIntakeTarget();

// Task to handle the control of the intake
void intakeControl(void *ignored);

// Whether or not the intake is at it's target
bool isIntakeReady();

// Wait until the intake is at it's desired target
void waitForIntake();

// Returns whether or not the intake is past a certain pos
bool intakeIsAbove(int value);

// Returns whether or not the intake is past a certain pos accounting for thresh
bool intakeIsAboveAccThresh(int value);

// Whether or not the intake should hold
void setShouldHoldIntake(bool shouldHold);

// Specifies the speed at which the intake should run when approaching target with p loop
int pLoopDetermineIntakeSpeed();

// Sets claw to open or closed
void setClawOpen(bool isOpen);

// Sets up the claw to open
void openClaw();

// Sets up the claw to close
void closeClaw();

// Checks if the claw is done it's movement
bool isClawReady();

// Waits for the Claw to be Ready
void waitForClaw();

//------------------------------------------------------------------------------

/*
** Manipulator control methods (manipulator.c)
*/

int getManipulatorIntakePos();

int getGoalType();

int getAction();

// Get how many cones are on the current target goal
int getConeCount();

void setConeCount(int coneCount);

// Whether or not the manipulator is still doing something
bool isManipulatorReady();

// Wait until the intake is at it's desired target
void waitForManipulator();

// Task to handle the control of the manipulator system
void manipulatorControl(void *ignored);

// -----------------------------------------------------------------------------

/*
** Bluetooth listener implementations (bluetooth_impl.c)
*/

void blueListen(char *message);

// ---------------------------------------------------------------------------

/*
** Auton Drive Implementations (autondrive_impl.c)
*/

void pLoopTurnPoint(int angleTarget);

void pLoopDriveStraight(int tickDiff, bool correctBackwards, bool correctDir);

void autoDriveToPoint(int x, int y, bool driveCorrectBackwards, bool driveCorrectDir);

void odomDriveForward(int millimeters, bool driveCorrectBackwards, bool driveCorrectDir);

void odomTurn(int degrees);

void odomTurnSB(int degrees);

void driveStraightRaw(int speed, int timeToDrive);

// ---------------------------------------------------------------------------

/*
** Auton Async Implementations (autonasync.c)
*/

void runMogoAsync(int speed, int runTime);

void waitForMogoAsync();

void pLoopDriveStraightAsync(int ticks, bool correctBackwards, bool correctDir);

void waitForDriveStraight();

void pLoopTurnPointAsync(int angle);

void waitForTurnPoint();

void runLiftAsync(int target, bool shouldOvershoot);

void waitForLiftAsync();

void runIntakeAsync(int target);

void waitForIntakeAsync();

// End C++ export structure
#ifdef __cplusplus
}
#endif

#endif
