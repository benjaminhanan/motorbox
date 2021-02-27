
// ----------LIBRARIES--------------

#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include "Stepper.h"



// --------CONSTANTS (won't change)---------------
const int LEDcountupdateinterval = 250;
const int buttonInterval = 200; // number of millisecs between button readings

unsigned long lastjoyread = 0;
unsigned long time;
unsigned long seconds = 0;
unsigned long firstsec = 0;
unsigned long secondsec = 0;
unsigned long lastButtonPress = 0;
byte seven_seg_digits[11] = { B11111100,  // = 0
                              B01100000,  // = 1
                              B11011010,  // = 2
                              B11110010,  // = 3
                              B01100110,  // = 4
                              B10110110,  // = 5
                              B10111110,  // = 6
                              B11100000,  // = 7
                              B11111110,  // = 8
                              B11100110,   // = 9
                              B11111100,
                             };


const int countledpin =  7;      // the pin numbers for the LEDs
const int ringledpin =  11;
const int stripledpin =  15;
const int numcountled = 10;
const int numringled = 12;
const int numstripled = 8;

const int ledcountdwnbutton = 13;
const int ledcountupbutton = 12;
const int modedwnbutton = 52;
const int modeupbutton = 53;

const int clk = 3;
const int dt = 2;
const int sw = 18;
 int currentStateCLK;
 int previousStateCLK;
 

// connect to the ST_CP of 74HC595 (pin 3,latch pin)
int latchPin = 9;
// connect to the SH_CP of 74HC595 (pin 4, clock pin)
int clockPin = 10;
// connect to the DS of 74HC595 (pin 2)
int dataPin = 8;

const int joyx = A2;
const int joyy = A1;
const int joybutton = A0;

const int systemoffpin = 26;
const int ledonpin = 27;

const int E1 = 44;
const int I1 = 32;
const int I2 = 33;

int stepsperrotation = 32;

Adafruit_NeoPixel ring (numringled, ringledpin, NEO_RGBW);
Adafruit_NeoPixel strip (numstripled, stripledpin, NEO_RGB);

CRGB countleds[numcountled];

//------------ VARIABLES (will change)---------------------

byte ledcountupbuttonstate = HIGH;
byte ledcountdwnbuttonstate = HIGH;
byte modecountupbuttonstate = HIGH;
byte modecountdwnbuttonstate = HIGH;
byte joybuttonstate = HIGH;

int rotarycount;
int stepstotake = 50;
int stepperspeed = 0;
int ledcount = 0;
int modecount = 2;
int dcspeed;
int counthue = 4;
int countbright;
int striphue;
int ringhue;
int huecount = 3;
int ringbright;

int xval;
int yval;

unsigned long currentMillis = 0;
unsigned long laststepperupdate = 0;
unsigned long previousButtonMillis = 0; // time when button press last checked
unsigned long previouscountupdateMillis = 0;

CHSV customColor;

void KNOBBUTTONISR() {
  if (digitalRead(sw) == LOW){

    if (millis() - lastButtonPress > 50)
    {
       huecount++;
    }
    
lastButtonPress = millis();
 
}
  delay(25); 
}

void KNOBISR() {
   // Read the current state of inputCLK
   currentStateCLK = digitalRead(clk);
    
   // If the previous and the current state of the inputCLK are different then a pulse has occured
   if (currentStateCLK != previousStateCLK){ 
       
     // If the inputDT state is different than the inputCLK state then 
     // the encoder is rotating counterclockwise
     if (digitalRead(dt) != currentStateCLK) { 
rotarycount++;
  secondsec = firstsec;

     } else {
       // Encoder is rotating clockwise
rotarycount--;
  secondsec = firstsec;
     }

   } 
   // Update previousStateCLK with the current state
   previousStateCLK = currentStateCLK;
   delay(15); 
}

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), KNOBISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), KNOBISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(18), KNOBBUTTONISR, CHANGE);
    
    FastLED.addLeds<WS2811, countledpin>(countleds, numcountled);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

pinMode(systemoffpin, OUTPUT);
pinMode(ledonpin, OUTPUT);

pinMode(ledcountupbutton, INPUT_PULLUP);
pinMode(ledcountdwnbutton, INPUT_PULLUP);
pinMode(modeupbutton, INPUT_PULLUP);
pinMode(modedwnbutton, INPUT_PULLUP);

pinMode(E1, OUTPUT);
pinMode(I1, OUTPUT);
pinMode(I2, OUTPUT);

   pinMode (clk,INPUT);
   pinMode (dt,INPUT);
   pinMode (sw, INPUT_PULLUP);
   
previousStateCLK = digitalRead(clk);

  pinMode(joybutton, INPUT_PULLUP);
  pinMode(joyy, INPUT);
  pinMode(joyx, INPUT);

ring.begin();
ring.show();

strip.begin();
strip.show();
}

void sevenSegWrite(byte ledcount) {
  // set the latchPin to low potential, before sending data
  digitalWrite(latchPin, LOW);
     
  // the original data (bit pattern)
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[ledcount]);  
 
  // set the latchPin to high potential, after sending data
  digitalWrite(latchPin, HIGH);
}

