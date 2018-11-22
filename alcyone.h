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
int directionUD[nStrips] = {1, 1, -1, 1, -1, 1, -1, -1, 1, -1, 1, -1};
int directionIO[nStrips] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1};
int stripDirection[nStrips] = {1, 1, -1, 1, -1, 1, -1, -1, 1, -1, 1, -1};

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

//************************************************************

void pleiadianSetup() {
  ads.begin();
}

void checkInputs() {
  if (buttonCheckTimer.isItTime()){
    uint16_t maxVal = 25700;
    uint16_t adc0 = min(maxVal, ads.readADC_SingleEnded(0));  // we read from the ADC, we have a sixteen bit integer as a result
    uint16_t adc1 = min(maxVal, ads.readADC_SingleEnded(1));
    uint16_t adc2 = min(maxVal, ads.readADC_SingleEnded(2));
    uint16_t adc3 = min(maxVal, ads.readADC_SingleEnded(3));

    int8_t m0 = map(adc0, 0, maxVal, 5, -5);
    int8_t m1 = map(adc1, 0, maxVal, 5, -5);
    int8_t m2 = map(adc2, 0, maxVal, 5, -5);
//    m2 = max(1, (int) m2);
    uint8_t m3 = map(adc3, 0, maxVal, 10, 0);
//    m3 = max(1, (int) m3);
//    Serial.printf("%d, %d, %d, %d\n", adc0, adc1, adc2, adc3);
//    Serial.printf("%d, %d, %d, %d\n", m0, m1, m2, m3);
    if (!holdOn){
      int8_t maxIn = max(max(abs(m0), abs(m1)), abs(m2));
      if (abs(m0) == maxIn) {
        if (m0 != IOVal){
          IOVal = m0;
          if (m0 < 0) {
            rev = true;
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = -1 * directionIO[i];
            }
          } else {
            rev = false;
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = directionIO[i];
            }
          }
          directionVal = 1;
          msgVal = m0;
          newVal = true;
        }
      } else if (abs(m1) == maxIn) {
        if (m1 != LRVal){
          LRVal = m1;
          if (m1 < 0) {
            rev = true;
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = -1 * directionLR[i];
            }
          } else {
            rev = false;
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = directionLR[i];
            }
          }
          directionVal = 2;
          msgVal = m1;
          newVal = true;
        }
      } else if (abs(m2) == maxIn) {
        if (m2 != UDVal){
          UDVal = m2;
          if (m2 < 0) {
            rev = true;
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = -1 * directionUD[i];
            }
          } else {
            rev = false;
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = directionUD[i];
            }
          }
          directionVal = 3;
          msgVal = m2;
          newVal = true;
        }
      }
      if (newVal && !(holdOn)) {
        if (msgVal == 0) {
          mainState.skaters.enabled = false;
        } else {
          mainState.skaters.enabled = true;
          mainState.skaters.plength = abs(msgVal);
          mainState.skaters.pspeed = 1;
  //        if (abs(msgVal > 3)) {
  //          mainState.skaters.pspeed = 2;
  //        } else {
  //          mainState.skaters.pspeed = 1;
  //        }
        }
      }
    }
    if (m3 != buttonVal) {
      buttonVal = m3;
      if ((m3 == 10) && holdOn) {
        holdOn = false;
        Serial.println("holdOff");
      } else if (m3 == 10) {
        proJoyMessage(msgVal, directionVal, rev);
        holdOn = true;
        Serial.println("holdOn");
      } 
    }
    
    buttonCheckTimer.resetTimer();
  }
}

void proJoyMessage(int8_t msgVal, uint8_t directionVal, bool rev) {
  String skaterMsgTemplate = "{\"poMessageType\": 9,  \"msgVal\": 0,  \"directionVal\": 0,  \"rev\": 0,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(skaterMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 9;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["patternStep"] = mainState.patternStep;
  
  
  nm_root["msgVal"] = msgVal;
  nm_root["directionVal"] = directionVal;
  nm_root["rev"] = rev;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

