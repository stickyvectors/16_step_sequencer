// Reads a button attached to a MCP23XXX pin.

#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

#define latchPin 5//2
#define clockPin 4//3
#define dataPin  3//4

byte dataToSend;
byte ledData[4] = {0b0000,0b0000,0b0000,0b0000}; // all off

bool buttonState[16] = {1,1,1,1,
                       1,1,1,1,
                       1,1,1,1,
                       1,1,1,1};

// this could actually be a single 16 bit byte!
bool ledState[16] = {0,0,0,0,
                    0,0,0,0,
                    0,0,0,0,
                    0,0,0,0};

void setup() {
  SerialUSB.begin(9600);
  //while (!Serial);
  SerialUSB.println("MCP23xxx Button Test!");

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C(0x20)) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    SerialUSB.println("Error.");
    while (1);
  }
  for(int i = 0; i < 16; i++){
    // configure pin for input with pull up
    mcp.pinMode(i, INPUT_PULLUP); 
  }

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  SerialUSB.println("Looping...");
}

void loop() {
  // reset row counter
  int row = 0;

  // check all buttons
  for(int i = 0; i < 16; i ++) 
  {
    // read a button
    int state = mcp.digitalRead(i);

    // LOW = pressed, HIGH = not pressed
    // button is pressed if true
    if (!state && state != buttonState[i]) 
    {
      // flip current button state and led state
      buttonState[i] = !buttonState[i];
      ledState[i] = !ledState[i];

      int currentRow = i / 4;
      int currentColumn = 3 - (i % 4); // bitwrite indexes from LSB (right-most bit) so we have to invert the col value

      //SerialUSB.println("writing a " + String(ledState[i]) + " to LED " + String(i));
      // write bit to led data
      bitWrite(ledData[currentRow], currentColumn, ledState[i]);

      //SerialUSB.println("row = " + String(currentRow) + " LED no. = " + (i));
      //SerialUSB.println(ledData[row]);

      //SerialUSB.println("Button " + String(i) + " Pressed! ");

      // lazy debouncing...
      delay(25);
    }
    // button is up
    else if (state && state != buttonState[i]) {
      buttonState[i] = !buttonState[i];  
    }

    // shift out LED data only 4x during iterating (coz only 4 rows)
    if((i + 1) % 4 == 0) {
      int currentRow = i / 4;
      
      // rows | columns
      dataToSend = (0b0001 << (4 + 3 - currentRow)) | (15 & ~ledData[row]);

      sendData(dataToSend);

      //SerialUSB.print("data sent to row " + String(row) + ": ");
      //for(int q = 0; q < 4; q ++)      {
      //   
      //  SerialUSB.print(bitRead((15 & ledData[row]), q));
      //}
      //SerialUSB.println("");

      // move to next row
      row += 1;
    }
  }
}

void sendData(byte data) {
  digitalWrite(latchPin, LOW);
  
  shiftOut(dataPin, clockPin, LSBFIRST, data);

  digitalWrite(latchPin, HIGH);
}
