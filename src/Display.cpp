#include "Arduino.h"
#include "Display.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//screen config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char _notes[13][3] = {{"C"}, {"C#"}, {"D"}, {"D#"},
                      {"E"}, {"F"}, {"F#"}, {"G"},
                      {"G#"}, {"A"}, {"A#"}, {"B"}};

Display::Display()
{
  //
}

void Display::begin() {
  //config screen
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // splash screen
  //fillTriangle();
  //initial display state
  updateBasicInfo();
}

void Display::home() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(_info);
  display.display();
}
void Display::seq(int s) {
  _seq = s + 1;
  _info[11] = '0';
  _info[12] = '0' + _seq;
  home();
}

void Display::div(int d) {
  _div = d;
  int fraction = 32 / _div;

  if(fraction == 10) {
    fraction = 12;
  }
  //all other odd divisions get a bit weird
  char cDiv[2];
  itoa(fraction, cDiv, 10);
  _info[18] = cDiv[0];
  _info[19] = cDiv[1];
  home();
}

void Display::swing(float s) {
  int percentage = int(s * 100);
  char cPercentage[3];
  itoa(percentage, cPercentage, 10);
  if(percentage < 10) {
    cPercentage[1] = '%';
  }
  else {  cPercentage[2] = '%'; }
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("SWING: ");
  display.setTextSize(4);
  display.print(cPercentage);
  display.display();
}
void Display::pitch(int p) {
  //if pitch / 33 = 12 then note = C
  int r = (p/33) % 12;
  char note[2];
  note[0] = _notes[r][0];
  note[1] = _notes[r][1];

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("NOTE: ");
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
    _info[12] = '0' + _seq;
  }
  else {
    char cSeq[2];
    itoa(_seq, cSeq, 10);
    _info[11] = cSeq[0];
    _info[12] = cSeq[1];
  }

  int fraction = 32 / _div;
  char cDiv[2];
  itoa(fraction, cDiv, 10);
  _info[18] = cDiv[0];
  _info[19] = cDiv[1];
  display.print(_info);
  display.display();
}

void Display::fillTriangle() {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(100);
  }
}
