// LEDS
#define latchPin 5
#define clockPin 4
#define dataPin  3

#define DATA_PIN 3
#define CLOCK_PIN 4
#define LATCH_PIN 5

#define DATA_PORT  PORT->Group[0]   // PORTA
#define CLOCK_PORT PORT->Group[0]
#define LATCH_PORT PORT->Group[0]

#define DATA_BIT   (1 << 9)  // PA09
#define CLOCK_BIT  (1 << 8)   // PA08
#define LATCH_BIT  (1 << 15)   // PA15

byte ledData[4] = {0b1111, 0b1111, 0b1111, 0b1111};

void setup(){
  SerialUSB.begin(9600);

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
}
    
void updateLEDs() {
  for(int r = 0; r < 4; r ++) {
    for(int l = 0; l < 4; l ++) {
      byte data = (0b0001 << (4 + 3 - r)) | (15 & ~ledData[l]);
      // Update shift register
      LATCH_PORT.OUTCLR.reg = LATCH_BIT;     // Latch low
      fastShiftOutLSBFirst(data);            // Shift data
      LATCH_PORT.OUTSET.reg = LATCH_BIT; 
      delayMicroseconds(100);
    }
  }
}

void fastShiftOutLSBFirst(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            DATA_PORT.OUTSET.reg = DATA_BIT;
        } else {
            DATA_PORT.OUTCLR.reg = DATA_BIT;
        }

        CLOCK_PORT.OUTSET.reg = CLOCK_BIT;
        CLOCK_PORT.OUTCLR.reg = CLOCK_BIT;
    }
}

void loop(){
  updateLEDs();
}
