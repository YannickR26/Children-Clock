#ifndef _MAINAPPLICATION_h
#define _MAINAPPLICATION_h

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Ticker.h>

#include "tRules.h"
#include "HttpServer.h"
#include "DFPlayer.h"
#include "Strip.h"

class MainApplication
{
  public:
    MainApplication();
    virtual ~MainApplication();
  
    void setup(void);
    void handle(void);

  private:
    void checkRules(void);
    tRules& searchLastRules(void);
    void applyRule(tRules &rule);
    void printTime(void);
    void updateNTP(void);

  private:
    Ticker              _tickerEvery1sec;
    WiFiManager         _wifiManager;
    DFPlayer            _player;
    int                 luminosity;
};

#endif

