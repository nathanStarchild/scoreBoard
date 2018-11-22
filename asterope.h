//************************************************************
// Moonbase Code 2018
// Location: Asterope
// Shape: Guardian Costue
// LEDs: 2 x 72 LED strips
// Interactivity: 
//************************************************************

//************************************************************
//includes
//#include "WiiChuck.h"
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 72;//57
const uint8_t nStrips = 2;//6
const uint16_t num_leds = stripLength * nStrips;
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {1, 1};
int directionUD[nStrips] = {1, -1};
int directionIO[nStrips] = {-1, 1};
int stripDirection[nStrips] = {1, -1};

uint16_t audienceSpot = 7;
uint16_t sweepSpot = 7;

//MilliTimer chuck_button(150); //debounce time

//WiiChuck chuck = WiiChuck();
//************************************************************

//************************************************************
//functions
//void pleiadianSetup();
//void checkInputs();
//void tailMessage(int pspeed, int plength);
//void rainMessage(int pspeed, int plength);
//void tripperTrapMode();
//void upset_mainState();

//************************************************************

void pleiadianSetup() {
//    chuck.begin();
//    chuck.update();
//    chuck.calibrateJoy();
}

void checkInputs() {
////  Serial.println("checking");
//  if (chuck_button.isItTime()){
//    chuck.update();
////    Serial.println(chuck.buttonZ);
//    if (chuck.buttonZ) {
//      if (chuck.buttonC) {
//        if (tripperTrapTimer.isRunning()) {
//          upset_mainState();
//        } else 
//          tripperTrapMode();
//      } else {
//        if (chuck.rightJoy()) {
//          mainState.dimmer.pspeed = min(10, mainState.dimmer.pspeed + 1);
//        }
//        if (chuck.leftJoy()) {
//          mainState.dimmer.pspeed = max(0, mainState.dimmer.pspeed - 1);
//        }
//        int pspeed = chuck.readRoll();// /30
//        pspeed = map(pspeed, -180, 180, -10, 10);
//  //      pspeed = chuck.readRoll() / 30;
//        int plength = chuck.readPitch();
//        if ((plength > 180) || (plength < 10)) {
//          plength = 10;
//        }
//  //      plength = random(20,120);
//        mainState.rain.enabled = false;
//        mainState.tail.enabled = true;
//        mainState.tail.pspeed = pspeed;
//        mainState.tail.plength = plength;
//        Serial.printf("tail speed: %d, length: %d\n", pspeed, plength);
//        tailMessage(pspeed, plength);
//      }
//    } else if (chuck.buttonC) {
//      int pspeed = chuck.readRoll();
//      pspeed = map(pspeed, -180, 180, 10, 30);
//      int plength = chuck.readPitch();
//      if ((plength > 180) || (plength < 1)) {
//        plength = 1;
//      }
//      plength = map(plength, 1, 180, 10, 30);
//      Serial.printf("rain speed: %d, rain length: %d\n", pspeed, plength);
//      
//      mainState.tail.enabled = false;
//      mainState.rain.enabled = true;
//      mainState.rain.pspeed = pspeed;
//      mainState.rain.plength = plength;
//      rainMessage(pspeed, plength);
//    }
//    chuck_button.resetTimer();
//  }
}


//
//void tailMessage(int pspeed, int plength){
//  String tailMsgTemplate = "{\"poMessageType\": 11,  \"pspeed\": 0,  \"plengthl\": 0, \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
// 
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& nm_root = jsonBuffer.parseObject(tailMsgTemplate);
//
//  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
//  
//  mainState.lastUpdate = mainState.nextUpdate;
//  mainState.nextUpdate = gotime;
//  
//  nm_root["poMessageType"] = 11;
//  nm_root["timestart"] = gotime;
//  nm_root["hue"] = mainState.hue;
//  nm_root["patternStep"] = mainState.patternStep;
//  
//  
//  nm_root["pspeed"] = pspeed;
//  nm_root["plength"] = plength;
//  
//  nm_msgp.remove(0);
//  nm_root.printTo(nm_msgp);
//  outbox = true;
//}
//
//void rainMessage(int pspeed, int plength) {
//  String rainMsgTemplate = "{\"poMessageType\": 12,  \"pspeed\": 0,  \"plengthl\": 0, \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
// 
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& nm_root = jsonBuffer.parseObject(rainMsgTemplate);
//
//  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
//  
//  mainState.lastUpdate = mainState.nextUpdate;
//  mainState.nextUpdate = gotime;
//  
//  nm_root["poMessageType"] = 12;
//  nm_root["timestart"] = gotime;
//  nm_root["hue"] = mainState.hue;
//  nm_root["patternStep"] = mainState.patternStep;
//  
//  
//  nm_root["pspeed"] = pspeed;
//  nm_root["plength"] = plength;
//  
//  nm_msgp.remove(0);
//  nm_root.printTo(nm_msgp);
//  outbox = true;
//}

