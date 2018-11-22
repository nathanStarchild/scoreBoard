//************************************************************
// Moonbase Code 2018
// Location: Caleano
// Shape: Stellated Tetrahedron
// LEDs: 6 x 2m strips
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
//    Serial.println(switchesValue, BIN);
    long sweepValue = checkTimedButton(SWEEP_BUTTON);
    if (sweepValue) {
      Serial.printf("sweepValue: %d\n", sweepValue);
      int8_t sweepInt = min((int) sweepValue/40, 127);
      Serial.printf("sweepInt: %d\n", sweepInt);
      sweepMessage(sweepInt, switchesValue);
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

