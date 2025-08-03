#ifndef Ledgrid_h
#define Ledgrid_h

#include <Arduino.h>
//#include <Adafruit_MCP23X17.h>
#include <data.h>

class LedGrid {
    public:
        LedGrid(Data *dataPtr);
        Data *data;
        void begin();
        void update();
        void fastShiftOutMSBFirst(uint8_t ledData);
    private:
        #define latchPin 5
        #define clockPin 4
        #define dataPin 3

        #define DATA_PORT  PORT->Group[0]
        #define CLOCK_PORT PORT->Group[0]
        #define LATCH_PORT PORT->Group[0]

        #define LATCH_BIT   (1 << 15)
        #define CLOCK_BIT  (1 << 8)
        #define DATA_BIT  (1 << 9)

        volatile uint8_t currentRow = 0;
};

#endif