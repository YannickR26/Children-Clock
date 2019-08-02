#ifndef _DFPLAYER_H_
#define _DFPLAYER_H_

#include <DFRobotDFPlayerMini.h>

class DFPlayer {
  public:
    DFPlayer();
    virtual ~DFPlayer();

    void setup(void);
    void handle(void);

    void play(uint8_t song_);
    void stop(void);

  private:
    void printDetail(uint8_t type, int value);
    
  private:
    DFRobotDFPlayerMini _player;
};

#endif
