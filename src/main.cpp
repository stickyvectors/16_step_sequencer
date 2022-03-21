#include <Arduino.h>
#include <LedMatrix.h>
#include <Display.h>
#include <Buttons.h>

int activeSeq = 0;   // for LEDs
int currentBeat = 0;  // for LEDs
int stepNumber = 0;   // tick tock for clock

//expressed as a pointer:
//beatStates[y][x] = *(beatStatesPointer[y] + x);
byte beatStates[5][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

byte (*beatStatesPointer)[16] = beatStates;

unsigned long tempo = 200;
unsigned long tempoTimer = 0; // tempo timer should reset every 1/32
int tempoDiv[5] = {2, 2, 2, 2, 2};
float swing[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
unsigned long beatTimers[5] = {0, 0, 0, 0, 0};
int stepCounters[5] = {0, 0, 0, 0, 0};

unsigned long dt = 0;
unsigned long lastMs = 0;

//LEDs
LedMatrix leds;
//Display
Display screen;
//Buttons
Buttons buttons;

void setup() {
  Serial.begin(9600);
  Serial.println("~ WELCOME TO CHRIS'S SEQUENCER ~");
  screen.begin();
  buttons.begin(beatStatesPointer);
}

void loop() {
  //read buttons
  //this now updates via pointers in buttons class
  buttons.read(beatStatesPointer);
  //get updated beat states
  //THIS IS A REALLY BAD WAY TO DO THIS!!!!!
  //for(int p = 0; p < 5; p ++) {
  //  for(int q = 0; q < 16; q ++) {
  //    beatStates[p][q] = buttons.beatStates[p][q];
  //  }
  //}
  dt = millis() - lastMs;
  tempoTimer += dt;
  lastMs = millis();
  //update beat timers
  for (int i = 0; i < 5; i ++) {
    beatTimers[i] += dt;
  }
  //fire clock
  if(tempoTimer > tempo) {
    tempoTimer = 0;
    if(stepNumber == 0) {
      //fire clock
      stepNumber = 0;
    }
    else { stepNumber = 1; }
  }
  //fire beats
  if(tempoTimer == 0){
    //Serial.println(String(beatTimers[0]) + " | " + String(tempoDiv[0] * tempo));
    // go through all sequences and fire beats
    for (int s = 0; s < 5; s ++) {
      if (beatTimers[s] > tempoDiv[s] * tempo) {
        // if beat of sequence is odd wait beatTime[x] * swing[x] before
        // firing beat
        if(swing[s] == 0) {
          //just fire beat
          beatTimers[s] = 0;
          if(s == activeSeq) {
            leds.step(beatStates[activeSeq], int(stepCounters[activeSeq]));
          }
        }
        else {
          if(stepCounters[s] % 2 == 0) {
            //it's even: fire the beat immediately
            beatTimers[s] = 0;
            if(s == activeSeq) {
              leds.step(beatStates[activeSeq], int(stepCounters[activeSeq]));
            }
          }
          else {
            //it's odd: wait swing[x] amount of time
            if(beatTimers[s] > tempoDiv[s] * tempo + tempoDiv[s] * tempo * swing[s]) {
              //we've reached desired swing, fire the beat
              beatTimers[s] = 0;
              if(s == activeSeq) {
                leds.step(beatStates[activeSeq], int(stepCounters[activeSeq]));
              }
            }
            //else { continue; }
          }
        }
        stepCounters[s] += 1;
        if(stepCounters[s] == 16) { stepCounters[s] = 0; }
      }
      else {
        //we aren't ready to fire this beat: do nothing
      }
    }
  }
  //update leds
  leds.update();
  //
} // end loop()
