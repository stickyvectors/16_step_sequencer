#include <Arduino.h>
#include <Wire.h>
#include "DAC.h"

//Adafruit_MCP4725 dac;

#define DAC_RESOLUTION (9)

DAC::DAC(BeatData *d, Adafruit_MCP4725 mcp, int dacNum, unsigned char addr) {
  _dPtr = d;
  _dacNum = dacNum;
  _addr = addr;
  dac = mcp;
  //
}

void DAC::begin() {
  dac.begin(_addr);
  dac.setVoltage(_dPtr->pitch[_dacNum][0], false);
}
void DAC::updatePitch(int beat) {
  int pitch = _dPtr->pitch[_dacNum][beat]; //the encoder does the math for us
  dac.setVoltage(pitch, false);
  //Serial.print("dac ");
  //Serial.print(_addr);
  //Serial.print(" set to: ");
  //Serial.println(pitch);
}
