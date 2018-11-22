//************************************************************
// LUNE:Aquarius Code 2018
// Location: There's only 1
// Shape: waves
// LEDs: 4 x 2m strips
// Interactivity: Tripper Trap mode + secret switches
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;
const uint8_t nStrips = 5;
const uint16_t num_leds = stripLength * nStrips;

#define TRIPPER_SWITCH 12
#define SWITCH1 5
#define SWITCH2 4
#define SWITCH3 13
#define BUTTON 0
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, -1, 1, -1, 1};
int directionIO[nStrips] = {1, -1, 1, -1, 1};
int stripDirection[nStrips] = {1, -1, 1, -1, 1};
uint16_t audienceSpot = 0;
uint16_t sweepSpot = 0;

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
void tripperTrapMode();
void upset_mainState();
void holdingPatternMode(uint8_t n);
void tranquilityMode();
void nextPalette();
void patternsOff();
void enlightenmentAchieved();
void addRipple();
void ripplesOff();
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
    if (true) {
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

