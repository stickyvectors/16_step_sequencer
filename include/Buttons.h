#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"
#include "LedMatrix.h"
#include "Display.h"

class Buttons
{
  public:
    Buttons();
    //void begin(int (*pBeatStates)[16], LedMatrix *ledsPointer, int* pPitch, int* pPitchChange);
    void begin(int (*pBeatStates)[16], LedMatrix *ledsPointer, Display* pScreen, int* pPitch, int* pPitchChange);
    void read(unsigned long dt);
  private:
    int (*_pBeatStates)[16];
    int* _pPitch;
    int* _pPitchChange;
    int _activeSeq = 0;
    unsigned long _dbDelay = 25;
    unsigned long _lastdbTime[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long _holdTime[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int _buttonState[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int _buttonHold[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int _lastButtonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    LedMatrix *_ledsPointer;
    Display *_pScreen;
};
#endif
