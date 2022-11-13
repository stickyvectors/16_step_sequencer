#include <Arduino.h>
#include <Events.h>
#include <EventQueue.h>
#include <EventDispatcher.h>
#include <DAC.h>
#include <Buttons.h>
#include <Encoder.h>
#include <BeatData.h>
#include <Gates.h>
#include <LedMatrix.h>
#include <Adafruit_MCP4725.h>

BeatData bData;
BeatData* dPtr = &bData;

EventQueue q;
EventQueue *qPtr = &q;
EventDispatcher dispatcher(dPtr, qPtr);
EventDispatcher *dispatcherPtr = &dispatcher;

#define tempoKnob A0
#define clkPin 12
int tempo = 200;
int tempoTimer = 0; //tempo resets every 1/64
int tempoDb = 0;
int tempoDbtime = 50;
int tSample = 0;
int tEstimate = 0;
int tickTock = 0;
//delta time
unsigned long dt = 0;
unsigned long lastDT = 0;
//buttons
Buttons buttons(dPtr, qPtr, dispatcherPtr);
Encoder encoder(dPtr, qPtr, dispatcherPtr);
//LED matrix
LedMatrix leds(dPtr, qPtr, dispatcherPtr);
LedMatrix * lPtr = &leds;
//DACs
Adafruit_MCP4725 dA;
Adafruit_MCP4725 dB;
DAC dacA(dPtr, dA, 0, 0x61);
DAC dacB(dPtr, dB, 1, 0x60);
DAC * aPtr = &dacA;
DAC * bPtr = &dacB;
//gates
Gates gates(dPtr, aPtr, bPtr, lPtr);


//testing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 25;
bool switchNote = false;
int buttonState;
int lastButtonState = 1;

void setup() {
  Serial.begin(9600);

  buttons.begin();
  encoder.begin();
  dacA.begin();
  dacB.begin();
  gates.begin();
  pinMode(clkPin, OUTPUT);

  tSample = analogRead(tempoKnob)/4.3;
  tEstimate = tSample;
  Serial.println("~WELCOME TO SEQ_MKII~");
}

void loop() {
  //calculate delta time
  dt = millis() - lastDT;
  tempoTimer += dt;
  lastDT = millis();
  //read buttons and encoder
  buttons.read(dt);
  encoder.read();

  //update leds
  leds.update();

  //count steps every 1/64, fire tempo every 1/16
  if(tempoTimer > tempo) {
    tempoTimer = 0;
    //check for beats we need to fire (LEDs are updated inside gates)
    gates.update(dt);
    //clock HIGH
    if(dPtr->stepNumber % 4 == 0) {
      digitalWrite(clkPin, HIGH);
    }
    else if(dPtr->stepNumber % 4 == 1) {
      digitalWrite(clkPin, LOW);
    }
    //increment our global step number (1/64)
    dPtr->stepNumber += 1;
    if(dPtr->stepNumber == 1024) {
      dPtr->stepNumber = 0; //reset back to beginning after this many 1024 steps
    }
  }

  tempoDb += dt;
  if(tempoDb > tempoDbtime) {
    //update tempo, if it got changed
    //exponential smoothing
    tSample = analogRead(tempoKnob)/4.3;
    tEstimate += (tSample - tEstimate) >> 3;

    if(tEstimate != tempo) {
      tempo = tEstimate;
      Serial.println(tempo);
    }
    //unsigned int tRead = analogRead(tempoKnob)/4.3;
    //if (tRead > tempo * 1.1 || tRead < tempo * 0.9) {
    //  tempo = tRead;
    //}
    tempoDb = 0;
  }

}
