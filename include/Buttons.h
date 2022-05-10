#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"
#include "BeatData.h"
#include "LedMatrix.h"
#include "Display.h"
#include "Encoder.h"

class Buttons
{
  public:
    Buttons(BeatData *d, LedMatrix *ledsPointer, Display* pScreen);
    //void begin(int (*pBeatStates)[16], LedMatrix *ledsPointer, int* pPitch, int* pPitchChange);
    void begin();
    void read(unsigned long dt);
    void fireGate(int gate);
  private:
    unsigned long _dbDelay = 25;
    unsigned long _lastdbTime[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long _holdTime[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int _buttonState[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int _buttonHold[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int _lastButtonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    BeatData *_dPtr;
    LedMatrix *_ledsPointer;
    Display *_pScreen;
    Encoder *_enc;
};
#endif
