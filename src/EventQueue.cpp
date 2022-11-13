#include "Arduino.h"
#include "EventQueue.h"


EventQueue::EventQueue() {
    init();
}


void EventQueue::init() {
  int i;

  eventQueueHead = 0;
  eventQueueTail = EVQUEUE_SIZE - 1;
  numEvents = 0;

  for (i = 0; i < EVQUEUE_SIZE; i++) {
    eventQueue[i] = Events::EV_NONE;
    eventParam[i] = 0;
  }
}


bool EventQueue::isEmpty() {
  return (numEvents == 0);
}


bool EventQueue::isFull() {
  return (eventQueueHead == eventQueueTail);
}


int EventQueue::getNumEvents() {
  return numEvents;
}


bool EventQueue::enqueueEvent(int ev_code, int ev_param) {
  if (isFull()) {
    // log the queue full error
    Serial.print(millis());
    Serial.println(" QUEUE FULL");
    return false;
  }

  // store the event
  eventQueue[eventQueueHead] = ev_code;
  eventParam[eventQueueHead] = ev_param;

  // update queue head value
  eventQueueHead = (eventQueueHead + 1) % EVQUEUE_SIZE;;

  // update number of events in queue
  numEvents++;

  return true;
}


bool EventQueue::dequeueEvent(int* ev_code, int* ev_param) {

  if (numEvents == 0) {
    return false;
  }

  eventQueueTail = (eventQueueTail + 1) % EVQUEUE_SIZE;

  // store event code and event parameter
  // into the user-supplied variables
  *ev_code = eventQueue[eventQueueTail];
  *ev_param = eventParam[eventQueueTail];

  // update number of events in queue
  numEvents--;

  return true;
}
