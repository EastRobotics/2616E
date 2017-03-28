#include "main.h"
#define MIN_AUTON_MODE 1

unsigned char maxAutonMode;
unsigned char autonMode = MIN_AUTON_MODE;
bool position = false; // False for left, true for right
bool color = false;    // False for red, true for blue

void autonInit(unsigned char _maxAutonMode) { maxAutonMode = _maxAutonMode; }

/*
** Sets the current autonomous mode to the supplied value
**
** PARAMETERS:
**   int: The mode to set autonomous to
**
** RETURNS:
**   bool: Whether or not the set number was a valid number
*/
bool setAutonMode(unsigned char _autonMode) {
  if ((_autonMode >= MIN_AUTON_MODE) &&
      (_autonMode <= maxAutonMode)) { // If valid input
    autonMode = _autonMode;
  }
  return false;
}

/*
** Returns the current autonomous mode
**
** RETURNS:
**   int: The current autonomous mode
*/
unsigned char getAutonMode() { return autonMode; }

/*
** Returns the minimum autonomous mode
**
** RETURNS:
**   int: The minimum autonomous mode (Always 1)
*/
unsigned char getAutonModeMin() { return MIN_AUTON_MODE; }

/*
** Returns the maximum autonomous mode
**
** RETURNS:
**   int: The current autonomous mode
*/
unsigned char getAutonModeMax() { return maxAutonMode; }

/*
** Sets the current autonomous position. False for left, true for right.
**
** PARAMETERS:
**   bool: The autonomous position. False for left, true for right.
*/
void setAutonPosition(bool setPosition) { position = setPosition; }

/*
** Gets the current autonomous position. False for left, true for right.
**
** RETURNS:
**   bool: The autonomous position. False for left, true for right.
*/
bool getAutonPosition() { return position; }

/*
** Sets the current autonomous color. False for blue, true for red.
**
** PARAMETERS:
**   bool: The autonomous color. False for blue, true for red.
*/
void setAutonColor(bool setColor) { color = setColor; }

/*
** Gets the current autonomous color. False for blue, true for red.
**
** RETURNS:
**   bool: The autonomous color. False for blue, true for red.
*/
bool getAutonColor() { return color; }
