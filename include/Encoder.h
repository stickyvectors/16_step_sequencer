#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"
#include "BeatData.h"

class Encoder
{
  public:
    Encoder(BeatData *d);
    void begin();
    void tick();
    int getDirection();
    void readPitch(int button);
  private:
    BeatData *_dPtr;
};

#endif
