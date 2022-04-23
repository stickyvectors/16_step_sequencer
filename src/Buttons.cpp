#include "Arduino.h"
#include "Buttons.h"

#include <Adafruit_MCP23X17.h>
#include "LedMatrix.h"
#include "Display.h"
#include "Encoder.h"

Adafruit_MCP23X17 mcp;
Adafruit_MCP23X17 mcp2;
Encoder enc;

#define seqButton 13
#define tempoDivButton 0
#define swingButton 1
#define transButton 2

int buttonHeld = -1;
int homeScreen = true;

Buttons::Buttons() {
//
}

void Buttons::begin(int (*pBeatStates)[16], int* pSeq, LedMatrix *ledsPointer, Display* pScreen, int* pPitch, int* pPitchChange, int* pDiv, float* pSwing) {
  //assign pointers
  _pBeatStates = pBeatStates;
  _pSeq = pSeq;
  _pPitch = pPitch;
  _pPitchChange = pPitchChange;
  _pScreen = pScreen;
  _pDiv = pDiv;
  _pSwing = pSwing;
  //pass leds pointer to private variable
  _ledsPointer = ledsPointer;
  //begin I2C connection
  //Serial.println("MCP23017 buttons running");
  if(!mcp.begin_I2C(0x20)) {
    Serial.println("MCP #1 0x20 error.");
    while (1);
  }
  if(!mcp2.begin_I2C(0x21)) {
    Serial.println("MCP #2 0x21 error.");
    while (1);
  }
  //configure button matrix
  for (int i = 0; i < 16; i ++) {
    //config pin for input pull up
    mcp.pinMode(i, INPUT_PULLUP);
  }
  //set up mod buttons
  pinMode(seqButton, INPUT);
  //these on mcp #2
  mcp2.pinMode(tempoDivButton, INPUT_PULLUP);
  mcp2.pinMode(swingButton, INPUT_PULLUP);
  mcp2.pinMode(transButton, INPUT_PULLUP);
  //start encoder
  enc.begin(pPitch, _pPitchChange);
}
//__attribute__((optimize("O0")))
void Buttons::read(unsigned long dt) {
  //if a button was already held, just read pitch
  if(buttonHeld >= 0) {
    //update encoder
    enc.tick();
    enc.readPitch(buttonHeld);
    if(_pPitchChange[buttonHeld] == 1) {
      (*_pScreen).pitch(_pPitch[buttonHeld]);
    }
    //also check for a button release
    int r = mcp.digitalRead(buttonHeld);
    if(r == LOW) {
      //just return
      return;
    }
    //we released the button, start reading buttons again
    else { buttonHeld = -1; }
  }
  //if our tempo div button is down
  else if (mcp2.digitalRead(tempoDivButton) == HIGH) {
    //update encoder
    enc.tick();
    //change division according to direction
    int div = _pDiv[*_pSeq] - enc.getDirection();
    if(div > 0) {
      _pDiv[*_pSeq] = div;
      (*_pScreen).div(div);
    }
    return;
  }
  //if our swing button is down
  else if (mcp2.digitalRead(swingButton) == HIGH) {
    //update encoder
    enc.tick();
    //change division according to direction
    float swing = _pSwing[*_pSeq] + (enc.getDirection() * 0.01f);
    if(swing > 0.0f) {
      _pSwing[*_pSeq] = swing;
      (*_pScreen).swing(swing);
    }
    homeScreen = false;
    return;
  }
  //no function buttons used (this is a bit lazy)
  else if (!homeScreen) { (*_pScreen).home(); homeScreen = true; }

  //cycle through all buttons
  for (int i = 0; i < 16; i++) {
    int r = mcp.digitalRead(i);
    //if we get a new button state before debounce, reset
    if(r != _lastButtonState[i] && _buttonHold[i] == 0) {
      _lastdbTime[i] = 0;
    }
    else { _lastdbTime[i] += dt; _lastButtonState[i] = r; } //increment timer
    //if our debounce time has passed and it's a new state:
    //it's a genuine press
    if((_lastdbTime[i]) > _dbDelay) {
      //if the state changed OR our button was already held do stuff
      if(r != _buttonState[i] || _buttonHold[i] == 1) {
        _buttonState[i] = r;
        //if our SEQ button is also down, switch seq instead
        if(digitalRead(seqButton) == LOW){
          if(_buttonState[i] == LOW) {
            if(i < 5) {
              (*_pScreen).seq(i);
              *_pSeq = i;
              //reset LEDs
              (*_ledsPointer).switchSequence(_pBeatStates, *_pSeq);
            }
          }
          //we can skip the rest
          return;
        }
        //otherwise do the usual:
        //button down
        else if (_buttonState[i] == LOW) {
          //button held
          _buttonHold[i] = 1;
          buttonHeld = i;
          // we can read pitch knob
        }
        //button up
        else if (_buttonState[i] == HIGH) {
          //if we changed our pitch while the button was held
          //then don't light the LED or change beat state
          if(_pPitchChange[i] == 1) {
            (*_pScreen).home();
            _pPitchChange[i] = 0;
            _buttonHold[i] = 0;
          }
          else {
            //if we didn't change the pitch then turn on beat
            int v = !_pBeatStates[*_pSeq][i];
            _pBeatStates[*_pSeq][i] = v;
            (*_ledsPointer).switchState(i, _pBeatStates[*_pSeq][i]);
            //finally set _buttonHold LOW
            _buttonHold[i] = 0;
          }
        }
      }
    }
    _lastButtonState[i] = r;
    //update our leds
    (*_ledsPointer).update();
  }
}
