#ifndef LedMatrix_h
#define LedMatrix_h

#include "Arduino.h"

#define latchPin 6
#define clockPin 7
#define dataPin 5

class LedMatrix
{
  public:
    LedMatrix();
    void update();
    void step(int (*pBeatStates)[16], int activeSeq, int currentBeat);
    void switchState(int beat, int state);
  private:
    byte _dataToSend;
    byte _ledData[4] = {0, 0, 0, 0};
};
#endif
