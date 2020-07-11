#include <MiniGrafx.h>
#include <ILI9341_SPI.h>

#include "TouchControllerWS.h"
#include "font.h"

#define MINI_BLACK  0
#define MINI_WHITE  1
#define MINI_YELLOW 2
#define MINI_BLUE   3

#define SCREEN_X 285
#define SCREEN_Y 240

int board_readNTC();
float board_getPower();

const String dayName[] = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche"};
const String monthName[] = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"};
const String monthNameShort[] = {"Janv.", "févr.", "mars", "avr.", "mai", "juin", "juil.", "août", "sept.", "oct.", "nov.", "déc."};

// defines the colors usable in the paletted 16 color frame buffer
uint16_t palette[] = {ILI9341_BLACK,  // 0
                      ILI9341_WHITE,  // 1
                      ILI9341_YELLOW, // 2
                      ILI9341_BLUE};  // 3

// Limited to 4 colors due to memory constraints
int BITS_PER_PIXEL = 2; // 2^2 =  4 colors

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
TouchControllerWS touchController(&ts);

void touchCalibration();
void calibrationCallback(int16_t x, int16_t y);
CalibrationCallback calibration = &calibrationCallback;

void screen_begin() {

  // The LED pin needs to set LOW
  // Use this pin to save energy
  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  delay(100);
  digitalWrite(TFT_LED, LOW);
  delay(200);
  
#define ILI9341_SWRESET 0x01
  tft.writecommand(ILI9341_SWRESET);  
  gfx.init();
  gfx.fillBuffer(MINI_BLACK);
  gfx.setRotation(3);
  gfx.commit();
  
  ts.begin();
  /*boolean isCalibrationAvailable = touchController.loadCalibration();
  if (!isCalibrationAvailable) {
    touchCalibration();
  } */
}

void calibrationCallback(int16_t x, int16_t y) {
  gfx.setColor(MINI_WHITE);
  gfx.fillCircle(x, y, 10);
}

void touchCalibration() {
  Serial.println("Touchpad calibration .....");
  touchController.startCalibration(&calibration);
  while (!touchController.isCalibrationFinished()) {
    gfx.fillBuffer(MINI_BLACK);
    gfx.setColor(MINI_YELLOW);
    gfx.setTextAlignment(TEXT_ALIGN_CENTER);
    gfx.drawString(140, 100, "Please calibrate\ntouch screen by\ntouch point");
    touchController.continueCalibration();
    gfx.commit();
    yield();
  }
  gfx.fillBuffer(MINI_BLACK);
  gfx.setColor(MINI_YELLOW);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.drawString(140, 100, "Calibration successfull");
  gfx.commit();
  touchController.saveCalibration();
}


// draws the clock
void drawTime() {
  int x = SCREEN_X / 2;
  int y = SCREEN_Y / 3;

  char time_str[20];
  char date_str[50];
  char *dstAbbrev;
  time_t now = dstAdjusted.time(&dstAbbrev);
  struct tm *timeinfo = localtime(&now);

  if (IS_STYLE_12HR)
  {
    int hour = (timeinfo->tm_hour+11)%12+1;  // take care of noon and midnight
    if (IS_SHOW_SECONDS) {
      sprintf(time_str, "%2d:%02d:%02d\n", hour, timeinfo->tm_min, timeinfo->tm_sec);
    }
    else {
      sprintf(time_str, "%2d:%02d\n", hour, timeinfo->tm_min);
    }
  } else {
    if (IS_SHOW_SECONDS) {
      sprintf(time_str, "%02d:%02d:%02d\n",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }
    else {
      sprintf(time_str, "%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min);
    }
  }

  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_WHITE);
  gfx.setFont(Schoolbell_Regular_48);
  gfx.drawString(x, y-20, time_str);

  gfx.setFont(Schoolbell_Regular_24);
  sprintf(date_str, "%s %02d %s\n", dayName[timeinfo->tm_wday-1].c_str(), timeinfo->tm_mday, monthName[timeinfo->tm_mon].c_str());
  gfx.drawString(x, y+50, date_str);
}

void drawProgress(uint8_t percentage, String text)
{
  gfx.fillBuffer(MINI_BLACK);
  gfx.setFont(ArialMT_Plain_16);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_YELLOW);
  gfx.drawString(140, 100, text);
  gfx.setColor(MINI_WHITE);
  gfx.drawRect(10, 128, SCREEN_X - 20, 15);
  gfx.setColor(MINI_BLUE);
  gfx.fillRect(12, 130, (SCREEN_X-24) * percentage / 100, 11);

  gfx.commit();
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  static int32_t dbm;
  dbm = (dbm + WiFi.RSSI()) / 2;
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}

void drawWifiQuality()
{
  int8_t quality = getWifiQuality();
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawString(SCREEN_X - 20, 9, String(quality) + "%");
  for (int8_t i = 0; i < 4; i++) {
    for (int8_t j = 0; j < 2 * (i + 1); j++) {
      if (quality > i * 25 || j == 0) {
        gfx.setPixel(SCREEN_X - 18 + 2 * i, 18 - j);
      }
    }
  }
}

void drawBattery() {
  uint8_t percentage = 100;
  
  float power = board_getPower();
  
  if (power > 4.15) percentage = 100;
  else if (power < 3.7) percentage = 0;
  else percentage = (power - 3.7) * 100 / (4.15-3.7);
  
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);  
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawString(26, 9, String(percentage) + "%");
  gfx.drawRect(1, 11, 18, 9);
  gfx.drawLine(21,13,21,17);  
  gfx.drawLine(22,13,22,17);  
  gfx.setColor(MINI_BLUE); 
  gfx.fillRect(3, 13, 15 * percentage / 100, 5);
}

void drawTemperature() {
  static float temperature;
  
  temperature = (temperature + (board_readNTC() / 10.0)) / 2;
  
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);  
  gfx.setFont(Schoolbell_Regular_24);
  char tmp[10];
  sprintf(tmp, "%.1f °C", temperature);
  gfx.drawString(SCREEN_X/2, SCREEN_Y-40, tmp);
}
