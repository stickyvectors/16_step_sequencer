#ifndef DAC_h
#define DAC_h

#include "Arduino.h"

class DAC
{
  public:
    DAC();
    void begin(int dacNum, unsigned char addr);
    void updatePitch(int beat);
  private:
    int _dacNum;
    int _semitone = 33;
};
#endif
