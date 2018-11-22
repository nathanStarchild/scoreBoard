//************************************************************
// Moonbase Code 2018
// Location: Pleione
// Shape: stellated Octahedron
// LEDs: 12 x 2m strips
// Interactivity: palette designer
//TODO: can cycle through existing palettes;
//************************************************************

//************************************************************
//includes
#include <Wire.h>
#include "Adafruit_MCP23017.h"
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
//wiring order: go up, then around the top, then the rest of the loop
int directionLR[nStrips] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, 0, 0, 0, 0, 0, 0, -1, 1, -1, 1, -1};
int directionIO[nStrips] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1};
int stripDirection[nStrips] = {1, 0, 0, 0, 0, 0, 0, -1, 1, -1, 1, -1};
uint16_t audienceSpot = 4;
uint16_t sweepSpot = 4;

MilliTimer buttonCheckTimer(150);
MilliTimer paletteActiveTimer(60000);
MilliTimer newIndexTimer(500);

uint8_t paletteIndex = 0;
//CRGB designColour = CRGB::Black;
//CRGBPalette16 currentDesignPalette;
//CRGBPalette16 targetDesignPalette;
uint8_t indexButtonsMask = 0xf;
uint8_t activateButtonMask = 0x1 << 4;
uint8_t broadcastButtonMask = 0x1 << 5;
boolean flashToggle = true;
uint8_t idxCounter = 0;

Adafruit_MCP23017 mcp;
Adafruit_ADS1115 ads(0x48);
//************************************************************

//************************************************************
//functions
void pleiadianSetup();
void checkInputs();
uint8_t getActivateButtonVal(uint8_t buttons);
uint8_t getPaletteIndex(uint8_t buttons);
CRGB getCurrentColour();
long getBroadcastButtonVal(uint8_t buttons);
void lightTheButtons(uint8_t buttons);
void paletteMessage();
void nextPaletteMessage();
void upset_mainState();
void nextPalette();
void patternsOff();
uint8_t arrayFromPalette(CRGBPalette16 thePalette);
void tranquilityMessage();
void tranquilityMode();
//************************************************************

void pleiadianSetup() {
    mcp.begin();      // use default address 0 
    ads.begin();

    Wire.beginTransmission(0x20);
    Wire.write(0x00); // IODIRA register
    Wire.write(0x00); // set all of port A to outputs
    Wire.endTransmission();
    Wire.beginTransmission(0x20);
    Wire.write(0x0D); // register
    Wire.write(0xFF); // set all of port B to pullup
    Wire.endTransmission();

    currentDesignPalette = OceanColors_p;
//    targetDesignPalette = OceanColors_p;
}

void checkInputs() {
  static uint8_t lastIndex = 0;
  static CRGB lastColour = CRGB::Black;
  static long lastBroadcastVal = 0;
  static bool changed[16];
  
  if (buttonCheckTimer.isItTime()) {
    if (paletteActiveTimer.isItTime()) {
      mainState.paletteDisplay.enabled = false;
      paletteActiveTimer.stopTimer();
//      Serial.printf("%s\n", "here we stop");
      upset_mainState();
    }
    
    uint8_t buttons = ~mcp.readGPIOAB() >> 8; //the ~ flips everything since the chip is active low logic
//    Serial.printf("buttons: %d\nactivateButtonMask: %d\nactivateVal: %d\nbroadcastButtonMask: %d\nbroadcastVal: %d", buttons, activateButtonMask, buttons & activateButtonMask, broadcastButtonMask, buttons & broadcastButtonMask);
    uint8_t activateButtonVal = getActivateButtonVal(buttons);
    long broadcastButtonVal = getBroadcastButtonVal(buttons);
//    Serial.println(broadcastButtonVal);
      if ((broadcastButtonVal  != 0) && !(paletteActiveTimer.isRunning())) {
        Serial.println(broadcastButtonVal);
        offline_pattern.resetTimer();
        if (broadcastButtonVal > 10*1000) {
          tranquilityMode();
          tranquilityMessage();
        } else {
          nextPalette();
          nextPaletteMessage();
  //        Serial.printf("nextPalette\n");
        }
      }
    if (activateButtonVal  == 1) {
//      Serial.printf("%s\n", "here we go");
      offline_pattern.resetTimer();
      paletteActiveTimer.startTimer(); //startTimer() resets the timer if it's already running
      patternsOff();
      for (int i=0; i<16;i++) {
        changed[i] = false;
      }
    }
    if (paletteActiveTimer.isRunning()) {
//      patternsOff();//allow other messages to interrupt????
      mainState.paletteDisplay.enabled = true;
      paletteIndex = getPaletteIndex(buttons);
      if (paletteIndex != lastIndex) {
        lastIndex = paletteIndex;
        paletteActiveTimer.resetTimer();
        if (newIndexTimer.isRunning()) {
//          currentDesignPalette = targetDesignPalette;
        }
        newIndexTimer.startTimer();
        idxCounter = 0;
      }
      designColour = getCurrentColour();
      if (designColour != lastColour && !newIndexTimer.isRunning()) {//TODO account for small errors
        if (abs(designColour.r - lastColour.r) > 5 || abs(designColour.g - lastColour.g) > 5 || abs(designColour.b - lastColour.b) > 5) {
          paletteActiveTimer.resetTimer(); 
        }
        lastColour = designColour;
        currentDesignPalette[paletteIndex] = designColour;
        changed[paletteIndex] = true;
        for (int i=0; i<16; i++) {
          if (!changed[i]) {
            currentDesignPalette[i] = designColour;
            changed[i] = true;
          }
        }
//        currentDesignPalette = targetDesignPalette;
      }
//      uint8_t broadcastButtonVal = getBroadcastButtonVal(buttons);
      if (broadcastButtonVal != lastBroadcastVal) {
        lastBroadcastVal = broadcastButtonVal;
        if (broadcastButtonVal  != 0) {
          Serial.println(broadcastButtonVal);
          offline_pattern.resetTimer();
          if (broadcastButtonVal > 10*1000) {
            tranquilityMode();
            tranquilityMessage();
          } else {
            paletteMessage();
            targetPalette = currentDesignPalette;
  //          Serial.println("broadcasting palette");
  //          paletteActiveTimer.resetTimer();/ 
          }      
        }
      }
//      nblendPaletteTowardPalette(currentDesignPalette, targetDesignPalette, 150);
    }
    lightTheButtons(buttons);
    if (newIndexTimer.isRunning()) {//flash the lights to show what part is being edited
      if (flashToggle) {
        currentDesignPalette[paletteIndex] = CRGB::White;
      } else {
        currentDesignPalette[paletteIndex] = CRGB::Black;
      }
      if (newIndexTimer.isItTime()) {
        if (idxCounter == 6) {
          newIndexTimer.stopTimer();
//          targetDesignPalette[paletteIndex] = designColour;
//          currentDesignPalette = targetDesignPalette;
        } else {
          flashToggle = !flashToggle;
          newIndexTimer.resetTimer();
          idxCounter++; 
        }
      }
    }
    buttonCheckTimer.resetTimer();
  }
}

