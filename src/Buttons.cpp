#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
#include <Buttons.h>

#include <stdio.h>

#include <Events.h>
#include <EventQueue.h>
#include <EventDispatcher.h>

//mcp23017s
Adafruit_MCP23X17 mcpA;
Adafruit_MCP23X17 mcpB;

#define INT_PIN_A 3 //mcp A interrupt pin
#define INT_PIN_B 4 //mcp B interrupt pin
#define NUM_KEYS 16
#define KB_DEBOUNCE_COUNT 3

#define tempoDivButton 8 //16
#define resetButton 9 //for the timebeing this is used
#define swingButton 9 //17
#define transButton 10 //18
#define seqButton 11 // N/a

unsigned long playButtonDbTime = 0;
int prevPlayButtonState = 0;

int seqBtnDown = 0;
int tempoDivBtnDown = 0;
int transBtnDown = 0;

int encoderTurned = 0;

int beatChanged = 0;
int id;
int seqChanged = 0;

Buttons::Buttons(BeatData *d, EventQueue *q, EventDispatcher *dispatcher) {
  _d = d;
  _q = q;
  _dispatcher = dispatcher;
}

void Buttons::_keyRelease(BeatData *d, int num) {
  if(seqBtnDown) {
    d->activeSeq = num;
    Serial.print("switched active seq to ");
    Serial.println(num);
    seqChanged = 1;
    id = num;
  }
  else if(!encoderTurned) {
    //turn on or off our beat :)
    int b = !d->beatStates[d->activeSeq][num];
    if(b) {
      Serial.print("turned on beat no. ");
    }
    else { Serial.print("turned off beat no. "); }
    Serial.println(num);
    d->beatStates[d->activeSeq][num] = b;
    beatChanged = 1;
    id = num;
  }
}
void Buttons::_btnHandler(int event, int param, BeatData *d) {
  switch(event) {
        case Events::EV_KEY_PRESS:
          Serial.print("key pressed: ");
          Serial.println(param);
          //Serial.println(d->pitch[0][param]);
          break;

        case Events::EV_KEY_RELEASE:
          Serial.print("key released: ");
          Serial.println(param);
          _keyRelease(d, param);
          encoderTurned = 0;
          break;

        case Events::EV_ENCODER:
          encoderTurned = 1;
          break;
    }
}


void Buttons::_btnManagerA() {
  static unsigned long prevMillis = 0;
    unsigned long currMillis;
    int i;

    currMillis = millis();
    if (currMillis - prevMillis >= _dbDelay) {
        prevMillis = currMillis;
        i = mcpA.getLastInterruptPin();
            if (!mcpA.digitalRead(i)) {    // buttons are active low
                _btnOn[i]++;
                _btnOff[i] = 0;
            }
            else {
                _btnOn[i] = 0;
                _btnOff[i]++;
            }

            if (_btnOn[i] >= KB_DEBOUNCE_COUNT) {
                _btnOn[i] = 0;
                _btnOff[i] = 0;
                _currState[i] = 1;
                if (_prevState[i] == 0) {
                    // it was released and now it's pressed
                    _prevState[i] = 1;
                    _q->enqueueEvent(Events::EV_KEY_PRESS, i);
                }
            }

            if (_btnOff[i] >= 1) {
                _btnOn[i] = 0;
                _btnOff[i] = 0;
                _currState[i] = 0;
                if (_prevState[i] == 1) {
                    // it was pressed and now it's released
                    _prevState[i] = 0;
                    _q->enqueueEvent(Events::EV_KEY_RELEASE, i);
                }
            }

    }
}

void Buttons::begin() {
  //mcp
  if(!mcpA.begin_I2C(0x20)) {
    Serial.println("MCP #1 0x20 error.");
    while (1);
  }
  //config interrupts
  pinMode(INT_PIN_A, INPUT);
  mcpA.setupInterrupts(true, false, LOW);
  //config button matrix
  for (int i = 0; i < 16; i ++) {
    mcpA.pinMode(i, INPUT_PULLUP);
    mcpA.setupInterruptPin(i, LOW);
  }

  if(!mcpB.begin_I2C(0x21)) {
    Serial.println("MCP #2 0x21 error.");
    while (1);
  }

  mcpB.pinMode(seqButton, INPUT_PULLUP);
  mcpB.pinMode(tempoDivButton, INPUT_PULLUP);
  mcpB.pinMode(transButton, INPUT_PULLUP);
  mcpB.pinMode(resetButton, INPUT_PULLUP);

  _dispatcher->addEventListener(Events::EV_KEY_PRESS, _btnHandler);
  _dispatcher->addEventListener(Events::EV_KEY_RELEASE, _btnHandler);
  _dispatcher->addEventListener(Events::EV_ENCODER, _btnHandler);
}

void Buttons::read(unsigned long dt) {
  //increment playbutton debounce
  playButtonDbTime += dt;
  //check if our sequence was reset
  int playButtonState = mcpB.digitalRead(resetButton);
  if(playButtonDbTime >= _dbDelay && playButtonState != prevPlayButtonState) {
    if(playButtonState && startStop) {
      _d->stepNumber = 0;
      for(int i = 0; i < 8; i ++) {
        _d->stepCounter[i] = 0;
      }
      _d->playPause = 0;
      startStop = 0;
      Serial.println("PAUSE");
    }
    else if(playButtonState && !startStop) {
      _d->playPause = 1;
      startStop = 1;
      Serial.println("PLAY");
    }
    prevPlayButtonState = playButtonState;
    playButtonDbTime = 0;
  }

  //check if any of our mod buttons are down
  //sequence
  if(!mcpB.digitalRead(seqButton)) {
    seqBtnDown = 1;
  }
  else { seqBtnDown = 0; }

  // tempoDiv
  if(mcpB.digitalRead(tempoDivButton)) {
    if(!tempoDivBtnDown) {
      _q->enqueueEvent(Events::EV_KEY_PRESS, 16);
    }
    tempoDivBtnDown = 1;
  }
  else {
    if(tempoDivBtnDown) {
      _q->enqueueEvent(Events::EV_KEY_RELEASE, 16);
    }
    tempoDivBtnDown = 0;
  }

  //transpose
  if(mcpB.digitalRead(transButton)){
    if(!transBtnDown) {
      _q->enqueueEvent(Events::EV_KEY_PRESS, 18);
    }
    transBtnDown = 1;
  }
  else {
    if(transBtnDown) {
      _q->enqueueEvent(Events::EV_KEY_RELEASE, 18);
    }
    transBtnDown = 0;
  }

  // read interrupt pin for button matrix
  if(!digitalRead(INT_PIN_A)) {
    _btnManagerA();
  }
  // if a beat was changed fire event
  if(beatChanged) {
    _q->enqueueEvent(Events::EV_BEATCHANGED, id);
    beatChanged = 0;
  }
  // if a sequence was changed fire event
  if(seqChanged) {
    _q->enqueueEvent(Events::EV_SEQUENCECHANGED, id);
    seqChanged = 0;
  }

  _dispatcher->run(); // this should probably be run on main.cpp
}
