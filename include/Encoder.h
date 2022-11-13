#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"
#include "BeatData.h"
#include "EventQueue.h"
#include "EventDispatcher.h"

class Encoder
{
public:
  Encoder(BeatData *d, EventQueue *q, EventDispatcher *dispatcher);
  void begin();
  void read();
  void update();
  void updatePitch();
  void transpose();
  void updateTempoDiv();

private:
  BeatData *_d;
  EventQueue *_q;
  EventDispatcher *_dispatcher;

  static void _btnHandler(int event, int param, BeatData *d);
};

#endif
