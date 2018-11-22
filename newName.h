//************************************************************
// Rachel Fest Code 2018
// Location: Caleano
// Shape: ?
// LEDs: ?
// Interactivity: Royal Sampler + Tranquility Override
//************************************************************

//************************************************************
//includes
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;
const uint8_t nStrips = 6;
const uint16_t num_leds = stripLength * nStrips;

#define TOGGLE_SWITCH 4
#define ROCKER_SWITCH 12
#define PUSH_SWITCH 13
#define SWEEP_BUTTON 5
#define TRANQUILITY_BUTTON 0
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, -1, 1, -1, 1, -1};
int directionIO[nStrips] = {1, -1, 1, -1, 1, -1};
int stripDirection[nStrips] = {1, -1, 1, -1, 1, -1};
uint16_t audienceSpot = 1;
uint16_t sweepSpot = 1;
uint8_t lastSwitchesVal = 0;

MilliTimer buttonCheckTimer(40);

MilliTimer paletteSyncTimer(21 * 60000);
//************************************************************

//************************************************************
//Functions
long checkTimedButton(uint8_t pin);
long checkTranquilityButton(uint8_t pin);
uint8_t readSwitches();
void sweepMessage(long sweepValue, uint8_t switchesValue);
void tranquilityMessage();
void tranquilityMode();
void nextPaletteMessage();
void rainbows();
void upset_mainState();
void tripperTrapMode();
void patternsOff();
void nextPalette();
void addRipple();
void enlightenmentAchieved();

void mapMessage(uint16_t b, uint8_t n);
void antsMessage();
void tooFlashyMessage();
void patternsOffMessage();
void tripperMessage(bool switchVal, bool launchVal);
//************************************************************

void pleiadianSetup() {
  pinMode(ROCKER_SWITCH,INPUT_PULLUP);
  pinMode(PUSH_SWITCH,INPUT_PULLUP);
  pinMode(SWEEP_BUTTON,INPUT_PULLUP);
  pinMode(TRANQUILITY_BUTTON,INPUT_PULLUP);
  pinMode(TOGGLE_SWITCH,INPUT_PULLUP);
}

void checkInputs() {
  if (buttonCheckTimer.isItTime()){
    uint8_t switchesValue = readSwitches();
    if(switchesValue != lastSwitchesVal){
      lastSwitchesVal = switchesValue;
      switch(switchesValue){
        
         case 0 : //no button pushed
            rainbows();
            break; 
         case 1 : //Button "0"   
            mainState.houseLights.enabled = true;            
//            houseLightsMessage();/
          break;
          case 2 : //Button "1"   
            tranquilityMode();
            tranquilityMessage();
          break;
          case 3 : //Button "2" upset_mainState()  
            upset_mainState();
            tripperMessage(false, false);
          break;
          case 4 : //Button "3"   
            tripperTrapMode();
            tripperMessage(true, false);
          break;
          case 5 : //Button "4"    
            nextPalette();
            nextPaletteMessage();
          break;
          case 6 : //Button "5"  
            patternsOff();
            mainState.ants.enabled = true;
            mainState.ants.plength = 20;
            mainState.ants.pspeed = 8;
            mainState.ants.decay = 150;
            stepRate = 1;
            fadeRate = 250; 
            antsMessage();
          break;
          case 7 : //Button "6" 
            mainState.breathe.enabled = false;
            mainState.tail.enabled = false;
            mainState.glitter.enabled = false;
            fadeRate = 70;  
             tooFlashyMessage();
          break;
          case 8 : //Button "7"  
            patternsOff();//this will trigger the ripple to be enabled   
      patternsOffMessage();
          break;
          case 9 : //Button "8"   
            patternsOff();//this will trigger the ripple to be enabled   
      patternsOffMessage();
          break;
          case 10 : //Button "9"   
            nextPalette();
            nextPaletteMessage();
          break;
          default :
          break;    
      }
    }
//    Serial.println(switchesValue, BIN);
    long buttonTime = checkTimedButton(SWEEP_BUTTON); 
    if (buttonTime) {
      if (buttonTime > 10) {
        Serial.println(buttonTime);
      }
      if (buttonTime > 10000) {
        Serial.println("tranquility");
        tranquilityMode();
      } else if (buttonTime > 6000) {
        Serial.println("houseLights");
        mainState.houseLights.enabled = !mainState.houseLights.enabled;
      } else if (buttonTime >= 2600) {//18
        Serial.println("tail");
        mainState.tail.plength = random(20, 120);
        mainState.tail.pspeed = (int) ((random(0, 30) - 15) / 3.0);
        if (mainState.tail.pspeed == 0) {
          mainState.tail.pspeed += 1;
        }
        Serial.printf("tail speed = %d, length = %d\n", mainState.tail.pspeed, mainState.tail.plength);
        mainState.tail.enabled = !mainState.tail.enabled;
      } else if (buttonTime >= 1800) { //13
        mainState.skaters.plength = random8(5);
        mainState.skaters.enabled = true;
        Serial.println("skaters");
      } else if (buttonTime >= 1200) {//7
        Serial.println("palette");
        nextPalette();
      } else if (buttonTime >= 600) {
        upset_mainState();
        Serial.println("uset_mainState()");
      }  else if (buttonTime >= 150) {
        Serial.println("addRipple");
        mainState.ripple.enabled = true;
        addRipple();
      }  else if (buttonTime > 1) {
        Serial.println("enlightenment");
        enlightenmentAchieved();
      }
    }
//    Serial.println("woop");
    long tranquilityValue = checkTranquilityButton(TRANQUILITY_BUTTON);
    if (tranquilityValue != 0) {//TODO long hold for complete override ie start tranquility timer
      Serial.println("tranquility");
      tranquilityMessage();
    }
    buttonCheckTimer.resetTimer();
  }
  if (paletteSyncTimer.isItTime()) {
    nextPaletteMessage();
    paletteCycleTimer.resetTimer();
    paletteSyncTimer.resetTimer();
  }
}

