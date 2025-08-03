#include <Arduino.h>
#include <data.h>
#include <TimerRoutines.h>
#include <LedGrid.h>

#define CLK_OUT_PIN 10
#define CLK_OUT_BIT 18 // PA18 ie. pin 10
// global data struct
Data data;
Data *dataPtr = &data;

// LED multiplexing
LedGrid ledGrid(dataPtr);

// timer routine class (handles setup and updating of timer)
TimerRoutines timerRoutines(dataPtr);
TimerRoutines *timerRoutinesPtr = &timerRoutines;

void TC4_Handler() {
  if (TC4->COUNT16.INTFLAG.bit.MC0) {
    TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;  // Clear interrupt

    // Do your clock pulse here
    if(data.globalQN % 24 == 0) {
      // pulse high
      PORT->Group[0].OUTSET.reg = (1 << CLK_OUT_BIT);
    }
    else if (data.globalQN % 24 == 12) {
      // pulse low
      PORT->Group[0].OUTCLR.reg = (1 << CLK_OUT_BIT);
    }

    data.globalQN ++;

    // if we have counted 96 (ie. enough for /4 main clock: 96 / 4 = 24)
    if(data.globalQN == 96) {
      data.globalQN = 0;
    }
  }
}

void TC5_Handler() {
  if (TC5->COUNT16.INTFLAG.bit.MC0) {
    TC5->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0; // Clear interrupt flag!!!!!

    // Do LED stuff
    ledGrid.update();
  }
}

void setup() {
  SerialUSB.begin(9600);
  while (!SerialUSB && millis() < 3000);  // avoid USB deadlock
  SerialUSB.println("SerialUSB ready");

  delay(1000);

  // configure pins
  pinMode(CLK_OUT_PIN, OUTPUT);
  pinMode(13, LED_BUILTIN);
  ledGrid.begin();

  timerRoutines.setupMidiTimer();
  timerRoutines.setupLedTimer(500); // 500Hz
}

void loop() {
  //ledGrid.update();
  SerialUSB.println("still alive");
  digitalWrite(13, HIGH);
  delay(300);
  digitalWrite(13, LOW);
}