void readbuttons () {

  if (millis() - previousButtonMillis >= buttonInterval) {

   if (digitalRead(ledcountupbutton) == LOW) {
      ledcountupbuttonstate = LOW; // this changes it to LOW if it was HIGH
      ledcount++;
      secondsec = firstsec; 
    }   else {
          ledcountupbuttonstate = HIGH;
        }    

    if (digitalRead(ledcountdwnbutton) == LOW) {
      ledcountdwnbuttonstate = LOW; // this changes it to LOW if it was HIGH
      ledcount--;
        secondsec = firstsec; 
    } else {
          ledcountdwnbuttonstate = HIGH;
        }

    if (digitalRead(modeupbutton) == LOW) {
      modecountupbuttonstate = LOW; // this changes it to LOW if it was HIGH
      modecount++;
      secondsec = firstsec;
    }  else {
          modecountupbuttonstate = HIGH;
        }

    if (digitalRead(modedwnbutton) == LOW) {
      modecountdwnbuttonstate = LOW; // this changes it to LOW if it was HIGH
      modecount--;
      secondsec = firstsec;
    }  else {
          modecountdwnbuttonstate = HIGH;
        }

      previousButtonMillis += buttonInterval;
  }
}

void rotaryupdate(){
  if (rotarycount==5){
    ledcount++;
    rotarycount=0;
  }

  if (rotarycount==-5){
    ledcount--;
    rotarycount=0;
  }
  
}

void huecountlimit(){

  if (huecount > 6){
    huecount = 6;
  }

  if (huecount < 0){
    huecount = 0;
  }


}

void hueupdate(){


}

void ringupdate(){

ringhue = 65536*(huecount/6);
ringbright = ledcount*15;
striphue = 65536*((huecount*2)/6);
uint32_t ringcolor = ring.gamma32(ring.ColorHSV(ringhue, 255, ringbright));
uint32_t stripcolor = strip.gamma32(strip.ColorHSV(striphue, 255, 100));
uint32_t blackcolor = strip.gamma32(strip.ColorHSV(0, 255, 0));
counthue = 255*(huecount/6);
CHSV customColor = CHSV(counthue, 255, 150);
ring.setPixelColorHSV(i, h, sat, val);
ring.setPixelColor(1, ringcolor);
ring.setPixelColor(2, ringcolor);
ring.setPixelColor(3, ringcolor);
ring.setPixelColor(4, ringcolor);
ring.setPixelColor(5, ringcolor);
ring.setPixelColor(6, ringcolor);
ring.setPixelColor(7, ringcolor);
ring.setPixelColor(8, ringcolor);
ring.setPixelColor(9, ringcolor);
ring.setPixelColor(10, ringcolor);
ring.setPixelColor(11, ringcolor);
ring.show();
  
}


void updatecounts(){

if (ledcount > 10){
  ledcount = 10;
}

if (ledcount < 0){
  ledcount = 0;
}

if (modecount > 8){
  modecount = 8;
}

if (modecount < 1){
  modecount = 1;
}

previouscountupdateMillis += LEDcountupdateinterval;

}

void dcmotorupdate(){
digitalWrite(I1, HIGH);
digitalWrite(I2, LOW);
analogWrite(E1, ledcount*15);
  
}

void countledupdate() {
CHSV customColor = CHSV(counthue, 255, 150);
if (ledcount == 0){
countleds[0] = CRGB::Black;
countleds[2] = CRGB::Black;
countleds[4] = CRGB::Black;
countleds[6] = CRGB::Black;
countleds[8] = CRGB::Black;
countleds[1] = CRGB::Black;
countleds[3] = CRGB::Black;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}
  
if (ledcount == 1){
countleds[0] = customColor;
countleds[2] = CRGB::Black;
countleds[4] = CRGB::Black;
countleds[6] = CRGB::Black;
countleds[8] = CRGB::Black;
countleds[1] = CRGB::Black;
countleds[3] = CRGB::Black;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 2){
countleds[0] = customColor;
countleds[2] = customColor;
countleds[4] = CRGB::Black;
countleds[6] = CRGB::Black;
countleds[8] = CRGB::Black;
countleds[1] = CRGB::Black;
countleds[3] = CRGB::Black;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 3){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Black;
countleds[8] = CRGB::Black;
countleds[1] = CRGB::Black;
countleds[3] = CRGB::Black;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 4){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Green;
countleds[8] = CRGB::Black;
countleds[1] = CRGB::Black;
countleds[3] = CRGB::Black;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 5){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Green;
countleds[8] = CRGB::Green;
countleds[1] = CRGB::Black;
countleds[3] = CRGB::Black;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 6){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Green;
countleds[8] = CRGB::Green;
countleds[1] = CRGB::Green;
countleds[3] = CRGB::Black;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 7){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Green;
countleds[8] = CRGB::Green;
countleds[1] = CRGB::Green;
countleds[3] = CRGB::Green;
countleds[5] = CRGB::Black;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 8){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Green;
countleds[8] = CRGB::Green;
countleds[1] = CRGB::Green;
countleds[3] = CRGB::Green;
countleds[5] = CRGB::Green;
countleds[7] = CRGB::Black; 
countleds[9] = CRGB::Black;
}

if (ledcount == 9){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Green;
countleds[8] = CRGB::Green;
countleds[1] = CRGB::Green;
countleds[3] = CRGB::Green;
countleds[5] = CRGB::Green;
countleds[7] = CRGB::Green; 
countleds[9] = CRGB::Black;
}

if (ledcount == 10){
countleds[0] = CRGB::Green;
countleds[2] = CRGB::Green;
countleds[4] = CRGB::Green;
countleds[6] = CRGB::Green;
countleds[8] = CRGB::Green;
countleds[1] = CRGB::Green;
countleds[3] = CRGB::Green;
countleds[5] = CRGB::Green;
countleds[7] = CRGB::Green; 
countleds[9] = CRGB::Green;
}
  FastLED.show();
}

