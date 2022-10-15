#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include "DAC.h"

Adafruit_MCP4725 dac;

#define DAC_RESOLUTION (9)

DAC::DAC(BeatData *d, int dacNum, unsigned char addr) {
  _dPtr = d;
  _dacNum = dacNum;
  _addr = addr;
  //
}

void DAC::begin() {
  dac.begin(_addr);
  dac.setVoltage(_dPtr->pitch[_dacNum][0], false);
}
void DAC::updatePitch(int beat) {
  int pitch = _dPtr->pitch[_dacNum][beat]; //the encoder does the math for us
  dac.setVoltage(pitch, false);
}
