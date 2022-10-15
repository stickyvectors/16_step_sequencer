#ifndef Display_h
#define Display_h

#include "Arduino.h"

class Display
{
  public:
    Display();
    void begin();
    void updateBasicInfo();
    void home();
    void seq(int seq);
    void div(int div);
    void swing(float s);
    void pitch(int pitch);
    void fillTriangle();
    bool returnHome = false;
  private:
    int _seq = 1;
    int _div = 2;
    /*const char _notes[13][3] = {{"C"}, {"C#"}, {"D"}, {"D#"},
                          {"E"}, {"F"}, {"F#"}, {"G"},
                          {"G#"}, {"A"}, {"A#"}, {"B"}};*/
    unsigned long _stateTimer = 0;
    bool _update = false;
};
#endif
