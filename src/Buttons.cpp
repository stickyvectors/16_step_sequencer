#include "Arduino.h"
#include "Buttons.h"

#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

#define seqButton 13
#define swingButton 0
#define transButton 1

Buttons::Buttons() {
  //
}

void Buttons::begin(byte (*beatStatesPointer)[16]) {
  Serial.println("MCP23017 buttons running");
  if(!mcp.begin_I2C()) {
    Serial.println("Error.");
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
}

void Buttons::read(byte (*beatStatesPointer)[16]) {
  //cycle through all buttons
  for (int i = 0; i < 16; i++) {
    int r = mcp.digitalRead(i);
    //if we get a new button state before debounce, reset
    if(r != _lastButtonState[i]) {
      _lastdbTime[i] = millis();
    }
    //if our debounce time has passed an it's a new state:
    //it's a genuine press
    if((millis() - _lastdbTime[i]) > _dbDelay) {
      if(r != _buttonState[i]) {
        _buttonState[i] = r;
        //if our SEQ button is also down, switch seq instead
        if(digitalRead(seqButton) == LOW){
          if(_buttonState[i] == LOW) {
            if(i < 5) {
              _activeSeq = i;
              //reset LEDs
            }
          }
          continue;
        }
        //otherwise do the usual:
        //button down
        else if (_buttonState[i] == LOW) {
          //button held
          _buttonHold[i] = 1;
          // we can read pitch knob
        }
        //button up
        else if (_buttonState[i] == HIGH) {
          //if we changed our pitch while the button was held
          //then don't light the LED or change beat state
          //if we didn't change the pitch then turn on beat
          *(beatStatesPointer[_activeSeq] + i) = _buttonState[i];
          //finally set _buttonHold LOW
          _buttonHold[i] = 0;
        }
      }
    }
    _lastButtonState[i] = r;
  }
}
