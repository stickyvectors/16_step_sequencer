#ifndef Display_h
#define Display_h

#include "Arduino.h"

class Display
{
  public:
    Display();
    void begin();
    void updateBasicInfo();
    void seq(int seq);
    void div(int div);
    void pitch(int pitch);
    void tick(unsigned long dt);
    void fillTriangle();
  private:
    int _seq = 1;
    int _div = 2;
    char _info[21] = "SEQ: |DIV:#01  |1/16";
    char _notes[13][3] = {{"C"}, {"C#"}, {"D"}, {"D#"},
                          {"E"}, {"F"}, {"F#"}, {"G"},
                          {"G#"}, {"A"}, {"A#"}, {"B"}};
    unsigned long _stateTimer = 0;
    bool _update = false;
};
#endif
