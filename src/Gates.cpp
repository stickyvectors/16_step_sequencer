#include "Gates.h"
#include <Adafruit_MCP23X17.h>

// divisions function as follows:
// 1/32 notes = 2
// 1/16 notes = 4
// 1/12 notes = 6 = 1/16 div + (1/16 div * 0.5)
// 1/8 notes = 8
// 1/4 notes = 16
// 1/3 notes = 24
// 1/2 notes = 32
// whole note = 64

//-STEPS--| 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 ...etc.
//--1/32--| X  *  X  *  X  *  X  *  X  *  X  *  X  *  X  *  X  *  X  *  X
//--1/24--| X  *  *  X  *  *  X  *  *  X  *  *  X  *  *  X  *  *  X  *  *
//--1/16--| X  *  *  *  X  *  *  *  X  *  *  *  X  *  *  *  X  *  *  *  X
//--1/12--| X  *  *  *  *  *  X  *  *  *  *  *  x  *  *  *  *  *  x  *  *
//--1/8---| X  *  *  *  *  *  *  *  X  *  *  *  *  *  *  *  X  *  *  *  *
//--1/6---| X  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
//--1/4---| X  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  X  *  *  *  *
//--1/3---| X  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
//--1/2---| X  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * >next beat step 32


Adafruit_MCP23X17 mult;

Gates::Gates(BeatData *d, DAC *dA, DAC *dB, LedMatrix *leds) {
  _d = d;
  _A = dA;
  _B = dB;
  _L = leds;
}

void Gates::begin() {
  if(!mult.begin_I2C(0x21)) {
    Serial.println("MCP #2 0x21 error.");
    while (1);
  }

  for (int i = 0; i < 8; i ++) {
    mult.pinMode(i, OUTPUT);
  }
}

//called every 1/64 beat
void Gates::update(unsigned long dt) {

  int step = _d->stepNumber;

  for(int i = 0; i < 8; i ++) {
    //first see if there's a high gate that needs turned off
    if(_d->gate[i]) {
      //there is, turn this gate off
      mult.digitalWrite(i, LOW);
      _d->gate[i] = 0;
    }

    // modulus our tempo over step, if it's zero, it fits this division
    if(step % _d->division[_d->tempoDiv[i]] == 0) {

      // step LEDs forward
      if(i == _d->activeSeq) {
        _L->step();
      }

      //then we are at a beat for this tempo division
      if(_d->beatStates[i][_d->stepCounter[i]] == 1) {
        //if that beat is active, let's fire the gate and pitch!
        if(!_d->gate[i]) {
          //send gate high
          mult.digitalWrite(i, HIGH);
          _d->gate[i] = 1;

          if(i == 0) {
            //update pitch
            _A->updatePitch(_d->stepCounter[i]);
          }
          if(i == 1) {
            //update pitch
            _B->updatePitch(_d->stepCounter[i]);
          }
        }
      }

      //increment the step counter for this sequence
      _d->stepCounter[i] += 1;

      if(_d->stepCounter[i] == 16) {
        _d->stepCounter[i] = 0; //reset back to beginning after this many 16 steps
      }
    }
  }
}
