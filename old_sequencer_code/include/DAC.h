#ifndef DAC_h
#define DAC_h

#include "Arduino.h"

class DAC
{
  public:
    DAC();
    void begin(int (*pBeatStates)[16], int* pPitch, unsigned char addr);
    void updatePitch(int beat);
  private:
    int (*_pBeatStates)[16];
    int* _pPitch;
    int _semitone = 33;
};
#endif
