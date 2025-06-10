// 0, 1, 2 for Seeeduino / 5, 4, 3 for SAMD21 breakout
#define latchPin 5
#define clockPin 4
#define dataPin  3

// first 4 bits are rows, second 4 bits are columns, so r1, r2, r3, r4, c1, c2, c3, c4
//byte ledData[4] = {0b01001011, 0b00001111, 0b00001111, 0b00001111};

// LED states per row (4-bit nibbles, each representing on/off for respective LED in a row)
byte ledData[4] = {0b0000, 0b0000, 0b0000, 0b0000};



void setup() {
  SerialUSB.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  SerialUSB.println("Looping...");
}

void loop() {

  // update row by row
  for(byte r = 0; r < 4; r ++) 
  {
    /* 
    as decimal numbers if we start at 8 and divide by 2 (so 8, 4, 2, 1),
    this is equivalent to 1000, 0100, 0010, 0001 which gives a stepping
    through effect on each row
    */
    byte h = 8;

    // count rows backwards because my schematic was reversed!
    byte row = (0b0001 << 4 + 3 - r);

    for(byte l = 0; l < 4; l ++) {
    
      // rows | colums
      byte data = row | (15 & ~h);

      // step through leds in row
      h = h / 2;

      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, data);
      digitalWrite(latchPin, HIGH);

      delay(250);
    }
  } 
}
