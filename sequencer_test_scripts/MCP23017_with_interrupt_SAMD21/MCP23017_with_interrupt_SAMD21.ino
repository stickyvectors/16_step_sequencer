#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>

#define INT_PIN 12      // microcontroller pin attached to INTA/B

volatile boolean awakenByInterrupt = false;

Adafruit_MCP23X17 mcp;

void setup() {
  SerialUSB.begin(9600);
  //while (!Serial);
  SerialUSB.println("MCP23xxx Interrupt Test!");

  // uncomment appropriate mcp.begin
  if (!mcp.begin_I2C(0x20)) {
  //if (!mcp.begin_SPI(CS_PIN)) {
    SerialUSB.println("Error.");
    while (1);
  }

  // configure MCU pin that will read INTA/B state
  pinMode(INT_PIN, INPUT);

  mcp.setupInterrupts(true, false, FALLING);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), handleInterrupt, FALLING);

  // configure pins for input with pull up
  for(int i = 0; i < 16; i ++) {
    mcp.pinMode(i, INPUT_PULLUP);
    mcp.setupInterruptPin(i, LOW);
  }

  // clear interrupts to allow pin back HIGH from previous program run
  mcp.clearInterrupts(); 

  SerialUSB.println("Looping...");
}

void loop() {
  if(awakenByInterrupt) {
    awakenByInterrupt = false;
    SerialUSB.println("INT CALLBACK");
    SerialUSB.print("Interrupt detected on pin: ");
    SerialUSB.println(mcp.getLastInterruptPin());
    mcp.clearInterrupts();
  }
  delay(1);
}

void handleInterrupt(){
  awakenByInterrupt=true;
}
