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

#ifdef __cplusplus // end method if
}
#endif

#endif
