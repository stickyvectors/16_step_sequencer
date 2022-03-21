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
    void step(byte beatState[16], int currentBeat);
    void switchState(byte beatState[16], int beat);
  private:
    byte _dataToSend;
    byte _ledData[4] = {0, 0, 0, 0};
};
#endif