uint8_t getActivateButtonVal(uint8_t buttons){
//  Serial.println(buttons & activateButtonMask,BIN);
  if (buttons & activateButtonMask) {
    return 1;
  }
  return 0;
}

uint8_t getPaletteIndex(uint8_t buttons) {
//  Serial.println(buttons & indexButtonsMask);
  return buttons & indexButtonsMask;
}

CRGB getCurrentColour() {
  uint16_t adc0 = ads.readADC_SingleEnded(0);  // we read from the ADC, we have a sixteen bit integer as a result
  uint16_t adc1 = ads.readADC_SingleEnded(1);
  uint16_t adc2 = ads.readADC_SingleEnded(2);
//  int16_t adc3;

  uint8_t h = map(adc0, 0, 27000, 255, 0);
  uint8_t s = map(adc1, 0, 27000, 255, 0);
  uint8_t v = map(adc2, 0, 27000, 255, 0);
  
//  Serial.printf("r: %d; g: %d; b: %d\n", adc0, adc1, adc2);
//  Serial.printf("r: %d; g: %d; b: %d\n", r, g, b);
  
  return CHSV(h, s, v);
}

long getBroadcastButtonVal(uint8_t buttons) {
  static int debounce = 25;          // ms debounce period to prevent flickering when pressing or releasing the button
  static long downTime = -1;         // time the button was pressed down
  static long upTime = -1;           // time the button was released
  static boolean buttonVal = HIGH;   // value read from button
  static boolean buttonLast = HIGH;  // buffered value of the button's previous state
  long holdTime = 0;
  long now = millis();
  buttonVal = !(buttons & broadcastButtonMask);
//  Serial.printf("buttonVal: %d\n", buttonVal);
  
  // Button pressed down
  if (buttonVal == LOW && buttonLast == HIGH && (now - upTime) > debounce) {
    Serial.printf("pressed at %d\n", now);
    downTime = now;
  }
  // Button released
  else if (buttonVal == HIGH && buttonLast == LOW && (now - downTime) > debounce) {
    Serial.printf("released at %d\n", now);
    upTime = now;
    holdTime = upTime - downTime;
    Serial.printf("holdTime at %d\n", holdTime);
//    holdTime = min(holdTime, (long) 127*40);//max hold length in frames
  }
  buttonLast = buttonVal;
  return holdTime;

//  if (buttons & broadcastButtonMask) {//todo add the ebutton code, return 3 for long press
//    return 3;
//  }
//  return 0;
}

void lightTheButtons(uint8_t buttons) {
  if (paletteActiveTimer.isRunning()) {
    for (int i=0;i<4;i++) {
      if (buttons & (0x01 << i)){
        mcp.digitalWrite(i, HIGH);
      } else {
        mcp.digitalWrite(i, LOW);
      }
    }
    mcp.digitalWrite(4, LOW);
    mcp.digitalWrite(5, HIGH);
  } else {
    for (int i=0;i<4;i++) {
      mcp.digitalWrite(i, LOW);
    }
    mcp.digitalWrite(4, HIGH);
    mcp.digitalWrite(5, LOW);
  }
  
}

void paletteMessage() { //send palette
  String paletteMsgTemplate = "{\"poMessageType\": 4,  \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0, \"plength\": 0, \"pspeed\": 0}";//, \"newPalette\": 0}";

  //store the palette as an array
  uint8_t palArray[48] = {};
  for (int i=0; i<16; i++) {
    int j = i * 3;
    palArray[j] = currentDesignPalette[i].red;
    palArray[j+1] = currentDesignPalette[i].green;
    palArray[j+2] = currentDesignPalette[i].blue;
  }
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(paletteMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 4;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  JsonArray& palJson = nm_root.createNestedArray("newPalette");
//  JsonObject& palJson = jsonBuffer.createArray();
  for (int i=0; i<48; i++){
    palJson.add(palArray[i]);
  }
  nm_root["newPalette"] = palJson;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

void nextPaletteMessage() { //send palette
  String nextPaletteMsgTemplate = "{\"poMessageType\": 8, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0, \"paletteIndex\": 0}";//, \"newPalette\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(nextPaletteMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 8;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["paletteIndex"] = paletteCycleIndex;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

void tranquilityMessage() {
  String tranquilityMsgTemplate = "{\"poMessageType\": 7,  \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";

  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(tranquilityMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 15;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;

  tranquilityMode();  
}

