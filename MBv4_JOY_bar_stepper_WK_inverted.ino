#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include "Stepper.h"
#define STEPS  32
#define LED_PIN     7
#define LED_PIN2     11
#define LED_PIN3     15
#define NUM_LEDS 10

#define NUM_LEDS2    12
#define NUM_LEDS3    8
#define inputCLK 3
#define inputDT 2
#define inputSW 18
#define pwrpin  26
#define pwrpin2 27
#define dwnsw 13
#define upsw 12
#define modeup 53
#define modedwn 52
#define joypush A0
#define joyy A1
#define joyx A2

int xval = 480;
int yval = 480;

int xneutral = 480;
int yneutral = 480;

int StepsToTake = 0;      // How much to move Stepper
Stepper small_stepper(STEPS, 36, 38, 37, 39);

unsigned long lastjoyread = 0;
unsigned long time;
unsigned long seconds = 0;
unsigned long firstsec = 0;
unsigned long secondsec = 0;
unsigned long lastButtonPress = 0;
 uint32_t rgbcolor;
uint32_t stripcolor;
// byte seven_seg_digits[11] = { B11111100,  // = 0
//                               B01100000,  // = 1
//                               B11011010,  // = 2
//                               B11110010,  // = 3
//                               B01100110,  // = 4
//                               B10110110,  // = 5
//                               B10111110,  // = 6
//                               B11100000,  // = 7
//                               B11111110,  // = 8
//                               B11100110,   // = 9
//                               B11111100,
//                             };

byte seven_seg_digits[11] = {
B00000011,
B10011111,
B00100101,
B00001101,
B10011001,
B01001001,
B01000001,
B00011111,
B00000001,
B00011001,
B00000011,
};
 
// connect to the ST_CP of 74HC595 (pin 3,latch pin)
int latchPin = 9;
// connect to the SH_CP of 74HC595 (pin 4, clock pin)
int clockPin = 10;
// connect to the DS of 74HC595 (pin 2)
int dataPin = 8;

CRGB leds[NUM_LEDS];

int E1 = 44;
int I1 = 32;
int I2 = 33;
int dcspeed = 0;
int rotarycount = 0;
int count = 3; 
 int currentStateCLK;
 int previousStateCLK;
boolean motordir = true;


int swcount = 0;
int hue = 0;
int hue2 = 65535;
int bright = 75;

void KNOBBUTTONISR() {
  if (digitalRead(inputSW) == LOW){

    if (millis() - lastButtonPress > 50)
    {
  swcount++;
  secondsec = firstsec;
    }
    
lastButtonPress = millis();
 
}
  delay(25); 
}

void KNOBISR() {
   // Read the current state of inputCLK
   currentStateCLK = digitalRead(inputCLK);

     // Read the current state of inputCLK
   currentStateCLK = digitalRead(inputCLK);
    
   // If the previous and the current state of the inputCLK are different then a pulse has occured
   if (currentStateCLK != previousStateCLK){ 
       
     // If the inputDT state is different than the inputCLK state then 
     // the encoder is rotating counterclockwise
     if (digitalRead(inputDT) != currentStateCLK) { 

              count++;
  secondsec = firstsec;
  dcspeed=dcspeed + 5;

     } else {
       // Encoder is rotating clockwise
       count--;
         dcspeed=dcspeed - 5;
  secondsec = firstsec;
     }

   } 
   // Update previousStateCLK with the current state
   previousStateCLK = currentStateCLK;
   delay(15); 
}


Adafruit_NeoPixel ring (NUM_LEDS2, LED_PIN2, NEO_RGBW);
Adafruit_NeoPixel strip (NUM_LEDS3, LED_PIN3, NEO_RGB);

void setup(){
Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), KNOBISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), KNOBISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(18), KNOBBUTTONISR, CHANGE);
    
    FastLED.addLeds<WS2811, LED_PIN>(leds, NUM_LEDS);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

