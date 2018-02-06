#include "main.h"
#include "string.h"

// Listen to bluetooth commands from an external controller and respond
void blueListen(char *message) {
  if (strcmp(message, "pos\r\n") == 0) { // Send position
    bprintf(1, "Robot pos: (%d,%d) >\r\n", getOdomPosX(), getOdomPosY());
  } else if (strcmp(message, "gyro\r\n") == 0) { // Send gyro
    bprintf(1, "Robot gyro: %d\r\n >", gyroGet(getGyro()));
  } else if (strcmp(message, "rpmdrive\r\n") == 0) {
    rpmTest();
  } else if (strcmp(message, "rpmmanual\r\n") == 0) {
    rpmTestManual(6, 127, 1000);
  } else if (strcmp(message, "sensortest\r\n") == 0) {
    bprintf(1, "Left  Drive Enc: %d\r\n", encoderGet(getEncoderBL()));
    bprintf(1, "Right Drive Enc: %d\r\n", encoderGet(getEncoderBR()));
    bprintf(1, "Claw  Lim: %d\r\n", digitalRead(DIGITAL_LIM_CLAW));
    bprintf(1, "Cart  Pot: %d\r\n", analogRead(ANALOG_POT_FOUR_BAR));
    bprintf(1, "Lift  pot L: %d\r\n", analogRead(ANALOG_POT_LIFT_L));
    bprintf(1, "Lift  pot R: %d\r\n", analogRead(ANALOG_POT_LIFT_R));
    bprintf(1, "Lift  pos: %d\r\n", getLiftPos());
    bprintf(1, "Lift  posL: %d\r\n", getLiftPosLeft());
    bprintf(1, "Lift  posR: %d\r\n", getLiftPosRight());
  } else if (strcmp(message, "startauton\r\n") == 0) {
    setRunAuton(true);
  } else if (strcmp(message, "ryan\r\n") == 0) { // Send give complaint
    bprint(1, "OMG it has too much give! >:(\r\n");
  } else if (strcmp(message, "checkauton\r\n") == 0) {
    const char *autonSide;
    if (getAutonPosition()) {
      autonSide = "Right";
    } else {
      autonSide = "Left";
    }
    bprintf(1, "Auton #%d\r\nAuton Side: ", getAutonMode());
    bprint(1, autonSide);
    bprint(1, "\r\n");

  } else // Unknown command
    bprintf(1, "I don't know what \"%s\" means :(", message);
}
