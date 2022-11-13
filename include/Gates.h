#ifndef Gates_h
#define Gates_h

#include "Arduino.h"
#include "BeatData.h"
#include "DAC.h"
#include "LedMatrix.h"

class Gates {
public:
  Gates(BeatData *d, DAC *dA, DAC *dB, LedMatrix *leds);
  void begin();
  void update(unsigned long dt);
private:
  BeatData *_d;
  DAC *_A;
  DAC *_B;
  LedMatrix *_L;

};
#endif
