// Reads a button attached to a MCP23XXX pin.

#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

bool buttonState[8] = {1,1,1,1,
                       1,1,1,1};

bool triggerState[8] = {0,0,0,0,
                        0,0,0,0};

unsigned long triggerTimeout;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long deltaTime = 0;

void setup() {
  SerialUSB.begin(9600);
  //while (!Serial);
  SerialUSB.println("MCP23xxx Button Test!");

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C(0x26)) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    SerialUSB.println("Error.");
    while (1);
  }

  for(int t = 0; t < 8; t ++) {
    // configure first 8 pins for trigger outputs
    mcp.pinMode(t, OUTPUT);
  }
  for(int q = 8; q < 16; q++){
    // configure last 8 pins for input with pull up
    mcp.pinMode(q, INPUT_PULLUP); 
  }

  SerialUSB.println("Looping...");
}

void loop() {
  // Get the current time
  currentMillis = millis();

  // Calculate delta time in ms
  deltaTime = (currentMillis - previousMillis);

  // Update the previous time for the next loop
  previousMillis = currentMillis;

  triggerTimeout += deltaTime;

  // check all buttons
  readButtons();

  if(triggerTimeout > 1000) {
    
    // set all outputs high, then low
    for(int i = 0; i < 8; i ++) {
      triggerState[i] = !triggerState[i];
      mcp.digitalWrite(i, triggerState[i]);
    }
    triggerTimeout = 0;
  }
  
}

void updateTriggers() {

}

void readButtons() {
  for(int i = 8; i < 16; i ++) 
  {
    // read a button
    int state = mcp.digitalRead(i);
    
    // LOW = pressed, HIGH = not pressed
    // button is pressed if true
    if (!state && state != buttonState[i]) 
    {
      // flip current button state and led state
      buttonState[i] = !buttonState[i];

      Serial.println("Button " + String(i) + " Pressed! ");

      // lazy debouncing...
      delay(25);
    }
    // button is up
    else if (state && state != buttonState[i]) {
      buttonState[i] = !buttonState[i];  
    }
  }
}
