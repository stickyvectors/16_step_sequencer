#ifndef DAC_h
#define DAC_h

#include "Arduino.h"
#include "BeatData.h"

class DAC
{
  public:
    DAC(BeatData *d, int dacNum, unsigned char addr);
    void begin();
    void updatePitch(int beat);
  private:
    BeatData *_dPtr;
    int _dacNum;
    unsigned char _addr;
    int _semitone = 33;
};
#endif
