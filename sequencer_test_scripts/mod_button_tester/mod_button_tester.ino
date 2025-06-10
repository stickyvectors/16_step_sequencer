
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

void setup() {
  SerialUSB.begin(9600);

  if (!mcp.begin_I2C(0x20)) {
    SerialUSB.println("Error.");
    while (1);
  }
  // only first 8 pins are buttons
  for(int i = 0; i < 8; i++){
    // configure pin for input with pull up
    mcp.pinMode(i, INPUT_PULLUP); 
  }
}

void loop() {
  for(int i = 0; i < 8; i ++) {
    int state = mcp.digitalRead(i);

    if(!state) {
      SerialUSB.println("Button " + String(i) + " Pressed!");
    }
  }

}
