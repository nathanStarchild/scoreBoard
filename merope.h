//************************************************************
// Moonbase Code 2018
// Location: Merope
// Shape: Stellated Tetrahedron
// LEDs: 6 x 2m strips
// Interactivity: Map Buttons
//************************************************************

//************************************************************
//includes

#include <Wire.h>
#include "Adafruit_MCP23017.h"
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;//42
const uint8_t nStrips = 6;//1
const uint16_t num_leds = stripLength * nStrips;
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, -1, 1, -1, 1, -1};
int directionIO[nStrips] = {1, -1, 1, -1, 1, -1};
int stripDirection[nStrips] = {1, -1, 1, -1, 1, -1};
uint16_t audienceSpot = 5;
uint16_t sweepSpot = 5;

MilliTimer mapButtonCheckTimer(100);
MilliTimer mapButtonSpeedTimer(300);

uint16_t lastMapVal = 0;

String mapMsgTemplate = "{\"poMessageType\": 3,  \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0, \"plength\": 0, \"pspeed\": 0}";
//void mapMessage(uint16_t b, uint8_t n);

Adafruit_MCP23017 mcp;
//************************************************************

//************************************************************
//functions

void pleiadianSetup() {
    mcp.begin();      // use default address 0

    Wire.beginTransmission(0x20);
    Wire.write(0x00); // IODIRA register
    Wire.write(0x00); // set all of port A to outputs
    Wire.endTransmission();
    Wire.beginTransmission(0x20);
    Wire.write(0x0D); // register
    Wire.write(0xFF); // set all of port B to pullup
    Wire.endTransmission();
}

void mapMessage(uint16_t b, uint8_t n) { //send the data from the enlightenment button
//String mapMsgTemplate = "{\"poMessageType\": 2,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(mapMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 3;
  nm_root["audience"] = b;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["plength"] = n;
  nm_root["pspeed"] = mainState.mapPattern.pspeed;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
//  mesh.sendBroadcast(nm_msgp);
  outbox = true;
}

uint16_t checkMap(){
  uint16_t buttons = ~mcp.readGPIOAB(); //the ~ flips everything since the chip is active low logic
  for (int i=0;i<7;i++) {//was i<8
    if ((buttons >>8) & (0x01 << i)){
      mcp.digitalWrite(i, HIGH);
    } else {
      mcp.digitalWrite(i, LOW);
    }
  }
  if (mainState.mapPattern.pspeed == 1) {
    mcp.digitalWrite(mainState.patternStep/25 % 8, HIGH);//TODO fix jitter....
  }
//  if (!buttons) {Serial.println(buttons,BIN);} 
//  Serial.println(buttons,BIN);
  return buttons;
}

void checkInputs() {
  static uint8_t nMapOn = 0;
  if (mapButtonCheckTimer.isItTime()){
    uint16_t mapVal = checkMap();
    mapVal = mapVal >> 8; //yep
    if (mapVal != lastMapVal){
      mapButtonSpeedTimer.resetTimer();
      mainState.mapPattern.pspeed = min(mainState.mapPattern.pspeed + 1, 50);
      lastMapVal = mapVal;
//      Serial.println(mapVal,BIN);
      nMapOn = 0;
      for (int i=0; i<7; i++){//was i<8
        if (mapVal & (0x01 << i)){
          nMapOn++;
        }
      }
//      Serial.println(nMapOn);
      mapMessage(mapVal, nMapOn);
      if (nMapOn > 0) {
        mainState.mapPattern.enabled = true;
        mainState.mapPattern.plength = nMapOn;
//        mainState.mapPattern.plength++;
      } else {
        mainState.mapPattern.enabled = false;
      }
    } else if (mapButtonSpeedTimer.isItTime() && !mainState.mapPattern.enabled) {
      mapButtonSpeedTimer.resetTimer();
      mainState.mapPattern.pspeed = max(mainState.mapPattern.pspeed - 1, 1);
    }
    mapButtonCheckTimer.resetTimer();
  }
}

