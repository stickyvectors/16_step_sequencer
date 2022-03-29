#include "Arduino.h"
#include "LedMatrix.h"

LedMatrix::LedMatrix() {
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

void LedMatrix::step(int (*pBeatStates)[16], int activeSeq, int currentBeat) {
  int lastBeat;
  lastBeat = currentBeat - 1;
  if(lastBeat < 0) { lastBeat = 15; }
  //invert whatever the state of the current beat is
  int inverse = !pBeatStates[activeSeq][currentBeat];
  //set the current beat to be inverse and the last beat back to whatever it was before
  bitWrite(_ledData[int(currentBeat / 4)], (3 - (currentBeat % 4)), inverse);
  bitWrite(_ledData[int(lastBeat / 4)], (3 - (lastBeat % 4)), pBeatStates[activeSeq][lastBeat]);
}

void LedMatrix::switchState(int beat, int state) {
  unsigned long newbit = !!state;    // Also booleanize to force 0 or 1
  _ledData[int(beat / 4)] ^= (-newbit ^ _ledData[int(beat / 4)]) & (1UL << (3 - (beat % 4)));
}
