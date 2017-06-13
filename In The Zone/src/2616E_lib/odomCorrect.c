#include "main.h"
#include "math.h"

////////////////////////////////////////////////////////////////////////////////
//
//            Use the lines on the field in order to correct
//             the position that the odometry is predicting
//
////////////////////////////////////////////////////////////////////////////////

#define WHITE_THRESH 200 // Value of the light sensor to be a white line

typedef struct line line;
typedef struct lineSensor lineSensor;
typedef struct point point;

struct line {
  double x1;
  double y1;
  double x2;
  double y2;
};

struct lineSensor {
  double offX;
  double offY;
  int portNum;
};

struct point {
  double x;
  double y;
};

struct line lines[10];
struct lineSensor lineSensors[4];

void setupLine(int index, double x1, double y1, double x2, double y2) {
  lines[index].x1 = x1;
  lines[index].y1 = y1;
  lines[index].x2 = x2;
  lines[index].y2 = y2;
}
// setup the lines on the "in the zone" field
void setupLines() {
  setupLine(0, 0.0, 1524.0, 609.6, 1524.0);
  setupLine(1, 1524.0, 0.0, 1524.0, 609.6);
  setupLine(2, 1828.8, 0.0, 3657.6, 1828.8);
  setupLine(3, 0.0, 1828.8, 1828.8, 3657.6);
  setupLine(4, 2133.6, 3048, 2133.6, 3657.6);
  setupLine(5, 3048, 2133.6, 3657.6, 2133.6);
  setupLine(6, 1219.2, 1828.8, 1828.8, 1219.2);
  setupLine(7, 1828.8, 1219.2, 2438.4, 1828.8);
  setupLine(8, 2438.4, 1828.8, 1828.8, 2438.4);
  setupLine(9, 1828.8, 2438.4, 1219.2, 1828.8);
}

void setupSensor(int index, double offX, double offY, int portNum) {
  lineSensors[index].offX = offX;
  lineSensors[index].offY = offY;
  lineSensors[index].portNum = portNum;
}

// setup the line sensors
// TODO: Fill with actual values later
void setupSensors() {
  setupSensor(0, -10, -10, ANALOG_LINE_LEFT_FRONT);
  setupSensor(1, 10, -10, ANALOG_LINE_RIGHT_FRONT);
  setupSensor(2, -10, 10, ANALOG_LINE_LEFT_BACK);
  setupSensor(3, 10, 10, ANALOG_LINE_RIGHT_BACK);
}

// returns the sensor number that is on the line
int sensorOnLine(int sensor) {
  return analogRead(lineSensors[sensor].portNum) > WHITE_THRESH;
}

// distance formula
double dist(double x1, double y1, double x2, double y2) {
  return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

double getFieldSlope(line fieldLine) {
  if (fieldLine.x2 - fieldLine.x1 != 0)
    return ((fieldLine.y2 - fieldLine.y1) / (fieldLine.x2 - fieldLine.x1));
  return 0; // zero is returned if either the line is horizontal or vertical
  //            so it must be handled later
}

double getPerpSlope(line fieldLine) {
  return 1 / getFieldSlope(fieldLine) * -1;
}

// point-slope form: y - y1 = m(x - x1)
double evalEq(double slope, double x, double x1, double y1) {
  return (slope * (x - x1)) + y1;
}

// gets intersect between two points, returns {-1,-1} if there is none
// x = (m1x1-y1-m2x2+y2)/(m1-m2)
point getIntersect(line l1, line l2) {
  double slope1 = getFieldSlope(l1);
  double slope2 = getFieldSlope(l2);
  // if the lines are parallel they are either the same, or never intersect
  if (round(slope1 * 100) / 100 == round(slope2 * 100) / 100) {
    struct point toReturn;
    toReturn.x = -1;
    toReturn.y = 1;
    return toReturn;
  }
  double x = ((((slope1 * l1.x1) - l1.y1) - ((slope2 * l2.x1) + l2.y1)) /
              (slope1 - slope2));
  struct point toReturn;
  toReturn.x = x;
  toReturn.y = evalEq(slope1, x, l1.x1, l1.y1);
  return toReturn;
}

// get the point on the line of the perpendicular
// projection intersecting the line sensor
// returns {-1,-1} if none exist
point getPerpProjection(line fieldLine, double x, double y) {
  double slope = getPerpSlope(fieldLine);
  // handle vertical (perpendicular) lines
  if ((round(slope * 100) / 100 == 0.0) && (fieldLine.y2 == fieldLine.y1)) {
    struct point toReturn;
    toReturn.x = x;
    toReturn.y = evalEq(slope, x, fieldLine.x1, fieldLine.y1);
    return toReturn;
  } else {
    // non vertical lines
    struct line toPass;
    toPass.x1 = x;
    toPass.y1 = y;
    toPass.x2 = (x + 1);
    toPass.y2 = evalEq(slope, (x + 1), x, y);
    point intersect = getIntersect(fieldLine, toPass);
    // if the intersect is off the bounds of the field line
    if ((intersect.x > fieldLine.x1 && intersect.x > fieldLine.x2) ||
        (intersect.x < fieldLine.x1 && intersect.x < fieldLine.x2) ||
        (intersect.y > fieldLine.y1 && intersect.y > fieldLine.y2) ||
        (intersect.y < fieldLine.y1 && intersect.y < fieldLine.y2)) {
      struct point toReturn;
      toReturn.x = -1;
      toReturn.y = -1;
      return toReturn;
    }
    return intersect;
  }
}

// get the closest line
// returns -1 if the robot is further than 3000.0 mm away from all lines somehow
// in that case you have bigger problems to worry about... (ie. getting the
// robot back on the field)
int getClosestLine(int sensorNum) {
  double lowestDistance = 3000.0;
  int lowestIndex = -1;
  double xPos = getOdomPosX() + lineSensors[sensorNum].offX;
  double yPos = getOdomPosY() + lineSensors[sensorNum].offY;
  for (int i = 0; i < 10; i++) {
    double distVal = 3000.0;
    for (int j = 0; j < 3; j++) {
      switch (j) {
      case 0: {
        distVal = dist(xPos, yPos, lines[i].x1, lines[i].y1);
        break;
      }
      case 1: {
        distVal = dist(xPos, yPos, lines[i].x2, lines[i].y2);
        break;
      }
      case 2: {
        point p = getPerpProjection(lines[i], xPos, yPos);
        if (p.x != -1 && p.y != -1)
          distVal = dist(xPos, yPos, p.x, p.y);
        break;
      }
      }
      if (distVal < lowestDistance) {
        lowestDistance = distVal;
        lowestIndex = i;
      }
    }
  }
  // could possibly add in a check here to see if the lowest distance is under
  // a certain value
  return lowestIndex;
}

// task that runs all the time to determine if position correction is necessary
void correctRobotPosition(void *param) {
  while (true) {
    for (int i = 0; i < 4; i++) {
      // if the robot is on the line
      if (sensorOnLine(i)) {
        // determine which line the robot is most likely on
        int lineNum = getClosestLine(i);
        // use the angle of that line to correct the position of the robot in a
        // direction perpendicular to that of the line
        if (lineNum != -1 && lineNum > 0 && lineNum < 10) {
          point newPos = getPerpProjection(lines[lineNum],
                                           getOdomPosX() + lineSensors[i].offX,
                                           getOdomPosY() + lineSensors[i].offY);
          setOdomPosX(round(newPos.x - lineSensors[i].offX));
          setOdomPosY(round(newPos.y - lineSensors[i].offY));
        }
      }
    }
    delay(25);
  }
}
