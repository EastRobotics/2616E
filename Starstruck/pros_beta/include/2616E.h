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

  void driveIfValid(unsigned char, int, const char*);

  void driveRaw(int, int, int, int);

  void drive(int, int);

  void driveTank(int, int);

  void driveHolonomic(int, int, int);

/*
** Methods from autonswitch.c
*/

  void autonInit(const char**);

  const char * getAutonName();

  bool setAutonMode(int);

  int getAutonMode();

  int getAutonModeMin();

  int getAutonModeMax();

  void setAutonPosition(bool);

  bool getAutonPosition();

  void setAutonColor(bool);

  bool getAutonColor();

  /*
  ** Methods from lcd.c
  */

  typedef void (*updateLCDFunction)(bool, int);

  typedef void (*menuPressMethod)(int);

  void setCycles(bool _cycles);

  void setRefreshTime(unsigned long);

  void lcdPrintTitle(const char *);

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

  void lcdSetMenuNext(menuPressMethod);

  void lcdSetMenuBack(menuPressMethod);

  void lcdSetUpdater(updateLCDFunction);

#ifdef __cplusplus // end method if
}
#endif

#endif
