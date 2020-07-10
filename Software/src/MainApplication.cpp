#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include <simpleDSTadjust.h>
#include <FS.h>

// #define ENABLE_OTA    // If defined, enable Arduino OTA code.
//#define WIFI_AP   // If defined, enable Wifi in AP Mode

// OTA
#ifdef ENABLE_OTA
  #include <ArduinoOTA.h>
#endif

#include "JsonConfiguration.h"
#include "MainApplication.h"
#include "settings.h"

simpleDSTadjust dstAdjusted(StartRule, EndRule);

#include "board.h"
#include "screen.h"

// Global variable for Tick
bool    readyForNtpUpdate;
bool    readyForScreenUpdate;

// Ticker every 1 seconds
void secTicker()
{
  static int tickNTPUpdate = NTP_UPDATE_INTERVAL_SEC;
  static int tickScreenUpdate = SCREEN_UPDATE_INTERVAL_SEC;

  tickNTPUpdate--;
  if (tickNTPUpdate <= 0) {
    readyForNtpUpdate = true;
    tickNTPUpdate = NTP_UPDATE_INTERVAL_SEC;
  }

  tickScreenUpdate--;
  if (tickScreenUpdate <= 0) {
    readyForScreenUpdate = true;
    tickScreenUpdate = SCREEN_UPDATE_INTERVAL_SEC;
  }
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager) {
  drawProgress(50, "Launch Wifi in AP Mode...");
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

MainApplication::MainApplication()
{
  delay(500);
  
  Serial.begin(115200);
  while (!Serial); // wait for serial attach
}

MainApplication::~MainApplication()
{
}

void MainApplication::setup(void)
{
  char text[50];

  delay(2000);

  Serial.println("Initializing...");
  Serial.flush();

  // WiFi.disconnect();

  // Ticker every 1 seconds
  _tickerEvery1sec.attach(1, secTicker);

  // Init screen
  screen_begin();

  // Check if update is available
  board_checkUpdate();

  // Init Led
  drawProgress(10, "Launch LED...");
  strip.setup(NB_LED, PIN_LED, NEO_GRB + NEO_KHZ800);
  strip.setMode(MODE_NO_MODE);
  strip.setAllColor(tColor());
  strip.setSpeed(10);
  strip.start();
  delay(200);

  // Read configuration
  drawProgress(20, "Updating configuration...");
  Configuration.setup();
  // Configuration.restoreDefault();
  delay(200);

  uint8_t val = 30;
  drawProgress(val, "Connecting Wifi...");
  #ifdef WIFI_MANAGER
    // Connect to Wifi client
    _wifiManager.setDebugOutput(true);
    //reset settings - for testing
    //_wifiManager.resetSettings();

    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    _wifiManager.setAPCallback(configModeCallback);
    //fetches ssid and pass and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    if (!_wifiManager.autoConnect(Configuration._hostname.c_str(), Configuration._hostname.c_str())) {
      Serial.println("failed to connect and hit timeout");
      //reset and try again, or maybe put it to deep sleep
      board_systemRestart();
      delay(1000);
    }
    
    WiFi.enableAP(false);
    WiFi.softAPdisconnect();
  #elif WIFI_AP
    Serial.println("Configuring Wifi access point...");
    WiFi.softAP(Configuration._hostname.c_str(), Configuration._passwd.c_str());
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  #else
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
  #endif

  int nbTry = 20;
  Serial.print("Connecting Wifi...");
  while ((WiFi.status() != WL_CONNECTED) && nbTry) {
    delay(500);
    Serial.print(".");
    nbTry--;
    val += 3;
    drawProgress(val, "Connecting Wifi...");
  }

  Serial.println();
  if (nbTry == 0 || !WiFi.isConnected()) {
    drawProgress(val, "Impossible to connect Wifi");
    Serial.println("Impossible to connect Wifi");
    delay(2000);
    ESP.reset();
  }
  else {
    // Wifi connected
    sprintf(text, "WiFi connected to: %s", WiFi.SSID().c_str());
    drawProgress(val, text);
    Serial.println(text);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  delay(1000);

  // Launch Server HTTP and FTP
  drawProgress(80, "Launch Server...");
  HTTPServer.setup();
  delay(200);

  // Get Clock from NTP server
  drawProgress(90, "Updating time...");
  updateNTP();
  delay(200);

  // Init MP3 player
  // _player.setup();

  // Search last rules and Apply it
  applyRule(searchLastRules());

  // Init OTA
  #ifdef ENABLE_OTA
    Serial.println("Arduino OTA activated.");
    
    // Port defaults to 8266
    ArduinoOTA.setPort(8266);
  
    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(Configuration._hostname.c_str());
  
    // No authentication by default
    //ArduinoOTA.setPassword("admin");
  
    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  
    ArduinoOTA.onStart([&]() {
      strip.setMode(MODE_FADE_IN_OUT);
      strip.setBrightness(80);
      strip.setAllColor(tColor(0, 127, 127));
      Serial.println("Arduino OTA: Start updating");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("Arduino OTA: End");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Arduino OTA Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Arduino OTA Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Arduino OTA: Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Arduino OTA: Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Arduino OTA: Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Arduino OTA: Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("Arduino OTA: End Failed");
    });
  
    ArduinoOTA.begin();
  #endif
  Serial.println("setup finished !");
}

void MainApplication::handle(void)
{
  static int oldMillis;
  
  #ifdef ENABLE_OTA
    ArduinoOTA.handle();
  #endif
  
  HTTPServer.handle();
  // _player.handle();
  strip.handle();

  checkRules();

  // if (!WiFi.isConnected()) {
  //   WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
  //   Serial.println("disconnected");
  //   delay(1000);
  // }

  if (touchController.isTouched(1000)) {
    TS_Point p = touchController.getPoint();
    Serial.printf("Screen touched => x: %d, y: %d\n", p.x, p.y);

    //digitalWrite(TFT_LED, HIGH);
  }

  if (readyForNtpUpdate) {
    updateNTP();
    readyForNtpUpdate = false;
  }

  if (readyForScreenUpdate) {
    gfx.fillBuffer(MINI_BLACK);
    //gfx.setColor(MINI_WHITE);
    //gfx.drawPalettedBitmapFromFile(10, 100, "SpriteFont_Calibri_Light_(72)_40x52.bmp");
    drawTime();
    drawBattery();
    drawWifiQuality();
    drawTemperature();
    gfx.commit();
    readyForScreenUpdate = false;
  }

  /* Read luminosity every 50 ms */
  if (abs(millis() - oldMillis) > 50 ) {
    oldMillis = millis();
    // read the value from the sensor
    int sensorValue = analogRead(A0);
    luminosity = luminosity*0.9 + map(sensorValue, 0, 1023, Configuration._maxIntensity, 1)*0.1;
    strip.setBrightness(luminosity);
    // Serial.printf("sensorValue: %d, luminosity: %d, map: %d\n", sensorValue, luminosity, map(sensorValue, 0, 1023, 50, 1));
  }
}

void MainApplication::checkRules(void)
{
  // Get current date time
  char *dstAbbrev;
  time_t now = dstAdjusted.time(&dstAbbrev);
  struct tm * timeinfo = localtime(&now);
  static int previousTimeSec;

  // wait 1s passed
  if (timeinfo->tm_sec == previousTimeSec)
    return;

  previousTimeSec = timeinfo->tm_sec;

  tRules curTime(timeinfo->tm_wday, timeinfo->tm_hour, timeinfo->tm_min);

  for (std::vector<tRules>::iterator it = Configuration._tabRules.begin() ; it != Configuration._tabRules.end() ; ++it) {
    if ((*it).enable) {
      if ((*it) == curTime) {
        applyRule((*it));
      }
    }
  }
}

tRules& MainApplication::searchLastRules(void)
{
  static tRules rule;
  int diffTime, oldDiffTime=9999;

  // Get current date time
  char *dstAbbrev;
  time_t now = dstAdjusted.time(&dstAbbrev);
  struct tm * timeinfo = localtime(&now);
  
  tRules curTime(timeinfo->tm_wday, timeinfo->tm_hour, timeinfo->tm_min);
  
  for (std::vector<tRules>::iterator it = Configuration._tabRules.begin() ; it != Configuration._tabRules.end() ; ++it) {
    if ((*it).enable) {
      diffTime = curTime-(*it);
      if (diffTime < oldDiffTime) {
        oldDiffTime = diffTime;
        rule = (*it);
      }
    }
  }

  Serial.print("Last rule found is: ");
  Serial.print(rule.toString().c_str());
  Serial.printf(", after %d min\n", oldDiffTime);      
  return rule;
}

void MainApplication::applyRule(tRules &rule) 
{
  static tRules curRule = tRules();
  
  if (rule == curRule) return;

  curRule = rule;
  strip.setAllColor(curRule.color);
  printTime();
  Serial.print("Change rules by: ");
  Serial.println(curRule.toString().c_str());  
}

void MainApplication::printTime(void)
{
  char buf[30];
  char *dstAbbrev;
  time_t t = dstAdjusted.time(&dstAbbrev);
  struct tm *timeinfo = localtime (&t);

  sprintf(buf, "%d, %d/%d/%d, %02d:%02d:%02d %s => ", timeinfo->tm_wday, timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, dstAbbrev);
  Serial.print(buf);
  Serial.flush();
}

void MainApplication::updateNTP(void)
{
  time_t now = time(nullptr);

  printTime();
  Serial.println("Old Time");

  Serial.print("UpdateNTP...");
  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
  while (now < EPOCH_1_1_2019) {
    now = time(nullptr);
    Serial.print(".");
    delay(500);
  }
  delay(100);
  Serial.println(" Done !");

  printTime();
  Serial.println("New Time");
}
