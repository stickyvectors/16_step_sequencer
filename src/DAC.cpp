#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include "DAC.h"

Adafruit_MCP4725 dac;

#define DAC_RESOLUTION (9)

DAC::DAC() {
  //
}

void DAC::begin(int (*pBeatStates)[16], int *pPitch, unsigned char addr) {
  _pBeatStates = pBeatStates;
  _pPitch = pPitch;
  dac.begin(addr);
  dac.setVoltage(_pPitch[0], false);
}

void DAC::updatePitch(int beat) {
  int pitch = _pPitch[beat]; //the encoder does the math for us
  dac.setVoltage(pitch, false);
}
