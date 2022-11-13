#ifndef DAC_h
#define DAC_h

#include "Arduino.h"
#include "Adafruit_MCP4725.h"
#include "BeatData.h"

class DAC
{
  public:
    DAC(BeatData *d, Adafruit_MCP4725 mcp, int dacNum, unsigned char addr);
    void begin();
    void updatePitch(int beat);
  private:
    BeatData *_dPtr;
    Adafruit_MCP4725 dac;
    int _dacNum;
    unsigned char _addr;
    int _semitone = 33;
};
#endif
