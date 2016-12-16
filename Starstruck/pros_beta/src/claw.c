#include "main.h"

void manageClaw(void * ignored){
  bool lastDirection = false; //false is close, true is open
  bool activePressing = false; //are we currently pressing a button
  unsigned int target = 0; //best fit position of claw
  while(true){
    if(true) { //if button is pressed
      if(true) { //if open button pressed
        target = 0; //find value of more open claw
        lastDirection = true;
      } else { //if closed button pressed
        target = 0; //find value of more closed claw
        lastDirection = false;
      }
      activePressing = true;
    } else { //if nothing is pressed
      activePressing = false;
    }
    //hold claw at certain position
      //make sure claw is forward (not askew)
      //watch for error, and correct it if it occurs
    delay(20);
  }
}
