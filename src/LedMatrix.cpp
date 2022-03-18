#include "Arduino.h"
#include "LedMatrix.h"

LedMatrix::LedMatrix() {
  //
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void LedMatrix::update(byte* beatStates[5][16], int currentSeq, int currentBeat) {
  for (int i = 0; i < 4; i ++) {
    _dataToSend = (1<< (i + 4)) | (15 & ~ _ledData[i]);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, _dataToSend);
    digitalWrite(latchPin, HIGH);
  }
}
