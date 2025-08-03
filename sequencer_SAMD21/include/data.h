#ifndef Data_h
#define Data_h

#include <Arduino.h>

struct Data
{
    // clock vars
    int bpm = 120; //default 120bpm
    volatile int globalQN = 0; //quarter note count

    // LED state info
    volatile uint16_t ledMask = 0b1111111111111111; // example LED state

};

#endif