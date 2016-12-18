#ifndef LIB2616E_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define LIB2616E_H_

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif // Start method list

/*
** Methods from drive.c
*/

  void driveInit(unsigned char, unsigned char, unsigned char, unsigned char);

  void driveSetReverse(bool, bool, bool, bool);

  void driveIfValid(unsigned char, int, const char*);

  void driveRaw(int, int, int, int);

  void drive(int, int);

  void driveTank(int, int);

  void driveHolonomic(int, int, int);

  void driveHolonomicWithLogic(int, int, int);

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

  /*
  ** Methods from pid.c
  */

  typedef double( * getCurrentValFunction)();

  typedef void( * setCurrentValFunction)(double);

  typedef void( * PIDIteratorCallbackFunction)();

  bool runPID(int);

  void pidLoop(void *);

  void startPIDLoop(unsigned int, double);

  void stopPIDLoop(unsigned int, bool);

  int setPIDLoop(unsigned int, getCurrentValFunction, setCurrentValFunction, PIDIteratorCallbackFunction, double, double, double, double, double);

  int addPIDLoop(getCurrentValFunction, setCurrentValFunction, PIDIteratorCallbackFunction, double, double, double, double, double);

  void initPID();

#ifdef __cplusplus // end method if
}
#endif

#endif
