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
    void update(byte* beatStates[5][16], int currentSeq, int currentBeat);
  private:
    byte _dataToSend;
    byte _ledData[4] = {0,0,0,0};
};
#endif
