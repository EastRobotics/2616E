#ifndef LIB2616E_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define LIB2616E_H_

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif // Start method list

//------------------------------------------------------------------------------
/*
** Global constants
*/
#define DIR_RIGHT true
#define DIR_LEFT false
#define DIR_UP true
#define DIR_DOWN false
#define DIR_FORWARD true
#define DIR_BACKWARD false
#define COLOR_RED true
#define COLOR_BLUE false

//------------------------------------------------------------------------------

/*
** Methods from drive.c
*/

  /*
  ** Sets the drive motors we're using. This makes it so we don't need to provide
  ** the ports every time we want to do any drive methods.
  **
  ** PARAMETERS:
  **   unsigned char: Front left  drive motor port
  **   unsigned char: Back  left  drive motor port
  **   unsigned char: Front right drive motor port
  **   unsigned char: Back  right drive motor port
  */
  void driveInit(unsigned char _driveFL, unsigned char _driveBL, unsigned char _driveFR, unsigned char _driveBR);

  /*
  ** Sets whether each drive motor should be reverse or not
  **
  ** PARAMETERS:
  **   bool: Front left  drive motor reverse
  **   bool: Back  left  drive motor reverse
  **   bool: Front right drive motor reverse
  **   bool: Back  right drive motor reverse
  */
  void driveSetReverse(bool _driveFLReverse, bool _driveBLReverse, bool _driveFRReverse, bool _driveBRReverse);

  /*
  ** Sets the given motor's speed, assuming the motor is valid. Otherwise, an
  ** error is output to the console.
  **
  ** PARAMETERS:
  **   unsigned char: Motor port to be set
  **   int: The speed of the motor, -127 to 127
  **   const char *: String to represent motor in console output if invalid
  **
  ** OPTIONAL PARAMETERS:
  **   const char *: Text representation of motor, used for error message
  **
  ** NOTES:
  **   Calls to printf can be taxing, so try to not have any erroneous calls
  */
  void driveIfValid(unsigned char motor, int speed, const char *string);

  /*
  ** Directly sets the motor speeds, checking to make sure the motors have been
  ** set. Base method for most other drive methods.
  **
  ** PARAMETERS:
  **   int: The speed of the front left  motor, -127 to 127
  **   int: The speed of the back  left  motor, -127 to 127
  **   int: The speed of the front right motor, -127 to 127
  **   int: The speed of the back  right motor, -127 to 127
  */
  void driveRaw(int speedFL, int speedBL, int speedFR, int speedBR);

  /*
  ** Sets the drive motors based on forward and turn speed. This is for tank
  ** style drives. Use driveHolonomic() for x-drive and mecanum style drives.
  **
  ** PARAMETERS:
  **   int: The speed/rate to move, -127 (backward) to 127 (forward)
  **   int: The speed/rate to turn, -127 (left)     to 127 (right)
  */
  void drive(int speedForward, int speedTurn);

  /*
  ** Sets left and right side drive speeds. Very basic drive style, and it's
  ** suggested to use drive() for normal driving. Made for tank style drives.

  ** PARAMETERS:
  **   int: The speed for left  drive, -127 (backward) to 127 (forward)
  **   int: The speed for right drive, -127 (backward) to 127 (forward)
  */
  void driveTank(int speedLeft, int speedRight);

  /*
  ** Sets the drive motors based on forward, turn, and strafe speed. This is for
  ** holonomic drives like x-drives and mecanum. Use drive() for tank drives.
  **
  ** PARAMETERS:
  **   int: The speed/rate to move,   -127 (backward) to 127 (forward)
  **   int: The speed/rate to turn,   -127 (left)     to 127 (right)
  **   int: The speed/rate to strafe, -127 (left)     to 127 (right)
  */
  void driveWithLogic(int speedForward, int speedTurn, int speedStrafe);

  // Drive with checks to rule out errors with joystick controls
  // NOTES:
  // 	Multipliers should always be (0 <= x <= 1.0). This avoids going too fast
  // or slow and going out of proportion
  // PARAMETERS:
  //	int: -127 to 127, speed to drive forward or backward respectively
  //	int: -127 to 127, speed to turn left or right respectively
  //	int: -127 to 127, speed to strafe left or right respectively
  //	float: What to reduce forward/backward speed to (0.7 -> 70% of input)
  //	float: What to reduce left/right turn speed to (0.7 -> 70% of input)
  //	float: What to reduce left/right strafe speed to (0.7 -> 70% of input)
  void driveWithLogic(int speedForward, int speedTurn, int speedStrafe);

  // Enables slew rate, limiting speed change to _slewRate every update cycle
  // PARAMETERS:
  //	int: Amount of speed change allowed every 20ms
  void enableSlew(int _slewRate);

  // Disables slew rate code
  void disableSlew();

  // Returns a speed based on the last speed and set slew rate
  // PARAMETERS:
  //	int: The current speed of the drive
  //  int: The target speed of the drive
  // RETURNS:
  //  int: The slewed speed to set the motors to
  int slew(int currentSpeed, int targetSpeed);

  /*
  ** Calls driveRaw with slewed speeds, using slew rate set by enableSlew
  **
  ** PARAMETERS:
  **   int: The target speed of the front left  motor, -127 to 127
  **   int: The target speed of the back  left  motor, -127 to 127
  **   int: The target speed of the front right motor, -127 to 127
  **   int: The target speed of the back  right motor, -127 to 127
  */
  void driveRawSlew(int speedFL, int speedBL, int speedFR, int speedBR);

