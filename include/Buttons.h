#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"

class Buttons
{
  public:
    Buttons();
    //ideally this would be a pointer for beatStates so we don't have to return
    void begin(byte (*beatStatesPointer)[16]);
    void read(byte (*beatStatesPointer)[16]);
    //byte beatStates[5][16];
  private:
    int _activeSeq = 0;
    unsigned long _dbDelay = 25;
    unsigned long _lastdbTime[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long _holdTime[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int _buttonState[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int _buttonHold[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int _lastButtonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};
#endif
