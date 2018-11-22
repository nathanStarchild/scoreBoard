//************************************************************
// Moonbase Code 2018
// Location: Elektra
// Shape: octahorny
// LEDs :6 x 2m strips
// Interactivity: Random Analog
//************************************************************
//dimmer
//breather
//frame rate ctl
//palette change rate

//************************************************************
//includes
#include "Adafruit_ADS1015.h"
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;
const uint8_t nStrips = 6;
const uint16_t num_leds = stripLength * nStrips;
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, -1, 1, -1, 1, -1};
int directionIO[nStrips] = {1, -1, 1, -1, 1, -1};
int stripDirection[nStrips] = {1, -1, 1, -1, 1, -1};
uint16_t audienceSpot = 2;
uint16_t sweepSpot = 2;

MilliTimer buttonCheckTimer(200);
MilliTimer restoreNormalityTimer(7*1000*60);

uint8_t dimmerVal = 0;
uint8_t breatherVal = 0;
uint8_t frameRateVal = 0;
//uint8_t paletteChangeVal = 0;
uint8_t fadeRateVal = 0;
bool messagesAllowed = false;
bool newMessage = false;

Adafruit_ADS1115 ads(0x48);
//************************************************************

//************************************************************
//functions
void randomAnalogMessage(uint8_t dimmerVal, uint8_t breatherVal, uint8_t frameRateVal, uint8_t fadeRateVal);
//************************************************************

void pleiadianSetup() {
    ads.begin();
}

void checkInputs() {
  if (buttonCheckTimer.isItTime()){
    uint16_t maxVal = 26000;
    uint16_t adc0 = min(maxVal, ads.readADC_SingleEnded(0));  // we read from the ADC, we have a sixteen bit integer as a result
    uint16_t adc1 = min(maxVal, ads.readADC_SingleEnded(1));
    uint16_t adc2 = min(maxVal, ads.readADC_SingleEnded(2));
    uint16_t adc3 = min(maxVal, ads.readADC_SingleEnded(3));

    uint8_t m0 = map(adc0, 0, maxVal, 10, 0);
    uint8_t m1 = map(adc1, 0, maxVal, 10, 0);
    int m2 = map(adc2, 0, maxVal, 7, -3);//then clip values below 1 to give us a range of 1-7 with higher likelihood of selecting 1
    m2 = max(1, (int) m2);
    uint8_t m3 = map(adc3, 0, maxVal, 10, 0);
//    m3 = max(1, (int) m3);
//    Serial.printf("%d, %d, %d, %d\n", adc0, adc1, adc2, adc3);
    Serial.printf("%d, %d, %d, %d\n", m0, m1, m2, m3);
  
    if (m0 != dimmerVal) {
      dimmerVal = m0;
      mainState.dimmer.plength = dimmerVal;
      newMessage = true;
    }
    if (m1 != breatherVal) {
      breatherVal = m1;
      mainState.breathe.pspeed = map(breatherVal, 0, 10, 1, 60);
      if (breatherVal != 0) {
        mainState.breathe.enabled = true;
      } else {
        mainState.breathe.enabled = false;
      }
      newMessage = true;
    }
    if (m2 != frameRateVal) {
      frameRateVal = m2;
      stepRate = frameRateVal;
      newMessage = true;
    }
//    if (m3 != paletteChangeVal) {//Change to fadeRate
//      paletteChangeVal = m3;
//      uint32_t pInterval = map(paletteChangeVal, 0, 10, 8000, 5*60000);
////      Serial.println(pInterval);
//      paletteCycleTimer.setInterval(pInterval);
//      newMessage = true;
//    }
    if (m3 != fadeRateVal) {
      fadeRateVal = m3;
      fadeRate = fadeRateVal * 24 + 1;
      newMessage = true;
    }
//    if ((m0 == 2) && (m1 == 4) && (m2 == 1) && (m3 == 7)) {
//      Serial.println("messages Enabbled!!!!");
//      messagesAllowed = true;
//    }
//    if ((m0 == 8) && (m1 == 6) && (m2 == 9) && (m3 == 3)) {
//      Serial.println("messages disabbled!!!!");
//      messagesAllowed = false;
//      randomAnalogMessage(0, 0, 1, 10);
//    }

    if (messagesAllowed && newMessage) {
      randomAnalogMessage(dimmerVal, breatherVal, frameRateVal, fadeRateVal);
      Serial.println("message sent!!!!");
      newMessage = false;
      restoreNormalityTimer.startTimer();
    }

    if (restoreNormalityTimer.isItTime()) {
      randomAnalogMessage(0, 0, 1, 50);
      restoreNormalityTimer.stopTimer();
    }
    
    buttonCheckTimer.resetTimer();
  }
}

//void checkDial(uint8_t pin, uint8_t lastVal) {
//  uint8_t currentVal = map(ads.readADC_SingleEnded(pin), 0, 26250, 0, 255);
//  if (abs(currentVal - lastVal) > 2) {
//    return currentVal;
//  }
//  return lastVal;
//}

void randomAnalogMessage(uint8_t dimmerVal, uint8_t breatherVal, uint8_t frameRateVal, uint8_t fadeRateVal) {
  String raMsgTemplate = "{\"poMessageType\": 7, \"dimmerVal\": 0, \"breatherVal\": 0, \"frameRateVal\": 0, \"fadeRateVal\": 0, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(raMsgTemplate);


//  uint32_t pInterval = (10 * 60000) / (pow(paletteChangeVal, 1.77));
  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["poMessageType"] = 7;
  nm_root["dimmerVal"] = dimmerVal;
  nm_root["breatherVal"] = map(breatherVal, 0, 10, 1, 60);
  nm_root["frameRateVal"] = frameRateVal;
//  nm_root["paletteChangeVal"] = (10 * 60000) / (pow(paletteChangeVal, 1.77));
  nm_root["fadeRateVal"] = fadeRateVal;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}
  


