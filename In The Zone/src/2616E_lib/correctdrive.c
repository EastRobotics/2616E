// #include "main.h"
// #include "math.h"
//
// char dat1[] = {
//         0, // 0
//         0, // 5
//         0, // 10
//         0, // 15
//         9, // 20
//         19, // 25
//         28, // 30
//         38, // 35
//         47, // 40
//         57, // 45
//         62, // 50
//         66, // 55
//         66, // 60
//         71, // 65
//         76, // 70
//         76, // 75
//         81, // 80
//         81, // 85
//         81, // 90
//         81, // 95
//         86, // 100
//         86, // 105
//         86, // 110
//         90, // 115
//         90, // 120
//         90 // 125
// };
// char dat2[] = { // mot1 with 3-(1-6) added
//         0, // 0
//         0, // 5
//         0, // 10
//         0, // 15
//         9, // 20
//         20, // 25
//         30, // 30
//         38, // 35
//         50, // 40
//         58, // 45
//         65, // 50
//         67, // 55
//         69, // 60
//         73, // 65
//         74, // 70
//         75, // 75
//         79, // 80
//         82, // 85
//         83, // 90
//         83, // 95
//         84, // 100
//         89, // 105
//         89, // 110
//         90, // 115
//         92, // 120
//         92 // 125
// };
//
//
//
//
// int getDistance(char one, char two) {
//   return abs(one - two);
// }
//
// char getClosestMotor(char* motToRpm, char rpm) {
//   for (char i = 1; i < sizeof(motToRpm) - 1; i++) {
//       // If this motor index is this RPM, return it
//       if (rpm == i)
//           return i;
//       // Skip leading 0s
//       if (motToRpm[i - 1] == (char) 0 || motToRpm[i] == (char) 0)
//           continue;
//       // We reached end of rpms, where we are has to be closest then
//       if (motToRpm[i + 1] == (char) 0)
//           return i;
//       // If this rpm is between motor values, return whichever is closer
//       // TODO Guess speed between two closest
//       if (motToRpm[i - 1] < rpm && motToRpm[i] > rpm)
//           return (getDistance(motToRpm[i - 1], rpm) > getDistance(motToRpm[i], rpm)) ? i : (char) (i - 1);
//   }
//   return (char) (sizeof(motToRpm) - 1); // Didn't get anywhere, highest must be closest
// }
//
// void printCurves(char[] curve1, char[] curve2) {
//   DecimalFormat format = new DecimalFormat("000");
//   DecimalFormat lformat = new DecimalFormat("00");
//   print("---------------------");
//   print("| SPD | DAT1 | DAT2 |");
//   print("---------------------");
//   for (int i = 0; i < sizeof(curve1); i++) {
//       print("| " + lformat.format(i + 1) + "  | " + format.format((int) curve1[i]) + "  | "
//               + format.format((int) curve2[i]) + "  |");
//       print("---------------------");
//   }
// }
//
// void printOutput(char* curve1, char* data1, char* curve2, char* data2) {
//   char* output1 = curve1; // TODO Clone
//   char* output2 = curve2; // TODO Clone
//   for (int i = 0; i < sizeof(output1); i++) {
//       output1[i] = data1[output1[i]];
//       output2[i] = data2[output2[i]];
//   }
//   printCurves(output1, output2);
// }
//
//
//
// void main() {
//     // Input data should be robot RPM data, where all non-moving speeds set to 0
//     // Input data should also always be low->high, no dips
//
//     print("Input Data:");
//     printCurves(dat1, dat2);
//     char trimdat1[] = dat1; // input:rpm
//     char trimdat2[] = dat2; // input:rpm
//
//     /*
//     ** Find and use the highest low (aka the lowest num both motors have)
//     */
//     char highestLow = 0;
//     int x = 1;
//     while (!(trimdat1[x - 1] == 0 && trimdat1[x] != 0) && x < sizeof(trimdat1)) x++;
//     highestLow = trimdat1[x];
//     x = 1;
//     while (!(trimdat2[x - 1] == 0 && trimdat2[x] != 0) && x < sizeof(trimdat2)) x++;
//     if (trimdat2[x] < highestLow)
//         highestLow = trimdat2[x];
//     // Set any lower than biggest small to 0
//     for (int i = 0; i < sizeof(trimdat1); i++) {
//         if ((int) trimdat1[i] < (int) highestLow) trimdat1[i] = 0;
//         if ((int) trimdat2[i] < (int) highestLow) trimdat2[i] = 0;
//     }
//
//     /*
//     ** Find and use the lowest high (aka the highest num both motors have)
//     */
//     int lowestHigh = trimdat1[sizeof(trimdat1) - 1];
//     if (trimdat2[sizeof(trimdat2) - 1] < lowestHigh)
//         lowestHigh = trimdat2[x];
//     // Set any higher than lowest high to 0
//     for (int i = 0; i < sizeof(trimdat1); i++) {
//         if (trimdat1[i] > lowestHigh) trimdat1[i] = 0;
//         if (trimdat2[i] > lowestHigh) trimdat2[i] = 0;
//     }
//
//     // Print out data after high/low
//     print("\nHigh/low trim:");
//     printCurves(trimdat1, trimdat2);
//
//     print("Highest low: " + highestLow);
//     print("Lowest high: " + lowestHigh);
//
//     /*
//     ** Find slope of highestLow to lowestHigh, fill motor RPMs with this data
//     */
//     char motor1[127]; // input mot : output mot
//     char motor2[127]; // input mot : output mot
//     double increment = (lowestHigh - highestLow) / 127.0;
//     for (int i = 0; i < 127; i++)
//         motor1[i] = getClosestMotor(trimdat1, (char) Math.floor(highestLow + ((double) i * increment) + 0.5));
//     for (int i = 0; i < 127; i++)
//         motor2[i] = getClosestMotor(trimdat2, (char) Math.floor(highestLow + ((double) i * increment) + 0.5));
//
//     // Print out data after high/low
//     print("\nFinalized input output:");
//     printCurves(motor1, motor2);
//
//     // Print out data after high/low
//     print("\nFinalized input : rpm:");
//     printOutput(motor1, trimdat1, motor2, trimdat2);
// }
