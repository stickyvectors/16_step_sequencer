#include <Arduino.h>
#include <LedMatrix.h>
#include <Display.h>
#include <Buttons.h>

int activeSeq = 0;   // for LEDs
int currentBeat = 0;  // for LEDs
int stepNumber = 0;   // tick tock for clock

//beat states
int beatStates[5][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
//pointer
int (*pBeatStates)[16];

//pitches
int pitch[16] = {792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792};
int changedPitch[16] = {0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int* pPitch = pitch;
int* pPitchChange = changedPitch;

unsigned long tempo = 100;
unsigned long tempoTimer = 0; // tempo timer should reset every 1/32
int tempoDiv[5] = {2, 2, 2, 2, 2};
float swing[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
unsigned long beatTimers[5] = {0, 0, 0, 0, 0};
int stepCounters[5] = {0, 0, 0, 0, 0};

unsigned long dt = 0;
unsigned long bt = 0;
unsigned long lastMs = 0;

//LEDs
LedMatrix leds;
LedMatrix *ledsPointer = &leds;
//Display
Display screen;
Display *pScreen = &screen;
//Buttons
Buttons buttons;

void setup() {
  //assign beat states and pitch pointers
  pBeatStates = beatStates;
  pPitch = pitch;
  //Serial begin
  Serial.begin(9600);
  Serial.println("~ WELCOME TO CHRIS'S SEQUENCER ~");
  //Display
  screen.begin();
  //Buttons
  buttons.begin(pBeatStates, ledsPointer, pScreen, pPitch, pPitchChange);

}

void loop() {
  //time stuff
  dt = millis() - lastMs;
  tempoTimer += dt;
  lastMs = millis();
  //read buttons
  //this now updates via pointers in buttons class
  //but it causes us all kinds of problems with LED brightness
  buttons.read(dt);
  //update beat timers
  for (int i = 0; i < 5; i ++) {
    //update leds
    leds.update();
    //
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
            leds.step(pBeatStates, activeSeq, int(stepCounters[activeSeq]));
          }
        }
        else {
          if(stepCounters[s] % 2 == 0) {
            //it's even: fire the beat immediately
            beatTimers[s] = 0;
            if(s == activeSeq) {
              leds.step(pBeatStates, activeSeq, int(stepCounters[activeSeq]));
            }
          }
          else {
            //it's odd: wait swing[x] amount of time
            if(beatTimers[s] > tempoDiv[s] * tempo + tempoDiv[s] * tempo * swing[s]) {
              //we've reached desired swing, fire the beat
              beatTimers[s] = 0;
              if(s == activeSeq) {
                leds.step(pBeatStates, activeSeq, int(stepCounters[activeSeq]));
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
  //
} // end loop()
