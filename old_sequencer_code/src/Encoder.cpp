#include "Arduino.h"
#include "Encoder.h"

#define encA 9
#define encB 10

int counter = 0;
int dir;
bool ignoredFirstClick = false;
int pos = 0;
int currentStateA;
int lastStateA;

Encoder::Encoder() {
  //
}

void Encoder::begin(int* pPitch, int* pPitchChange) {
  _pPitch = pPitch;
  _pPitchChange = pPitchChange;
  //pin modes
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);

  lastStateA = digitalRead(encA);
}

void Encoder::tick() {
  currentStateA = digitalRead(encA);
  int currentStateB = digitalRead(encB);
  if(currentStateA != lastStateA) {
    if(ignoredFirstClick) {
      if (currentStateB != currentStateA) {
        counter --;
        dir = -1;
      }
      else {
        counter ++;
        dir = 1;
      }
      ignoredFirstClick = false;
    }
    else {
      if(currentStateB != currentStateA) {
        dir = -1;
      }
      else { dir = 1; }
      ignoredFirstClick = true;
    }
  }
  lastStateA = currentStateA;
}

int Encoder::getDirection() {
  if(pos != counter) {
    counter = pos;
    return dir;
  }
  else {
    return 0;
  }
}
void Encoder::readPitch(int button) {
  int newPos = counter;
  //if encoder turned
  if(pos != newPos) {
    int pChange = ((newPos - pos) * 33);
    if(_pPitch[button] + pChange > -1 && _pPitch[button] + pChange < 4096) {
      _pPitch[button] += pChange;
      _pPitchChange[button] = 1;
    }
    pos = newPos;
  }
}