void modestripupdate(){

  if (modecount == 1){
strip.setPixelColor(0, stripcolor);
strip.setPixelColor(1, blackcolor);
strip.setPixelColor(2, blackcolor);
strip.setPixelColor(3, blackcolor);
strip.setPixelColor(4, blackcolor);
strip.setPixelColor(5, blackcolor);
strip.setPixelColor(6, blackcolor);
strip.setPixelColor(7, blackcolor);

  }
  if (modecount == 2){
strip.setPixelColor(0, blackcolor);
strip.setPixelColor(1, stripcolor);
strip.setPixelColor(2, blackcolor);
strip.setPixelColor(3, blackcolor);
strip.setPixelColor(4, blackcolor);
strip.setPixelColor(5, blackcolor);
strip.setPixelColor(6, blackcolor);
strip.setPixelColor(7, blackcolor);

  }

    if (modecount == 3){
strip.setPixelColor(0, blackcolor);
strip.setPixelColor(1, blackcolor);
strip.setPixelColor(2, stripcolor);
strip.setPixelColor(3, blackcolor);
strip.setPixelColor(4, blackcolor);
strip.setPixelColor(5, blackcolor);
strip.setPixelColor(6, blackcolor);
strip.setPixelColor(7, blackcolor);

  }

    if (modecount == 4){
strip.setPixelColor(0, blackcolor);
strip.setPixelColor(1, blackcolor);
strip.setPixelColor(2, blackcolor);
strip.setPixelColor(3, stripcolor);
strip.setPixelColor(4, blackcolor);
strip.setPixelColor(5, blackcolor);
strip.setPixelColor(6, blackcolor);
strip.setPixelColor(7, blackcolor);

  }

    if (modecount == 5){
strip.setPixelColor(0, blackcolor);
strip.setPixelColor(1, blackcolor);
strip.setPixelColor(2, blackcolor);
strip.setPixelColor(3, blackcolor);
strip.setPixelColor(4, stripcolor);
strip.setPixelColor(5, blackcolor);
strip.setPixelColor(6, blackcolor);
strip.setPixelColor(7, blackcolor);

  }
    if (modecount == 6){
strip.setPixelColor(0, blackcolor);
strip.setPixelColor(1, blackcolor);
strip.setPixelColor(2, blackcolor);
strip.setPixelColor(3, blackcolor);
strip.setPixelColor(4, blackcolor);
strip.setPixelColor(5, stripcolor);
strip.setPixelColor(6, blackcolor);
strip.setPixelColor(7, blackcolor);

  }
    if (modecount == 7){
strip.setPixelColor(0, blackcolor);
strip.setPixelColor(1, blackcolor);
strip.setPixelColor(2, blackcolor);
strip.setPixelColor(3, blackcolor);
strip.setPixelColor(4, blackcolor);
strip.setPixelColor(5, blackcolor);
strip.setPixelColor(6, stripcolor);
strip.setPixelColor(7, blackcolor);

  }
  
    if (modecount == 8){
strip.setPixelColor(0, blackcolor);
strip.setPixelColor(1, blackcolor);
strip.setPixelColor(2, blackcolor);
strip.setPixelColor(3, blackcolor);
strip.setPixelColor(4, blackcolor);
strip.setPixelColor(5, blackcolor);
strip.setPixelColor(6, blackcolor);
strip.setPixelColor(7, stripcolor);

  }
  strip.show();
}

void loop(){
  // put your main code here, to run repeatedly:
currentMillis = millis();
seconds = currentMillis/1000;
firstsec = seconds;
readbuttons();
updatecounts();
huecountlimit();
hueupdate();
sevenSegWrite(ledcount);
rotaryupdate();
countledupdate();
modestripupdate();
dcmotorupdate();
ringupdate();




if (seconds > .3){
digitalWrite(ledonpin, HIGH);
}

if ((secondsec + 10) < firstsec) {
  digitalWrite(systemoffpin, HIGH);
}

}
