#include "Arduino.h"
#include "Encoder.h"
#include <Events.h>

#define encA 9
#define encB 10

int counter = 0;
int pos = 0;
int dir;
int currentStateA;
int lastStateA;
bool ignoredFirstClick = false;

int readEncoder = 0;
int targetButton = -1;

int sentEvent = 0;

Encoder::Encoder(BeatData *d, EventQueue *q, EventDispatcher *dispatcher) {
  _d = d;
  _q = q;
  _dispatcher = dispatcher;
}

void Encoder::_btnHandler(int event, int param, BeatData *d) {
  switch(event) {
    case Events::EV_KEY_PRESS:
      readEncoder = 1;
      targetButton = param;
      break;

    case Events::EV_KEY_RELEASE:
        readEncoder = 0;
        targetButton = -1;
        sentEvent = 0;
        break;
  }
}

void Encoder::begin() {
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);

  lastStateA = digitalRead(encA);

  _dispatcher->addEventListener(Events::EV_KEY_PRESS, _btnHandler);
  _dispatcher->addEventListener(Events::EV_KEY_RELEASE, _btnHandler);
}

void Encoder::read() {
  if(readEncoder == 1) {
    update();
    if(targetButton < 16 && targetButton >= 0) {
      updatePitch();
    }
    else if(targetButton == 16) {
      //then it's a mod button
      updateTempoDiv();
    }
    else if(targetButton == 18) {
      //then its transpose
      transpose();
    }
  }
}

void Encoder::update() {
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

void Encoder::updatePitch() {
  int newPos = counter;
  if(pos != newPos) {

    if(!sentEvent) {
      //tell buttons the encoder moved (the param does nothing)
      _q->enqueueEvent(Events::EV_ENCODER, 0);
      sentEvent = 1;
    }

    int pChange = ((newPos - pos) * 33);
    int pitch = _d->pitch[_d->activeSeq][targetButton] + pChange;

    if(pitch > -1 && pitch < 4096) {
      _d->pitch[_d->activeSeq][targetButton] = pitch;
      Serial.print("pitch: ");
      Serial.println(pitch);
    }

    pos = newPos;
  }
}

void Encoder::transpose() {
  int newPos = counter;
  if(pos != newPos) {
    if(!sentEvent) {
      _q->enqueueEvent(Events::EV_ENCODER, 0);
      sentEvent = 1;
    }

    int pChange = ((newPos - pos) * 33 * 12);
    int canTranspose = 1;
    for(int i = 0; i < 16; i ++) {
      int pitch = _d->pitch[_d->activeSeq][i] + pChange;
      if(pitch < 0 && pitch > 4095) {
        Serial.println("CAN'T TRANSPOSE");
        canTranspose = 0;
        break;
      }
    }
    if(canTranspose == 1) {
      for(int i = 0; i < 16; i ++) {
        int pitch = _d->pitch[_d->activeSeq][i] + pChange;
        _d->pitch[_d->activeSeq][i] = pitch;
      }
      Serial.println("TRANSPOSED SEQ");
    }
    pos = newPos;
  }
}

void Encoder::updateTempoDiv() {
  int newPos = counter;
  if(pos != newPos) {
    if(!sentEvent) {
      _q->enqueueEvent(Events::EV_ENCODER, 0);
      sentEvent = 1;
    }

    int tDivChange = (newPos - pos);
    int div = _d->tempoDiv[_d->activeSeq] + tDivChange;

    if(div > -1 && div < 10) {
      Serial.print("SWITCHED DIVISION TO: ");
      Serial.println(_d->division[div]);
      _d->tempoDiv[_d->activeSeq] = div;
    }
    pos = newPos;
  }
}
