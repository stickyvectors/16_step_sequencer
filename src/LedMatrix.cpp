#include "Arduino.h"
#include "LedMatrix.h"

LedMatrix::LedMatrix(BeatData *d) {
  _dPtr = d;
  //
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}
//call this every frame
void LedMatrix::update() {
  for (int i = 0; i < 4; i ++) {
    _dataToSend = (1 << (i + 4)) | (15 & ~_ledData[i]);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, _dataToSend);
    digitalWrite(latchPin, HIGH);
  }
}

void LedMatrix::step() {
  int currentBeat = _dPtr->stepCounter[_dPtr->activeSeq];
  int lastBeat;
  lastBeat = currentBeat - 1;
  if(lastBeat < 0) { lastBeat = 15; }
  //invert whatever the state of the current beat is
  int inverse = !_dPtr->beatStates[_dPtr->activeSeq][currentBeat];
  //set the current beat to be inverse and the last beat back to whatever it was before
  bitWrite(_ledData[int(currentBeat / 4)], (3 - (currentBeat % 4)), inverse);
  bitWrite(_ledData[int(lastBeat / 4)], (3 - (lastBeat % 4)), _dPtr->beatStates[_dPtr->activeSeq][lastBeat]);
}

void LedMatrix::switchState(int beat, int state) {
  unsigned long newbit = !!state;    // Also booleanize to force 0 or 1
  _ledData[int(beat / 4)] ^= (-newbit ^ _ledData[int(beat / 4)]) & (1UL << (3 - (beat % 4)));
}

void LedMatrix::switchSequence() {
  int b = 0;
  for(int r = 0; r < 4; r ++) {
    for(int c = 0; c < 4; c ++) {
      int state = _dPtr->beatStates[_dPtr->activeSeq][b];
      bitWrite(_ledData[r], (3 - c % 4), state);
      b += 1;
    }
    byte _dataToSend = (1 << (r + 4)) | (15 & ~_ledData[r]);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, _dataToSend);
    digitalWrite(latchPin, HIGH);
  }
}
