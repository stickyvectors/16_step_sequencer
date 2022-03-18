#include "Arduino.h"
#include "Display.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//screen config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Display::Display()
{
  //config screen
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  //initial display state
  updateBasicInfo();
}
//called every frame
void Display::tick(unsigned long dt) {
  if(_update){
    //do nothing until a time has elapsed
    _stateTimer += dt;
    if(_stateTimer > 1000) {
      updateBasicInfo();
    }
  }
}

void Display::seq(int s) {
  _seq = s;
  updateBasicInfo();
}

void Display::div(int d) {
  _div = d;
  updateBasicInfo();
}

void Display::pitch(int p) {
  //if pitch / 33 = 12 then note = C
  int r = (p/33) % 12;
  char note[2];
  note[0] = _notes[r][0];
  note[1] = _notes[r][1];
  display.clearDisplay();
  display.setTextSize(1);
  display.print("NOTE:");
  display.setTextSize(4);
  display.print(note);
  display.display();
  _update = true;
  _stateTimer = 0;
}

void Display::updateBasicInfo() {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

  if(_seq < 10) {
    _info[11] = '0';
    _info[12] = char(_seq);
  }
  else {
    char cSeq[2];
    itoa(_seq, cSeq, 10);
    _info[11] = cSeq[0];
    _info[12] = cSeq[1];
  }

  int fraction = _div / 32;
  char cDiv[2];
  itoa(fraction, cDiv, 10);
  _info[17] = cDiv[0];
  _info[18] = cDiv[1];

  display.print(_info);
  display.display();
}
