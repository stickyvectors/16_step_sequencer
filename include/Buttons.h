#ifndef Buttons_h
#define Buttons_h

#include <BeatData.h>
#include <EventQueue.h>
#include <EventDispatcher.h>

class Buttons
{
public:
  Buttons(BeatData *d, EventQueue *q, EventDispatcher *dispatcher);

  void begin();
  void read(unsigned long dt);

private:
  BeatData *_d;
  EventQueue *_q;
  EventDispatcher *_dispatcher;
  unsigned long _dbDelay = 25;
  unsigned long _prevTime[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int _currState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int _prevState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int _btnOn[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int _btnOff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  static void _btnHandler(int event, int param, BeatData *d);
  static void _keyRelease(BeatData *d, int num);
  void _btnManagerA();
};
#endif
