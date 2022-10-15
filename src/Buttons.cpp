#include "Arduino.h"
#include "Buttons.h"

#include <Adafruit_MCP23X17.h>
#include "LedMatrix.h"
#include "Encoder.h"

Adafruit_MCP23X17 mcp;
Adafruit_MCP23X17 mcp2;

#define INT_PIN 3
#define tempoDivButton 8
#define swingButton 9
#define transButton 10
#define seqButton 11

int buttonHeld = -1;
int homeScreen = true;

Buttons::Buttons(BeatData *d, LedMatrix *ledsPointer, Display* pScreen) {
  _dPtr = d;
  //pass leds pointer to private variable
  _ledsPointer = ledsPointer;
}

void Buttons::begin() {

  Encoder enc(_dPtr);
  _enc = &enc;

  if(!mcp.begin_I2C(0x20)) {
    Serial.println("MCP #1 0x20 error.");
    while (1);
  }

  if(!mcp2.begin_I2C(0x21)) {
    Serial.println("MCP #2 0x21 error.");
    while (1);
  }

  //configure interrupts
  pinMode(INT_PIN, INPUT);
  mcp.setupInterrupts(true, false, LOW);
  //configure button matrix
  for (int i = 0; i < 16; i ++) {
    //config pin for input pull up
    mcp.pinMode(i, INPUT_PULLUP);
    //attach interrupt
    mcp.setupInterruptPin(i, LOW);
  }
  //set up mod buttons
  pinMode(seqButton, INPUT);
  //these on mcp #2
  mcp2.pinMode(seqButton, INPUT_PULLUP);
  mcp2.pinMode(tempoDivButton, INPUT_PULLUP);
  mcp2.pinMode(swingButton, INPUT_PULLUP);
  mcp2.pinMode(transButton, INPUT_PULLUP);
  //gates on mcp2
  for(int i = 0; i < 6; i ++) {
      mcp2.pinMode(i, OUTPUT);
  }
  //start encoder
  _enc->begin();
}
//__attribute__((optimize("O0")))
void Buttons::read(unsigned long dt) {
  //if we received an interrupt:
  if(!digitalRead(INT_PIN)) {
    int btn = mcp.getLastInterruptPin();
    int r = mcp.digitalRead(btn);
    //debounce
    //if we get new state before debounce timeout (and we aren't just holding it down)
    //reset timeout
    if(r != _lastButtonState[btn] && _buttonHold[btn] == 0) {
      _lastdbTime[btn] = 0;
    }
    //otherwise, increment timer
    else { _lastdbTime[btn] += dt; _lastButtonState[btn] = r; }

    //if our debounce time has passed and it's a new state,
    //then it's a genuine press
    if((_lastdbTime[btn]) > _dbDelay || buttonHeld == btn) {
      //just a press
      //if(r != _buttonState[btn]) {
        //if SEQ button held, switch
        if(mcp2.digitalRead(seqButton) == LOW) {
          if(btn < 8) {
            if(_dPtr->activeSeq != btn) {
              _dPtr->activeSeq = btn;
              Serial.print("seq ");
              Serial.println(btn);
              _ledsPointer->switchSequence();
            }
            return;
          }
        };
        //if button LOW, then it's held
        if(r == LOW) {
          _buttonHold[btn] = 1;
          buttonHeld = btn;
          //
        }
        //if button HIGH, then it's been released
        else if (r == HIGH) {
          _buttonHold[btn] = 0;
          //if we changed our pitch while button was down
          //then don't light the LED or change beat state
          if(_dPtr->changedPitch[_dPtr->activeSeq][btn] == 1) {
            //return display to home screen
            //_pScreen->home();
            _dPtr->changedPitch[_dPtr->activeSeq][btn] = 0;

          }
          else {
            //we didn't change the pitch so turn on beat
            int v = !_dPtr->beatStates[_dPtr->activeSeq][btn];
            _dPtr->beatStates[_dPtr->activeSeq][btn] = v;
            _ledsPointer->switchState(btn, v);
          }
          _buttonHold[btn] = 0;
          buttonHeld = -1;
        }
      //}
    }
    _lastButtonState[btn] = r;
  }
  //if we already have a button held down do other stuff
  if(buttonHeld >= 0) {
    //update encoder
    _enc->tick();
    //read enc data and assign to held beat
    _enc->readPitch(buttonHeld);
    if(_dPtr->changedPitch[_dPtr->activeSeq][buttonHeld] == 1) {
      //_pScreen->pitch(_dPtr->pitch[_dPtr->activeSeq][buttonHeld]);
    }
    return;
    //do we need to check for a button release or will that be picked up by a second interrupt?
  }
  //if our tempo div button is down
  if(mcp2.digitalRead(tempoDivButton) == HIGH) {
    //update encoder
    _enc->tick();
    int dir = _enc->getDirection();
    //change division according to direction
    if(dir != 0) {
      _dPtr->tempoDiv[_dPtr->activeSeq] += dir;
    }
    return;
  }
  //if our swing button is down
  else if(mcp2.digitalRead(swingButton) == HIGH) {
    //update encoder
    _enc->tick();
    int dir = _enc->getDirection();
    //change division according to direction
    if(dir != 0) {
      float s = _dPtr->swing[_dPtr->activeSeq] + (dir * 0.01f);
      _dPtr->swing[_dPtr->activeSeq] = s;
      Serial.print("swing ");
      Serial.println(s);
    }
    return;
  }
  //no function buttons used (this is a bit lazy) return display to default

  //cycle through all buttons
    //if we get a new button state before debounce, reset

      //otherwise, increment timer

    //if our debounce time has passed and it's a new state:
    //it's a genuine press
      //if the state changed OR our button was already held do stuff
        //if our SEQ button is also down, switch seq instead
          //tell LEDs to switch sequence
          //we can skip the rest

        //otherwise do the usual:
        //if button down
          //button held
          // we can read pitch knob

        //if button up
          //if we changed our pitch while the button was held
          //then don't light the LED or change beat state
            //if we didn't change the pitch then turn on beat
            //finally set _buttonHold LOW

    //updateL _lastButtonState[i]
}

void Buttons::fireGate(int gate) {
  mcp2.digitalWrite(gate, HIGH);
}

void Buttons::closeGate(int gate) {
  mcp2.digitalWrite(gate, LOW);
}
