#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"

class Encoder
{
  public:
    Encoder();
    void begin();
    void tick();
    int getDirection();
    void readPitch(int button);
  private:
};
#endif
