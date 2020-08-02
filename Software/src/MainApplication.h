#pragma once

#include "tRules.h"
// #include "DFPlayer.h"

class MainApplication
{
  public:
    MainApplication();
    virtual ~MainApplication();
  
    void setup(void);
    void handle(void);

  protected:
    void checkRules(void);
    tRules& searchLastRules(void);
    void applyRule(tRules &rule);
    void printTime(void);
    void updateNTP(void);
    void updateScreen(void);

  private:
    tRules              _currentRule, _nextRule;
    // DFPlayer            _player;
    int                 _luminosity;
};
