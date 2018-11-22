//************************************************************
// Moonbase Code 2018
// Location: Atlas
// Shape: Remote Control
// LEDs: 3 x 3m strips
// Interactivity: Tripper Trap mode + secret switches
//************************************************************

//************************************************************
//includes
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;//42
const uint8_t nStrips = 15;//1
const uint16_t num_leds = stripLength * nStrips;

#define TRIPPER_SWITCH 12
#define SWITCH1 5
#define SWITCH2 4
#define SWITCH3 13
#define BUTTON 0
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, -1, 1, 1, -1, 1, 1, -1, 1, 1, -1, 1, -1,1, -1};
int directionIO[nStrips] = {1, -1, 1, 1, -1, 1, 1, -1, 1, -1, 1, -1, 1, -1, 1};
int stripDirection[nStrips] = {1, -1, 1, 1, -1, 1, 1, -1, 1, 1, -1, 1, -1, 1, -1};
uint16_t audienceSpot = 7;
uint16_t sweepSpot = 7;

MilliTimer buttonCheckTimer(100);
MilliTimer mapTimer(2000);
MilliTimer holdingPatternTimer(20 * 60000);
//************************************************************

//************************************************************
//functions
void pleiadianSetup();
void checkInputs();
void checkTripperTrapswitch();
void checkSecret();
long checkTimedButton(uint8_t pin);
//void tripperMessage(bool switchVal);
//void mapMessage(uint16_t b, uint8_t n);
//void ctlMessage(bool switchVal, uint8_t audience);
//void holdingPatternMessage();
void tripperTrapMode();
void upset_mainState();
void holdingPatternMode(uint8_t n);
void tranquilityMode();
void nextPalette();
void patternsOff();
void enlightenmentAchieved();
//************************************************************


void pleiadianSetup() {
  pinMode(SWITCH1,INPUT_PULLUP);
  pinMode(SWITCH2,INPUT_PULLUP);
  pinMode(SWITCH3,INPUT_PULLUP);
  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(TRIPPER_SWITCH,INPUT);
  
  holdingPatternTimer.stopTimer();
//  buttonCheckTimer.startTimer();
}

void checkInputs() {
//  Serial.println("checkinputs");
  if (buttonCheckTimer.isItTime()) {
//    Serial.println("Checking");
    checkTripperTrapswitch();
    checkSecret();
    buttonCheckTimer.resetTimer();
  }
}

void checkTripperTrapswitch(){
  static bool lastVal = false;
  bool switchVal = digitalRead(TRIPPER_SWITCH);
//  Serial.printf("last: %s, current: %s\n",lastVal, switchVal);
//  Serial.println(lastVal, BIN);
//  Serial.println(switchVal, BIN);
  if (switchVal != lastVal) {
    lastVal = switchVal;
//    tripperMessage(switchVal);
    if (switchVal) {
      tripperTrapMode();
    } else {
      upset_mainState();//TODO copyme()
    }
  }
}

void checkSecret(){
//  Serial.println("checking Secret");
  static uint8_t audience = 1;
  static bool lastCtlVal = true;
  static bool lastHoldingVal = true;
  
  bool onSwitchVal = digitalRead(SWITCH1);
  bool ctlSwitchVal = digitalRead(SWITCH2);
  bool holdingSwitchVal = digitalRead(SWITCH3);
  long buttonTime = checkTimedButton(BUTTON);

//  Serial.println("on switch: " + onSwitchVal);
//  Serial.println("ctl Switch: " + ctlSwitchVal);
//  Serial.println("Holding Switch: " + holdingSwitchVal);

  if (holdingSwitchVal) {
    if (holdingSwitchVal != lastHoldingVal) {
      Serial.println("HoldingSwitch");
  //    holdingPatternMode(1);
       mainState.blendwave.enabled = true;
       lastHoldingVal = holdingSwitchVal;
    }
  } else {
    if (holdingSwitchVal != lastHoldingVal) {
       mainState.blendwave.enabled = false;
       lastHoldingVal = holdingSwitchVal;
    } 
  }

  if (ctlSwitchVal) {
    if (ctlSwitchVal != lastCtlVal) {
      Serial.println("ctlSwitch");
  //    holdingPatternMode(1);
       mainState.rain.enabled = true;
       lastCtlVal = ctlSwitchVal;
    }
  } else {
    if (ctlSwitchVal != lastCtlVal) {
       mainState.rain.enabled = false;
       lastCtlVal = ctlSwitchVal;
    } 
  }

//  if (onSwitchVal) { //on switch not working
    Serial.println(buttonTime);
    if (buttonTime > 10000) {
      Serial.println("tranquility");
      tranquilityMode();
    } else if (buttonTime > 6000) {
      Serial.println("holding pattern");
      holdingPatternMode(1);
    } else if (buttonTime >= 2000) {
      Serial.println("tail");
      mainState.tail.enabled = !mainState.tail.enabled;
    } else if (buttonTime >= 1200) {
      Serial.println("palette");
      nextPalette();
    } else if (buttonTime >= 600) {
      mainState.skaters.enabled = true;
      Serial.println("skaters");
    }  else if (buttonTime >= 200) {
      Serial.println("ripple");
      patternsOff();
      mainState.ripple.enabled = true;
    }  else if (buttonTime > 99) {
      Serial.println("enlightenment");
      enlightenmentAchieved();
    }
//  }
  
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
    Serial.println("down");
    downTime = now;
  }
  // Button released
  else if (buttonVal == HIGH && buttonLast == LOW && (now - downTime) > debounce) {
    Serial.println("up");
    upTime = now;
//    Serial.println(downTime);
//    Serial.println(upTime);
//    Serial.println(upTime - downTime);
    holdTime = upTime - downTime;
//    Serial.println(holdTime);
//    holdTime = min(holdTime, (long) 127*40);//max hold length in frames
  }
  buttonLast = buttonVal;
  return holdTime;
}

void tripperMessage(bool switchVal) { //send the data from the enlightenment button
  String tripperMsgTemplate = "{\"poMessageType\": 5,  \"switchVal\": LOW,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(tripperMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 5;
  nm_root["audience"] = 0xff;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_root["switchVal"] = switchVal;
  
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
//  mesh.sendBroadcast(nm_msgp);
  outbox = true;
}

void holdingPatternMessage() { //send the data from the enlightenment button
  String holdingMsgTemplate = "{\"poMessageType\": 14,  \"switchVal\": LOW,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(holdingMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 14;
  nm_root["audience"] = 0xff;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

void ctlMessage(bool switchVal, uint8_t audience) { //send the data from the enlightenment button
  String ctlMsgTemplate = "{\"poMessageType\": 13,  \"switchVal\": LOW,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(ctlMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 13;
  nm_root["audience"] = audience;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_root["enabled"] = switchVal;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

