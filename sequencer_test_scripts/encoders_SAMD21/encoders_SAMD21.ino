#define pinA 8
#define pinB 9

volatile byte aFlag = 0;
volatile byte bFlag = 0;
int readingA = 0;
int readingB = 0;
volatile byte encoderPosition = 0;
volatile byte oldEncoderPosition = 0;

void setup() {
  pinMode(pinA, INPUT_PULLDOWN);
  pinMode(pinB, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(pinA), PinA, RISING); // EXTIN[6]
  attachInterrupt(digitalPinToInterrupt(pinB), PinB, RISING); // EXTIN[7]

  SerialUSB.begin(115200);
}

void PinA() {
  noInterrupts(); //block interrupts before reading pins

  readingA = digitalRead(pinA);
  readingB = digitalRead(pinB);

  if(readingA && readingB && aFlag) {
    encoderPosition --;
    bFlag = 0;
    aFlag = 0;
  }
  else if(readingA && !readingB) bFlag = 1;
  interrupts(); // resume interrupts
}

void PinB() {
  noInterrupts(); //block interrupts before reading pins

  readingA = digitalRead(pinA);
  readingB = digitalRead(pinB);

  if(readingA && readingB && bFlag) {
    encoderPosition ++;
    bFlag = 0;
    aFlag = 0;
  }
  else if(!readingA && readingB) aFlag = 1;
  interrupts(); // resume interrupts
}

void loop() {
  if(oldEncoderPosition != encoderPosition) {
    SerialUSB.println(encoderPosition);
    oldEncoderPosition = encoderPosition;
  }
}