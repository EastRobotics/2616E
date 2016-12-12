#include "main.h"

void autonomous() {
  switch (getAutonMode()) {
    case 1:
      print("Ran auton one!");
      return;
    case 2:
      print("Ran auton two!");
      return;
    default:
      print("Ran auton that wasn't given a case!");
  }
}
