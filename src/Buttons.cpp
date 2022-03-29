#include "Arduino.h"
#include "Buttons.h"

#include <Adafruit_MCP23X17.h>
#include "LedMatrix.h"
#include "Display.h"
#include "Encoder.h"

Adafruit_MCP23X17 mcp;
Encoder enc;

#define seqButton 13
#define swingButton 0
#define transButton 1

int buttonHeld = -1;

Buttons::Buttons() {
//
}

void Buttons::begin(int (*pBeatStates)[16], LedMatrix *ledsPointer, Display* pScreen, int* pPitch, int* pPitchChange) {
  _pBeatStates = pBeatStates;
  _pPitch = pPitch;
  _pPitchChange = pPitchChange;
  _pScreen = pScreen;
  //pass leds pointer to private variable
  _ledsPointer = ledsPointer;
  //begin I2C connection
  //Serial.println("MCP23017 buttons running");
  if(!mcp.begin_I2C()) {
    Serial.println("Button error.");
    while (1);
  }
  for (int i = 0; i < 16; i ++) {
    //config pin for input pull up
    mcp.pinMode(i, INPUT_PULLUP);
  }
  //set up mod buttons (eventually this will be another MCP23017)
  pinMode(seqButton, INPUT);
  pinMode(swingButton, INPUT);
  pinMode(transButton, INPUT);
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
    else { buttonHeld = -1; }
  }
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
              _activeSeq = i;
              //reset LEDs
            }
          }
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
            int v = !_pBeatStates[_activeSeq][i];
            _pBeatStates[_activeSeq][i] = v;
            (*_ledsPointer).switchState(i, _pBeatStates[_activeSeq][i]);
            //finally set _buttonHold LOW
            _buttonHold[i] = 0;
          }
        }
      }
    }
    _lastButtonState[i] = r;

  }
}
