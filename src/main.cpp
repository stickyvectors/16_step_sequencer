#include <Arduino.h>
#include <LedMatrix.h>
#include <Display.h>
#include <Buttons.h>
#include <DAC.h>
#include <BeatData.h>

/*int activeSeq = 0;   // for LEDs
int* pSeq = &activeSeq;
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

//pitches A
int pitchA[16] = {792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792};
int changedPitchA[16] = {0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int* pPitchA = pitchA;
int* pPitchChangeA = changedPitchA;
//pitches B
int pitchB[16] = {792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792, 792};
int changedPitchB[16] = {0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int* pPitchB = pitchB;
int* pPitchChangeB = changedPitchB;*/

unsigned long tempo = 100;
unsigned long tempoTimer = 0; // tempo timer should reset every 1/32

/*int tempoDiv[5] = {2, 2, 2, 2, 2};
int* pDiv = tempoDiv;

float swing[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float* pSwing = swing;

unsigned long beatTimers[5] = {0, 0, 0, 0, 0};
int stepCounters[5] = {0, 0, 0, 0, 0};*/

unsigned long dt = 0;
unsigned long bt = 0;
unsigned long lastMs = 0;
//DATA
BeatData data;
//LEDs
LedMatrix leds;
LedMatrix *ledsPointer = &leds;
//Display
Display screen;
Display *pScreen = &screen;
//Buttons
Buttons buttons;
//DACs
DAC dacA;
DAC dacB;

void setup() {
  //Serial begin
  Serial.begin(9600);
  Serial.println("~ WELCOME TO CHRIS'S SEQUENCER ~");
  //Display
  screen.begin();
  //Buttons

  dacA.begin(data.beatStates, data.pitchA, 0x6);
  dacB.begin(data.beatStates, data.pitchB, 0x6);
}

void loop() {
  //time stuff
  dt = millis() - lastMs;
  tempoTimer += dt;
  lastMs = millis();

  //read buttons

  //fire clock
  if(tempoTimer > tempo) {
    tempoTimer = 0;
    if(data.stepNumber == 0) {
      //fire clock
      data.stepNumber = 0;
    }
    else { data.stepNumber = 1; }
  }
  //fire beats
  if(tempoTimer == 0){
    //Serial.println(String(beatTimers[0]) + " | " + String(tempoDiv[0] * tempo));
    // go through all sequences and fire beats
    for (int s = 0; s < 5; s ++) {
      if (data.beatTimer[s] > data.tempoDiv[s] * tempo) {
        // if beat of sequence is odd wait beatTime[x] * swing[x] before
        // firing beat
        if(data.swing[s] == 0) {
          //just fire beat
          data.beatTimer[s] = 0;
          if(s == data.activeSeq) {
            leds.step(data.beatStates, data.activeSeq, int(data.stepCounter[data.activeSeq]));
          }
          //only fire dac for first sequence
          if(s == 0) {
            dacA.updatePitch(data.stepCounter[data.activeSeq]);
          }
        }
        else {
          if(data.stepCounter[s] % 2 == 0) {
            //it's even: fire the beat immediately
            data.beatTimer[s] = 0;
            if(s == data.activeSeq) {
              leds.step(data.beatStates, data.activeSeq, int(data.stepCounter[data.activeSeq]));
            }
            //only fire dac for first sequence
            if(s == 0) {
              dacA.updatePitch(data.stepCounter[data.activeSeq]);
            }
          }
          else {
            //it's odd: wait swing[x] amount of time
            if(data.beatTimer[s] > data.tempoDiv[s] * tempo + data.tempoDiv[s] * tempo * data.swing[s]) {
              //we've reached desired swing, fire the beat
              data.beatTimer[s] = 0;
              if(s == data.activeSeq) {
                leds.step(data.beatStates, data.activeSeq, int(data.stepCounter[data.activeSeq]));
              }
              //only fire dac for first sequence
              if(s == 0) {
                dacA.updatePitch(data.stepCounter[data.activeSeq]);
              }
            }
            else { continue; }
          }
        }
        data.stepCounter[s] += 1;
        if(data.stepCounter[s] == 16) { data.stepCounter[s] = 0; }
      }
      else {
        //we aren't ready to fire this beat: do nothing
      }
    }
  }
  //
} // end loop()
