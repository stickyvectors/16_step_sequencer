#include "Arduino.h"
#include "Encoder.h"
#include "BeatData.h"

#define encA 9
#define encB 10

extern BeatData beatData;

int counter = 0;
int dir;
bool ignoredFirstClick = false;
int pos = 0;
int currentStateA;
int lastStateA;

Encoder::Encoder() {
  //
}

void Encoder::begin() {
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
    if(beatData.pitch[beatData.activeSeq][button] + pChange > -1 && beatData.pitch[beatData.activeSeq][button] + pChange < 4096) {
      beatData.pitch[beatData.activeSeq][button] += pChange;
      beatData.changedPitch[beatData.activeSeq][button] = 1;
    }
    pos = newPos;
  }
}
