#ifndef LIB2616E_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define LIB2616E_H_

// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

  /*
  ** Drive
  */
  extern unsigned char motorDriveFL;
  extern unsigned char motorDriveBL;
  extern unsigned char motorDriveFR;
  extern unsigned char motorDriveBR;

// End C++ export structure
#ifdef __cplusplus
}
#endif

#endif