pinMode(pwrpin, OUTPUT);
pinMode(pwrpin2, OUTPUT);

pinMode(upsw, INPUT_PULLUP);
pinMode(dwnsw, INPUT_PULLUP);

pinMode(E1, OUTPUT);
pinMode(I1, OUTPUT);
pinMode(I2, OUTPUT);

   pinMode (inputCLK,INPUT);
   pinMode (inputDT,INPUT);
   pinMode (inputSW, INPUT_PULLUP);
   
previousStateCLK = digitalRead(inputCLK);

  pinMode(joypush, INPUT_PULLUP);
  pinMode(joyy, INPUT);
  pinMode(joyx, INPUT);

ring.begin();
ring.show();

strip.begin();
strip.show();
  
}

void sevenSegWrite(byte count) {
  // set the latchPin to low potential, before sending data
  digitalWrite(latchPin, LOW);
     
  // the original data (bit pattern)
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[count]);  
 
  // set the latchPin to high potential, after sending data
  digitalWrite(latchPin, HIGH);
}

void loop(){

xval = analogRead(joyx);
yval = analogRead(joyy);
uint32_t stripcolor = strip.ColorHSV(25000, 255, 100);
strip.fill(stripcolor, 0, 5);
strip.show();
  
    if (swcount > 5) {
  swcount = 0;
}

if((time-35)>lastjoyread) {

  if (yval>700) {
  StepsToTake = StepsToTake + ((xval-512)*.07);
  firstsec = seconds;
  }

  if (yval<300) {
  StepsToTake = StepsToTake + ((xval-512)*.03);
  firstsec = seconds;
  }
  
    lastjoyread=time;
}
  small_stepper.setSpeed(700); 
    small_stepper.step(StepsToTake);
  StepsToTake = 0;
analogWrite(E1, dcspeed);
time = millis();
unsigned long seconds = (time/1000);
firstsec = seconds;

if (motordir == true) {
digitalWrite(I1, HIGH);
digitalWrite(I2, LOW);
}


if (motordir == false) {
digitalWrite(I1, HIGH);
digitalWrite(I2, LOW);
}

if (digitalRead(upsw) == LOW){
  count++;
  dcspeed = dcspeed + 10;
  secondsec = firstsec;
  delay(70);
}

if (digitalRead(dwnsw) == LOW){
  count--;
  dcspeed = dcspeed - 10;
  secondsec = firstsec;
  delay(70);
}


if (swcount == 0){
  hue = 0;
  hue2 = 1820;

}

if (swcount == 1){
  hue = 51;
  hue2 = 3640;

}

if (swcount == 2){
  hue = 102;
  hue2 = 5460;

}

if (swcount == 3){
  hue = 153;
  hue2 = 7280;

}

if (swcount == 4){
  hue = 204;
  hue2 = 9100;

}

if (swcount == 5){
  hue = 255;
  hue2 = 10920;

}

if (swcount > 5) {
  swcount = 0;
  hue2 = 0;

}

  CHSV customColor = CHSV(hue, 255, bright);
  uint32_t rgbcolor = ring.ColorHSV((hue2*100), 255, dcspeed);

    analogWrite(E1, dcspeed);

if (rotarycount == 3)
{
  count++;
  rotarycount=0;
}

if (rotarycount == -3)
{
  count--;
  rotarycount=0;
}



if (count == 0) {

leds[0] = CRGB::Black;
leds[2] = CRGB::Black;
leds[4] = CRGB::Black;
leds[6] = CRGB::Black;
leds[8] = CRGB::Black;
leds[1] = CRGB::Black;
leds[3] = CRGB::Black;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black; 
        FastLED.show();
sevenSegWrite(0);

  }

if ((count/3) == 1) {

leds[0] = customColor;
leds[2] = CRGB::Black;
leds[4] = CRGB::Black;
leds[6] = CRGB::Black;
leds[8] = CRGB::Black;
leds[1] = CRGB::Black;
leds[3] = CRGB::Black;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black; 
FastLED.show();
sevenSegWrite(1);

  }


if ((count/3) == 2) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = CRGB::Black;
leds[6] = CRGB::Black;
leds[8] = CRGB::Black;
leds[1] = CRGB::Black;
leds[3] = CRGB::Black;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black; 
FastLED.show();
sevenSegWrite(2);

  }

  
