#include "main.h"

#define ENC_L_RATIO 1      // Gear ratio of drive to left encoder
#define ENC_R_RATIO 1      // Gear ratio of drive to right encoder
#define ENCODER_READINGS 5 // Amount of encoder resdings to average for RPM
#define READING_DELAY 50   // Delay between each encoder reading
#define BREAK_TIME 10000   // Time alotted to flip the robot over
#define SPEED_UP_TIME 100  // Milliseconds alotted for drive to speed up

// CONFIGURE PORTS

// Measure the rpm values
double measureRPM(bool right) {
  Encoder enc;
  // Get readings from the proper encoder
  if (right)
    enc = getEncoderBR();
  else
    enc = getEncoderBL();
  double measurements[ENCODER_READINGS]; // encoder readings in an array
  int initialVal = encoderGet(enc);   // the initial value of the drive encoder
  fprintf(uart1, "INIT: %d\r\n", initialVal);
  // Read the encoder ENCODER_READINGS times with a delay of READING_DELAY
  // between readings
  for (int i = 0; i < ENCODER_READINGS; i++) {
    delay(READING_DELAY);
    measurements[i] = encoderGet(enc) - initialVal;
    //fprintf(uart1, "SUPER-RAWENC-%d: %d\r\n", i, encoderGet(getEncoderBL()));
    //fprintf(uart1, "RAWENC-%d: %d\r\n", i, encoderGet(enc));
    //fprintf(uart1, "ENC-%d: %f\r\n", i, measurements[i]);
  }
  double calcs[ENCODER_READINGS]; // an array for the calculated values to be
                               // stored in
  calcs[0] = measurements[0];
  for (int i = 1; i < ENCODER_READINGS; i++) {
    calcs[i] =
        measurements[i] -
        measurements[i - 1];   // Calculate the difference between each reading
    //fprintf(uart1, "1-CALC-%d: %f\r\n", i, calcs[i]);
    calcs[i] /= READING_DELAY; // Divide Δticks by Δtime(ms) to get ticks/ms
    //fprintf(uart1, "2-CALC-%d: %f\r\n", i, calcs[i]);
    calcs[i] *= 166.66667;     // Conversion between ticks/ms and rpm
    //fprintf(uart1, "3-CALC-%d: %f\r\n", i, calcs[i]);
    calcs[i] *= (right)
                    ? ENC_R_RATIO
                    : ENC_L_RATIO; // Multiply by gear ratio to get output ratio
    //fprintf(uart1, "4-CALC-%d: %f\r\n", i, calcs[i]);
  }

  // average the rpm readings
  double total = 0;
  for (int i = 0; i < ENCODER_READINGS; i++)
    total += calcs[i];

  return total / ENCODER_READINGS;
}

// Run the rpm test for the drive to match the drive speeds to an rpm
void rpmTest() {
  bprint(1, "Running RPM Test\r\n");
  bprint(1, "Running Right Side\r\n");
  bprint(1, "------------------\r\n");
  // Measure the rpm of the drive at different speeds
  for (int i = 5; i < 127; i += 5) {
    driveRaw(0, 0, i, i);
    delay(SPEED_UP_TIME);
    fprintf(uart1, "(%d, %f)\r\n", i, measureRPM(true));
  }
  bprint(1, "------------------\r\n");
  fprintf(uart1, "Giving %d seconds to flip the robot\r\n", BREAK_TIME / 1000);
  delay(BREAK_TIME); // Give time to flip the robot
  bprint(1, "Running Left Side\r\n");
  bprint(1, "------------------\r\n");
  // Measure the rpm of the drive at different speeds
  for (int i = 5; i < 127; i += 5) {
    driveRaw(i, i, 0, 0);
    delay(SPEED_UP_TIME);
    fprintf(uart1, "(%d,%f)\r\n", i, measureRPM(false));
  }
  bprint(1, "------------------\r\n");
  bprint(1, "DONE TEST\r\n");
}

// Run the rpm test with a manual configuration
void rpmTestManual(int motorPort, int speed, int speedUpTime) {
  bprint(1, "Running RPM Test\r\n");
  motorSet(motorPort, speed);
  delay(speedUpTime);
  fprintf(uart1, "RPM: %f\r\n", measureRPM(false));
  motorSet(motorPort, 0);
}
