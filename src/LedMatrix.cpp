#include <Arduino.h>
#include <LedMatrix.h>

LedMatrix::LedMatrix(BeatData *d, EventQueue *q, EventDispatcher *dispatcher) {
  _d = d;
  _q = q;
  _dispatcher = dispatcher;
  //set pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  //lambda function for use by event dispatcher
  auto switchB = [](int event, int num, BeatData *_d) {
    unsigned long newbit = !!_d->beatStates[_d->activeSeq][num];
    Serial.println(newbit);
    _d->ledData[int(num / 4)] ^= (-newbit ^ _d->ledData[int(num / 4)]) & (1UL << (3 - (num % 4)));
  };

  //lambda for sequence switch
  auto switchS = [](int event, int num, BeatData *_d) {
    int b = 0;
    for(int r = 0; r < 4; r ++) {
      for (int c = 0; c < 4; c ++) {
        int state = _d->beatStates[num][b];
        bitWrite(_d->ledData[r], (3 - c % 4), state);
        b += 1;
      }
    }
  };

  // add event listener
  _dispatcher->addEventListener(Events::EV_BEATCHANGED, switchB);
  _dispatcher->addEventListener(Events::EV_SEQUENCECHANGED, switchS);
}

// called every frame
void LedMatrix::update() {
  for (int i = 0; i < 4; i ++) {
    _dataOut = (1 << (i + 4)) | (15 & ~_d->ledData[i]);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, _dataOut);
    digitalWrite(latchPin, HIGH);
  }
}

// called for beats of current sequence
void LedMatrix::step() {
  // retrieve current beat
  int currentBeat = _d->stepCounter[_d->activeSeq];
  // retrieve last beat
  int lastBeat = currentBeat - 1;

  if(lastBeat < 0) { lastBeat = 15; }

  // flip current beat
  int inverse = !_d->beatStates[_d->activeSeq][currentBeat];
  bitWrite(_d->ledData[int(currentBeat / 4)], (3 - (currentBeat % 4)), inverse);

  // flip previous beat
  bitWrite(_d->ledData[int(lastBeat / 4)], (3 - (lastBeat % 4)), _d->beatStates[_d->activeSeq][lastBeat]);
}
