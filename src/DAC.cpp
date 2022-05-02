#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include "DAC.h"
#include "BeatData.h"

Adafruit_MCP4725 dac;

extern BeatData beatData;

#define DAC_RESOLUTION (9)

DAC::DAC() {
  //
}

void DAC::begin(int dacNum, unsigned char addr) {
  dac.begin(addr);
  _dacNum = dacNum;
  dac.setVoltage(beatData.pitch[dacNum][0], false);
}

void DAC::updatePitch(int beat) {
  int pitch = beatData.pitch[_dacNum][beat]; //the encoder does the math for us
  dac.setVoltage(pitch, false);
}
