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

void LedMatrix::step(byte beatState[16], int currentBeat) {
  int lastBeat;
  if (currentBeat == 0) { lastBeat = 15; }
  else { lastBeat = currentBeat - 1; }
  //invert whatever the state of the current beat is
  byte inverse = ~beatState[currentBeat];
  //set the current beat to be inverse and the last beat back to whatever it was
  bitWrite(_ledData[int(currentBeat / 4)], (3 - (currentBeat % 4)), inverse);
  bitWrite(_ledData[int(lastBeat / 4)], (3 - (lastBeat % 4)), beatState[lastBeat]);
}

void LedMatrix::switchState(byte beatState[16], int beat) {
  byte inverse = ~beatState[beat];
  bitWrite(_ledData[int(beat / 4)], (3 - (beat % 4)), inverse);
}
