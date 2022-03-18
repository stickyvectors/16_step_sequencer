#include <Arduino.h>

int currentSeq = 0;   // for LEDs
int currentBeat = 0;  // for LEDs
int stepNumber = 0;   // tick tock for clock

byte* beatStates[5][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

unsigned long tempo = 200;
unsigned long tempoTimer = 0; // tempo timer should reset every 1/32
int tempoDiv[5] = {2, 2, 2, 2, 2};
float swing[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
unsigned long beatTimers[5] = {0, 0, 0, 0, 0};
int stepCounters[5] = {0, 0, 0, 0, 0};

unsigned long dt = 0;
unsigned long lastMs = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  dt = millis() - lastMs;
  tempoTimer += dt;
  lastMs = millis();

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
    // go through all sequences and fire beats
    for (int s = 0; s < 5; s ++) {
      if (beatTimers[s] > tempoDiv[s] * tempo) {
        // if beat of sequence is odd wait beatTime[x] * swing[x] before
        //firing beat
        if(swing[s] == 0) {
          //just fire beat
          beatTimers[s] = 0;
        }
        else {
          if(stepCounters[s] % 2 == 0) {
            //it's even: fire the beat immediately
            beatTimers[s] = 0;
          }
          else {
            //it's odd: wait swing[x] amount of time
            if(beatTimers[s] > tempoDiv[s] * tempo + tempoDiv[s] * tempo * swing[s]) {
              //we've reached desired swing, fire the beat
              beatTimers[s] = 0;
            }
            else { beatTimers[s] += dt; }
          }
        }
        stepCounters[s] += 1;
        if(stepCounters[s] == 16) { stepCounters[s] = 0; }
      }
      else {
        //we aren't ready to fire this beat: update timer
        beatTimers[s] += dt;
      }
    }
  }
  //
} // end loop()
