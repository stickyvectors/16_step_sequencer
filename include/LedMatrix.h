#ifndef LedMatrix_h
#define LedMatrix_h

#include <Arduino.h>
#include <BeatData.h>
#include <EventQueue.h>
#include <EventDispatcher.h>

#define dataPin 5
#define latchPin 6
#define clockPin 7

class LedMatrix
{
public:
  LedMatrix(BeatData *d, EventQueue *q, EventDispatcher *dispatcher);
  void update();
  void step();

private:
  BeatData *_d;
  EventQueue *_q;
  EventDispatcher *_dispatcher;
  byte _dataOut;
  void switchBeat(int event, int num, BeatData *_d);
};
#endif
