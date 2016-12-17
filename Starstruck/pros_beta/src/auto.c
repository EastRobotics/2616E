#include "main.h"

// KEEP IN MIND, AFTER AUTON, IF WE'RE PLUGGED IN SOME SENSORS WILL SHUT DOWN
// DON'T BE DUMB AND FORGET THIS UNTIL A COMPETITION. THAT WOULD BE BAD.
// From: Cameron, To: Cameron.
void autonomous() {
  switch (getAutonMode()) {
    case 1:
      print("Ran auton one!");
      break;
    case 2:
      print("Ran auton two!");
      break;
    default:
      print("Ran auton that wasn't given a case!");
  }

  // Should be run after all autons
  if (isOnline()) {
    imeShutdown(); // Disable our IMEs
    gyroShutdown(getGyro()); // Disable our gyro
  }
}