//------------------------------------------------------------------------------

/*
** Methods from autonswitch.c
*/

  void autonInit(unsigned char);

  const char * getAutonName();

  bool setAutonMode(unsigned char);

  unsigned char getAutonMode();

  unsigned char getAutonModeMin();

  unsigned char getAutonModeMax();

  void setAutonPosition(bool);

  bool getAutonPosition();

  void setAutonColor(bool);

  bool getAutonColor();

//------------------------------------------------------------------------------

  /*
  ** Methods from lcd.c
  */

  typedef void (*updateLCDFunction)(bool, int);

  void lcdSetCycles(bool _cycles);

  void setRefreshTime(unsigned long);

  void lcdPrintTitle(const char *);

  void lcdPrintCentered(const char *, unsigned char);

  void lcdHome();

  void lcdSetPage(int);

  void lcdNextPage();

  void lcdLastPage();

  void lcdResetAutoRefresh();

  void lcdAutoRefresh(void *);

  void lcdManager(void *);

  // Messy, really translates to:
  // TaskHandle startLCDMenu(void (*manager)(void *))
  void lcdStartMenu();

  void lcdInitMenu(int, int, int);

  void lcdSetMenuNext(void (*)(int));

  void lcdSetMenuBack(void (*)(int));

  void lcdSetUpdater(updateLCDFunction);

//------------------------------------------------------------------------------

  /*
  ** Methods from sensors.c
  */

  unsigned int powerLevelExpander(unsigned char channel, bool newVersion);

  float voltLevelExpander(unsigned char channel, bool newVersion);

  float voltLevelMain();

  float voltLevelBackup();

  unsigned char batteryStatus(float);

  int RPMToMotor(float);

  float motorToRPM(int);

  float getSlope(float x1, float y1, float x2, float y2);

  float getYInt(float slope, float x, float y);

  float lerp(float, float, float, float, float);

  int getLerpedSpeed(int power, int minSpeed, int minPower);

  bool sameSign(int one, int two);

//------------------------------------------------------------------------------

  /*
  ** Methods from pid.c
  */

  typedef double (*getCurrentValFunction)();

  typedef void (*setCurrentValFunction)(double);

  typedef void (*PIDIteratorCallbackFunction)(void);

  typedef void (*initFunction)(void);

  bool runPID(int);

  void pidLoop(void *);

  void startPIDLoop(unsigned int, double);

  void stopPIDLoop(unsigned int, bool);

  int setPIDLoop(unsigned int index, initFunction init,
                 getCurrentValFunction valueGetter,
                 setCurrentValFunction valueSetter,
                 PIDIteratorCallbackFunction PIDIteratorCallback, double kP,
                 double kI, double kD, double integralLimit, double threshold);

  int addPIDLoop(initFunction init, getCurrentValFunction valueGetter,
                 setCurrentValFunction valueSetter,
                 PIDIteratorCallbackFunction PIDIteratorCallback, double kP,
                 double kI, double kD, double integralLimit, double threshold);

  void initPID();

  void shutdownPID();

  void resetPIDLoop(int index);

  void waitForPID(int index);

  double distance(double x1, double x2, double y1, double y2);

  //------------------------------------------------------------------------------

    /*
    ** Methods from odometry.c
    */

  void initOdomScale(float wheelDiam, float driveCircum, float wheelEncRatio);

  void trackRobotPosition(void *param);

  int getOdomPosX();

  int getOdomPosY();

  int getOdomTheta();

  void odomReset();

  void setOdomPosX(int x);

  void setOdomPosY(int y);

//------------------------------------------------------------------------------

  /*
  ** Bluetooth.c
  */

  #define bprintf(uart, format, args...) fprintf (uart == 1 ? uart1 : uart2, format, args)

  void hc05Init(char uart, bool atMode);

  char * bluetoothRead(char uart);

  void bprint(char uart, const char * message);

  void blisten(char uart, void( * callback)(char*));

//------------------------------------------------------------------------------

  /*
  ** Reshape.c
  */

  void rpmTest();

  void rpmTestManual(int motorPort, int speed, int speedUpTime);

#ifdef __cplusplus // end method if
}
#endif

#endif
