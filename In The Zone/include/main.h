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

// TODO Configure speeds
/*
** Constants to represent certain positions
*/
#define POSITION_GOAL_NONE -1
#define POSITION_INTAKE_GROUND 1
#define POSITION_INTAKE_LOADER 2
#define POSITION_GOAL_STATIC 3
#define POSITION_GOAL_BASE_INTERNAL 4
#define POSITION_GOAL_BASE_EXTERNAL 5
#define POSITION_WAITING 6

//------------------------------------------------------------------------------

/*
** Prototypes for initialization, operator control and autonomous
*/

void autonomous();

void initializeIO();

void initialize();

void operatorControl();

void implUpdateLCD(bool userCaused, int page);
void implMenuNext(int page);
void implMenuBack(int page);

//------------------------------------------------------------------------------

/*
** Sensor accessor methods
*/

// Holds gyro from the init.c file
Gyro getGyro();
Encoder getEncoderBR();
Encoder getEncoderBL();
void killDriveEncoders();
void initDriveEncoders();

//------------------------------------------------------------------------------

/*
** Lift control methods (lift.c)
*/

// Directly sets lift motor speeds
// Shouldn't be used unless making control loops
void setLiftSpeedRaw(int speedLeft, int speedRight);

// Sets lift speed using bias correction
// Positive speed upward, negative downward
void setLiftSpeed(int speed);

// Sets the value for the lift to try and reach
void setLiftTarget(int target);

// Transforms a goal constant into a height constant
int getGoalHeight(int goal);

// Sets the lift target to the right height for the goal type and cone count
void setLiftTargetSmart(int goal, int cones);

// Task to handle the control of the lift
void liftControl(void *ignored);

//------------------------------------------------------------------------------

/*
** Intake control methods (intake.c)
*/

// Directly sets intake motor speeds
// Shouldn't be used unless making control loops
void setIntakeSpeedRaw(int speedLeft, int speedRight);

// Sets intake speed using bias correction
// Positive speed upward, negative downward
void setIntakeSpeed(int speed);

// Sets the value for the intake to try and reach
void setIntakeTarget(int target);

// Transforms a goal constant into a pos constant
int getGoalPos(int goal);

// Sets the intake target to the right pos for the goal type
void setIntakeTargetSmart(int goal);

// Task to handle the control of the intake
void intakeControl(void *ignored);

// End C++ export structure
#ifdef __cplusplus
}
#endif

#endif
