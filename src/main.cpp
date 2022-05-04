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
DAC dacA;
DAC dacB;

void setup() {
  //Serial begin
  Serial.begin(9600);
  Serial.println("~ WELCOME TO CHRIS'S SEQUENCER ~");
  //Display
  //screen.begin();
  //Buttons
  buttons.begin();
  dacA.begin(0, 0x6);
  dacB.begin(1, 0x6);
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
    }
    else { beatData->stepNumber = 1; }
  }
  //fire beats
  if(tempoTimer == 0){
    // go through all sequences and fire beats
    for (int s = 0; s < 5; s ++) {
      if (beatData->beatTimer[s] > beatData->tempoDiv[s] * tempo) {
        // if beat of sequence is odd wait beatTime[x] * swing[x] before
        // firing beat
        if(beatData->swing[s] == 0) {
          //just fire beat
          beatData->beatTimer[s] = 0;
          if(s == beatData->activeSeq) {
            leds.step();
          }
          //only fire dac for first sequence
          if(s == 0) {
            dacA.updatePitch(beatData->stepCounter[s]);
          }
          if(s == 1) {
            dacB.updatePitch(beatData->stepCounter[s]);
          }
        }
        else {
          if(beatData->stepCounter[s] % 2 == 0) {
            //it's even: fire the beat immediately
            beatData->beatTimer[s] = 0;
            if(s == beatData->activeSeq) {
              leds.step();
            }
            //only fire dac for first sequence
            if(s == 0) {
              dacA.updatePitch(beatData->stepCounter[beatData->activeSeq]);
            }
          }
          else {
            //it's odd: wait swing[x] amount of time
            if(beatData->beatTimer[s] > beatData->tempoDiv[s] * tempo + beatData->tempoDiv[s] * tempo * beatData->swing[s]) {
              //we've reached desired swing, fire the beat
              beatData->beatTimer[s] = 0;
              if(s == beatData->activeSeq) {
                leds.step();
              }
              //only fire dac for first sequence
              if(s == 0) {
                dacA.updatePitch(beatData->stepCounter[beatData->activeSeq]);
              }
            }
            else { continue; }
          }
        }
        beatData->stepCounter[s] += 1;
        if(beatData->stepCounter[s] == 16) { beatData->stepCounter[s] = 0; }
      }
      else {
        //we aren't ready to fire this beat: do nothing
      }
    }
  }
  //
} // end loop()