if ((count/3) == 3) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = CRGB::Black;
leds[8] = CRGB::Black;
leds[1] = CRGB::Black;
leds[3] = CRGB::Black;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black;
FastLED.show();
sevenSegWrite(3);

  }

if ((count/3) == 4) {
  
leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = customColor;
leds[8] = CRGB::Black;
leds[1] = CRGB::Black;
leds[3] = CRGB::Black;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black;
FastLED.show();
sevenSegWrite(4);

  }

if ((count/3) == 5) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = customColor;
leds[8] = customColor;
leds[1] = CRGB::Black;
leds[3] = CRGB::Black;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black;
FastLED.show();
sevenSegWrite(5);

  }

if ((count/3) == 6) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = customColor;
leds[8] = customColor;
leds[1] = customColor;
leds[3] = CRGB::Black;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black;
FastLED.show();
sevenSegWrite(6);

  }

if ((count/3) == 7) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = customColor;
leds[8] = customColor;
leds[1] = customColor;
leds[3] = customColor;
leds[5] = CRGB::Black;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black;
FastLED.show();
sevenSegWrite(7);

  }

if ((count/3) == 8) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = customColor;
leds[8] = customColor;
leds[1] = customColor;
leds[3] = customColor;
leds[5] = customColor;
leds[7] = CRGB::Black; 
leds[9] = CRGB::Black;
FastLED.show();
sevenSegWrite(8);

  }

if ((count/3) == 9) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = customColor;
leds[8] = customColor;
leds[1] = customColor;
leds[3] = customColor;
leds[5] = customColor;
leds[7] = customColor;
leds[9] = CRGB::Black;
FastLED.show();
sevenSegWrite(9);

  }

  if ((count/3) == 10) {

leds[0] = customColor;
leds[2] = customColor;
leds[4] = customColor;
leds[6] = customColor;
leds[8] = customColor;
leds[1] = customColor;
leds[3] = customColor;
leds[5] = customColor;
leds[7] = customColor;
leds[9] = customColor;
FastLED.show();
sevenSegWrite(0);

  }

if (dcspeed <= 100 && dcspeed > -1) {
ring.setPixelColor(0, rgbcolor);
ring.setPixelColor(1, rgbcolor);
ring.setPixelColor(2, rgbcolor);
ring.setPixelColor(3, rgbcolor);
ring.setPixelColor(4, rgbcolor);
ring.setPixelColor(5, rgbcolor);
ring.setPixelColor(6, rgbcolor);
ring.setPixelColor(7, rgbcolor);
ring.setPixelColor(8, rgbcolor);
ring.setPixelColor(9, rgbcolor);
ring.setPixelColor(10, rgbcolor);
ring.setPixelColor(11, rgbcolor);
ring.show();
}

if (count < 0)
{
  count = 0;
}

if (count > 30)
{
  count = 30;
}

if (dcspeed < 0){
  dcspeed = 0;
}

if (dcspeed > 200){
  dcspeed = 200;
}

if ((secondsec + 5) < firstsec) {
  bright = 25;
}
else {
  bright = 75;
}

if (yval>600){

  secondsec = firstsec;
  
}

if (yval<400){

  secondsec = firstsec;
  
}

if ((secondsec + 50) < firstsec) {
  digitalWrite(pwrpin, HIGH);
}


if (seconds > .3) {
  digitalWrite(pwrpin2, HIGH);
}
  Serial.println(xval);


}
