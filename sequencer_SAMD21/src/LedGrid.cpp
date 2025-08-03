#include <LedGrid.h>

LedGrid::LedGrid(Data *dataPtr) {
    data = dataPtr;
    // Do not use SerialUSB.print inside here as it is called before SerialUSB.begin()!!!
}

// called in setup
void LedGrid::begin() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}


void LedGrid::fastShiftOutMSBFirst(uint8_t ledData) {
  for (int i = 7; i >= 0; i--) {
    if (ledData & (1 << i)) {
      DATA_PORT.OUTSET.reg = DATA_BIT;
    } else {
      DATA_PORT.OUTCLR.reg = DATA_BIT;
    }
    CLOCK_PORT.OUTSET.reg = CLOCK_BIT;
    CLOCK_PORT.OUTCLR.reg = CLOCK_BIT;
  }
}

// called in TC5 handler
void LedGrid::update() {
    // Just test shift register output: row bits HIGH, column bits LOW (all on)
    //LATCH_PORT.OUTCLR.reg = LATCH_BIT;
    //fastShiftOutMSBFirst(0b11110000);  // Row 0 active, all columns ON
    //LATCH_PORT.OUTSET.reg = LATCH_BIT;

    
    // Row select (only one active row)
    uint8_t rowBits = 1 << (7 - currentRow);  // bits 7–4

    // Extract 4 column bits for this row
    uint8_t shift = (3 - currentRow) * 4;     // shift to correct nibble
    uint8_t cols = (~(data->ledMask >> shift)) & 0x0F;  // active LOW logic

    uint8_t dataByte = rowBits | cols;

    LATCH_PORT.OUTCLR.reg = LATCH_BIT;
    fastShiftOutMSBFirst(dataByte);
    LATCH_PORT.OUTSET.reg = LATCH_BIT;

    currentRow = (currentRow + 1) % 4;
}