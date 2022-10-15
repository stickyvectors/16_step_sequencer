#include <Arduino.h>
#include <LedMatrix.h>
#include <Display.h>
#include <Buttons.h>
#include <DAC.h>
#include <BeatData.h>

#define clkPin 12
unsigned long tempo = 100;
unsigned long tempoTimer = 0; // tempo timer should reset every 1/32

unsigned long dt = 0;
unsigned long bt = 0;
unsigned long lastMs = 0;
//DATA
BeatData bData;
BeatData *beatData = &bData;
//LEDs
LedMatrix leds(beatData);
LedMatrix *pLEDS = &leds;
//Display
Display screen;
Display *pScreen = &screen;
//Buttons
Buttons buttons(beatData, pLEDS, pScreen);
//DACs
DAC dacA(beatData, 0, 0x61);
DAC dacB(beatData, 1, 0x60);

void setup() {
  //Serial begin
  Serial.begin(9600);
  Serial.println("~ WELCOME TO CHRIS'S SEQUENCER ~");
  //Display
  screen.begin();
  //Buttons
  buttons.begin();
  //DACs
  dacA.begin();
  dacB.begin();
  //clock
  pinMode(clkPin, OUTPUT);
}

void loop() {
  //time stuff
  dt = millis() - lastMs;
  tempoTimer += dt; //increment timer
  lastMs = millis();
  //read buttons
  buttons.read(dt);
  for (int i = 0; i < 5; i ++) {
    //update leds
    leds.update();
    // update beat timers
    beatData->beatTimer[i] += dt;
  }
  //fire clock
  if(tempoTimer > tempo) {
    tempoTimer = 0;
    if(beatData->stepNumber == 0) {
      //fire clock
      beatData->stepNumber = 0;
      digitalWrite(clkPin, HIGH);
    }
    else { beatData->stepNumber = 1; digitalWrite(clkPin, LOW); }
  }
  //fire beats
  if(tempoTimer == 0){
    // go through all sequences and fire beats
    for (int s = 0; s < 5; s ++) {
      //see if we need to close a gate
      if (beatData->beatTimer[s] > (beatData->tempoDiv[s] * tempo / 2)) {
        if(beatData->beatStates[s][beatData->stepCounter[s]] == 1) {
          buttons.closeGate(s);
        }
      }
      //see if we need to open a gate
      if (beatData->beatTimer[s] > beatData->tempoDiv[s] * tempo) {
        // check if there is an active beat on this step
          // no swing, fire beat
          if(beatData->swing[s] == 0) {
            if(s == beatData->activeSeq) {
              leds.step();
            }
            if(beatData->beatStates[s][beatData->stepCounter[s]] == 1) {
              //only fire dac for first and second sequence
              if(s == 0) {
                dacA.updatePitch(beatData->stepCounter[s]);
              }
              if(s == 1) {
                dacB.updatePitch(beatData->stepCounter[s]);
              }
              //just fire beat
              buttons.fireGate(s);
            }
            beatData->beatTimer[s] = 0;

          }
          // there is swing, calculate it!
          else {
            if(beatData->stepCounter[s] % 2 == 0) {

              if(s == beatData->activeSeq) {
                leds.step();
              }
              if(beatData->beatStates[s][beatData->stepCounter[s]] == 1) {
                //only fire dac for first sequence
                if(s == 0) {
                  dacA.updatePitch(beatData->stepCounter[s]);
                }
                if(s == 1) {
                  dacB.updatePitch(beatData->stepCounter[s]);
                }
                //it's even: fire the beat immediately
                buttons.fireGate(s);

              }
              beatData->beatTimer[s] = 0;

            }
            else {
              //it's odd: wait swing[x] amount of time
              if(beatData->beatTimer[s] > beatData->tempoDiv[s] * tempo + beatData->tempoDiv[s] * tempo * beatData->swing[s]) {

                if(s == beatData->activeSeq) {
                  leds.step();
                }
                if(beatData->beatStates[s][beatData->stepCounter[s]] == 1) {
                  //only fire dac for first sequence
                  if(s == 0) {
                    dacA.updatePitch(beatData->stepCounter[s]);
                  }
                  if(s == 1) {
                    dacB.updatePitch(beatData->stepCounter[s]);
                  }
                  //we've reached desired swing, fire the beat
                  buttons.fireGate(s);
                }
                beatData->beatTimer[s] = 0;

              }
              else { continue; }
            }
          } // end of swing check
        //increment our counter regardless
        beatData->stepCounter[s] += 1;
        if(beatData->stepCounter[s] == 16) { beatData->stepCounter[s] = 0; }
      } // end of beat check
      else {
        //we aren't ready to fire this beat: do nothing
      }
    }
  }
  //
} // end loop()
