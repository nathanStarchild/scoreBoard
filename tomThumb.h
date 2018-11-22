////************************************************************
//// Moonbase Code 2018
//// Location: Status screen
//// Shape: none
//// LEDs: none
//// Interactivity: status output
////************************************************************
//
////************************************************************
////includes
//#define I2C_ADDRESS 0x3C
//#include <Wire.h>
//#include "SSD1306Ascii.h"
//#include "SSD1306AsciiWire.h"
//
//SSD1306AsciiWire oled;
////************************************************************
//
////************************************************************
////Compile time Constants
//const uint8_t stripLength = 42;//42
//const uint8_t nStrips = 2;//1
//const uint16_t num_leds = stripLength * nStrips;
////************************************************************
//
////************************************************************
////Global variables
//int directionLR[nStrips] = {1};
//int directionUD[nStrips] = {1};
//int directionIO[nStrips] = {1};
//int stripDirection[nStrips] = {1};
//uint16_t audienceSpot = 1;
//uint8_t sweepSpot = 1;
////************************************************************





//************************************************************
// Moonbase Code 2018
// Location: Alcyone
// Shape: Large Icosahedron
// LEDs: 12 x 2m strips
// Interactivity: Pro Joy
//************************************************************

//************************************************************
//includes
#include "Adafruit_ADS1015.h"
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;
const uint8_t nStrips = 12;
const uint16_t num_leds = stripLength * nStrips;
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {-1, 1, -1, 1, -1, 1, 1, -1, 1, -1, 1, -1};
int directionIO[nStrips] = {-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1};
int stripDirection[nStrips] = {-1, 1, -1, 1, -1, 1, 1, -1, 1, -1, 1, -1};

uint16_t audienceSpot = 6;
uint16_t sweepSpot = 6;

MilliTimer buttonCheckTimer(100);

int8_t IOVal = 0;
int8_t UDVal = 0;
int8_t LRVal = 0;
uint8_t buttonVal = 0;
uint8_t directionVal = 1;
int8_t msgVal = 0;
bool holdOn = false;
bool newVal = false;
bool rev = false;

Adafruit_ADS1115 ads(0x48);
//************************************************************

//************************************************************
//functions
void pleiadianSetup();
void checkInputs();
void proJoyMessage(int8_t msgVal, uint8_t directionVal, bool rev);


// Test for minimum program size.

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
//
SSD1306AsciiWire oled;

//************************************************************

void pleiadianSetup() {
//  Serial.println("anything?");
  Wire.begin();                
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.clear();
  oled.print("Hello world!!!!!");
}

void checkInputs() {
  //pass
}
