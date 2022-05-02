#ifndef LedMatrix_h
#define LedMatrix_h

#include "Arduino.h"
#include "BeatData.h"

#define latchPin 6
#define clockPin 7
#define dataPin 5

class LedMatrix
{
  public:
    LedMatrix(BeatData *d);
    void update();
    void step();
    void switchState(int beat, int state);
    void switchSequence();
  private:
    BeatData *_dPtr;
    byte _dataToSend;
    byte _ledData[4] = {0, 0, 0, 0};
};
#endif