long checkTimedButton(uint8_t pin) {
  static int debounce = 25;          // ms debounce period to prevent flickering when pressing or releasing the button
  static long downTime = -1;         // time the button was pressed down
  static long upTime = -1;           // time the button was released
  static boolean buttonVal = HIGH;   // value read from button
  static boolean buttonLast = HIGH;  // buffered value of the button's previous state
  long holdTime = 0;
  long now = millis();
  buttonVal = digitalRead(pin);
  
  // Button pressed down
  if (buttonVal == LOW && buttonLast == HIGH && (now - upTime) > debounce) {
    downTime = now;
  }
  // Button released
  else if (buttonVal == HIGH && buttonLast == LOW && (now - downTime) > debounce) {
    upTime = now;
    holdTime = upTime - downTime;
    holdTime = min(holdTime, (long) 127*40);//max hold length in frames
  }
  buttonLast = buttonVal;
  return holdTime;
}


long checkTranquilityButton(uint8_t pin) {
  static int debounce = 25;          // ms debounce period to prevent flickering when pressing or releasing the button
  static long downTime = -1;         // time the button was pressed down
  static long upTime = -1;           // time the button was released
  static boolean buttonVal = HIGH;   // value read from button
  static boolean buttonLast = HIGH;  // buffered value of the button's previous state
  long holdTime = 0;
  long now = millis();
  buttonVal = digitalRead(pin);
  
  // Button pressed down
  if (buttonVal == LOW && buttonLast == HIGH && (now - upTime) > debounce) {
    downTime = now;
  }
  // Button released
  else if (buttonVal == HIGH && buttonLast == LOW && (now - downTime) > debounce) {
    upTime = now;
    holdTime = upTime - downTime;
    holdTime = min(holdTime, (long) 127*40);//max hold length in frames
  }
  buttonLast = buttonVal;
  return holdTime;
}

uint8_t readSwitches() {
    bool rs = !digitalRead(ROCKER_SWITCH); // ! = "not" for inverting active-high inputs.
    bool ps = !digitalRead(PUSH_SWITCH);
    bool ts = !digitalRead(TOGGLE_SWITCH);
    uint8_t switchVal = rs + (ps << 1) + (ts << 2);
    return switchVal;  
}

void rainbows() {
        mainState.mapPattern.enabled = true;
        mainState.mapPattern.plength = 7;
        mainState.mapPattern.pspeed = 10;
        mapMessage(32, 7);
}

void sweepMessage(long sweepValue, uint8_t switchesValue) {
  String sweepMsgTemplate = "{\"poMessageType\": 6,  \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0, \"plength\": 0, \"pspeed\": 0}";

  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(sweepMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 6;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["pspeed"] = sweepValue;
  nm_root["plength"] = switchesValue;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;

  mainState.sweep.enabled = true;
  mainState.sweep.plength = switchesValue;
  mainState.sweep.pspeed = sweepValue;
//  Serial.println(sweepValue);
  mainState.sweep.decay = 0;
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


void tripperMessage(bool switchVal, bool launchVal) {
  String tripperMsgTemplate = "{\"poMessageType\": 5,  \"switchVal\": LOW,  \"launchVal\": LOW,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(tripperMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  if (launchVal) {
    gotime += INTERVAL;
  }
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 5;
  nm_root["audience"] = 0xff;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_root["switchVal"] = switchVal;
  nm_root["launchVal"] = launchVal;
  
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


void mapMessage(uint16_t b, uint8_t n) { //send the data from the enlightenment button
String mapMsgTemplate = "{\"poMessageType\": 2,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
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
  nm_root["pspeed"] = 10;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

void antsMessage(){
  String antsMsgTemplate = "{\"poMessageType\": 16, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(antsMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 16;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

void tooFlashyMessage(){
  String tooFlashyMsgTemplate = "{\"poMessageType\": 17, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(tooFlashyMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 17;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

void patternsOffMessage(){
  String patternsOffMsgTemplate = "{\"poMessageType\": 18, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(patternsOffMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 18;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

