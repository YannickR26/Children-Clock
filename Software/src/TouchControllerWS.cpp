#include <LittleFS.h>

#include "TouchControllerWS.h"

TouchControllerWS::TouchControllerWS(XPT2046_Touchscreen *touchScreen) {
  this->touchScreen = touchScreen;
}

#define calibFileName "/calibration.conf"

bool TouchControllerWS::loadCalibration() {
  // always use this to "mount" the filesystem
  bool result = LittleFS.begin();
  Serial.println("LittleFS opened: " + result);

  // this opens the file in read-mode
  File f = LittleFS.open(calibFileName, "r");

  if (!f) {
    return false;
  } 

  //Lets read line by line from the file
  String dxStr = f.readStringUntil('\n');
  String dyStr = f.readStringUntil('\n');
  String axStr = f.readStringUntil('\n');
  String ayStr = f.readStringUntil('\n');

  dx = dxStr.toFloat();
  dy = dyStr.toFloat();
  ax = axStr.toInt();
  ay = ayStr.toInt();

  f.close();

  return true;
}

bool TouchControllerWS::saveCalibration() {
  LittleFS.begin();

  // open the file in write mode
  File f = LittleFS.open(calibFileName, "w");

  if (!f) {
    Serial.println("file creation failed");
    return false;
  }

  // now write two lines in key/value style with  end-of-line characters
  f.println(dx);
  f.println(dy);
  f.println(ax);
  f.println(ay);

  f.close();

  return true;
}

void TouchControllerWS::startCalibration(CalibrationCallback *calibrationCallback) {
  state = 0;
  this->calibrationCallback = calibrationCallback;
}

void TouchControllerWS::continueCalibration() {
    TS_Point p = touchScreen->getPoint();

    if (state == 0) {
      (*calibrationCallback)(10, 10);
      if (touchScreen->touched()) {
        p1 = p;
        state++;
        lastStateChange = millis();
        Serial.printf("state 0: x: %d, y: %d, z: %d\n", p.x, p.y, p.z);
      }

    } else if (state == 1) {
      (*calibrationCallback)(230, 230);
      if (touchScreen->touched() && (millis() - lastStateChange > 1000)) {

        p2 = p;
        state++;
        lastStateChange = millis();
        Serial.printf("state 1: x: %d, y: %d, z: %d\n", p.x, p.y, p.z);

        dx = 240.0 / abs(p1.y - p2.y);
        dy = 320.0 / abs(p1.x - p2.x);
        ax = p1.y < p2.y ? p1.y : p2.y;
        ay = p1.x < p2.x ? p1.x : p2.x;
      }

    }
}

bool TouchControllerWS::isCalibrationFinished() {
  return state == 2;
}

bool TouchControllerWS::isTouched() {
  return touchScreen->touched();
}

bool TouchControllerWS::isTouched(int16_t debounceMillis) {
  if (touchScreen->touched() && ((millis() - lastTouched) > (unsigned)debounceMillis)) {
    lastTouched = millis();
    return true;
  }
  return false;
}

TS_Point TouchControllerWS::getPoint() {
    TS_Point p = touchScreen->getPoint();
    int x = (p.y - ax) * dx;
    int y = 320 - (p.x - ay) * dy;
    p.x = x;
    p.y = y;
    return p;
}

