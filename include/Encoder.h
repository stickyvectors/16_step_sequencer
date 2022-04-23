#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"

class Encoder
{
  public:
    Encoder();
    void begin(int* pPitch, int* pPitchChange);
    void tick();
    int getDirection();
    void readPitch(int button);
  private:
    int* _pPitch;
    int* _pPitchChange;
};
#endif
