//************************************************************
// Moonbase Code 2018
// Location: Maia
// Shape: pyramid
// LEDs: 15 * 1.5m strips
// Interactivity: switch row + tripper trap
//************************************************************

//************************************************************
//includes
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;
const uint8_t nStrips = 7;
const uint16_t num_leds = stripLength * nStrips;

#define SWITCH4 12
#define SWITCH1 5
#define SWITCH2 4
#define SWITCH3 13
#define TRIPPERSWITCH 0
//************************************************************

//************************************************************
//Global variables
//wiring order: up one, around the octa loop, around the horizontal triangle, up to the top, all the way down, data ext across the base, then up to the top
int directionLR[nStrips] = {1, 1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, -1, 1, -1, 1, -1, 1};
int directionIO[nStrips] = {1, -1, 1, -1, 1, -1, 1};
int stripDirection[nStrips] = {1, -1, 1, -1, 1, -1, 1};
uint16_t audienceSpot = 0;
uint16_t sweepSpot = 0;

MilliTimer buttonCheckTimer(200);
MilliTimer primedTimer(10 * 1000);
MilliTimer goForLaunchTimer(10 * 1000); //time window to initiate launch with tripper trap switch after setting the prerequisites

bool rainVal = false;
bool blendWaveVal = false;
bool rippleVal = false;
bool antsVal = false;
bool tripperVal = false;
bool newMessage = false;
//************************************************************


void switchRowMessage(bool rainVal, bool  blendWaveVal, bool  rippleVal, bool antsVal);
void tripperMessage(bool switchVal, bool launchVal);
void tripperTrapMode();
void upset_mainState();
void addRipple();

void pleiadianSetup() {
  pinMode(SWITCH1,INPUT_PULLUP);
  pinMode(SWITCH2,INPUT_PULLUP);
  pinMode(SWITCH3,INPUT_PULLUP);
  pinMode(SWITCH4,INPUT_PULLUP);
  pinMode(TRIPPERSWITCH,INPUT_PULLUP);
  //pass
}

void checkInputs() {
  if (buttonCheckTimer.isItTime()) {
    bool sw1 = !digitalRead(SWITCH1);
    bool sw2 = !digitalRead(SWITCH2);
    bool sw3 = !digitalRead(SWITCH3);
    bool sw4 = !digitalRead(SWITCH4);
    bool swtt = !digitalRead(TRIPPERSWITCH);

    // to initiate launch sequence and open the portal to the sea of tranquility, the switches must first all be off. We are now primed for launch
    // When the firstz switch the operator has 10 seconds to turn all the switches, we are now go for launch.
    // then the operator has 10 seconds to switch on the tripper trap switch to initiate launch and we go to the moon!

    bool primed = !(rainVal || blendWaveVal || rippleVal || antsVal || TRIPPERSWITCH); 

    if (sw1 != rainVal) {
      rainVal = sw1;
      newMessage = true;
      Serial.printf("rain %d\n", rainVal);
      if (primed) {
        primedTimer.startTimer(); //you really should need to switch them on in order, but for now you just have to start witht the first one...
      }
    }
    if (sw2 != blendWaveVal) {
      blendWaveVal = sw2;
      newMessage = true;
      Serial.printf("blend %d\n", blendWaveVal);
    }
    if (sw3 != rippleVal) {
      rippleVal = sw3;
      newMessage = true;
      Serial.printf("ripple %d\n", rippleVal);
    }
    if (sw4 != antsVal) {
      antsVal = sw4;
      newMessage = true;
      Serial.printf("ants %d\n", antsVal);
    }
    if (newMessage){
      mainState.rain.enabled = rainVal;
      mainState.blendwave.enabled = blendWaveVal;
      mainState.ripple.enabled = rippleVal;
      if (rippleVal) {
        addRipple();
      }
      mainState.ants.enabled = antsVal;
      switchRowMessage(rainVal, blendWaveVal, rippleVal, antsVal);
      if (rainVal && blendWaveVal && rippleVal && antsVal && !primedTimer.isItTime()) {
        Serial.println("go for launch!");
        goForLaunchTimer.startTimer();
      }
      newMessage = false;
    }

    //Check Tripper Trap Switch
    if (swtt != tripperVal) {
      bool launchVal = false;
      tripperVal = swtt;
      if (!goForLaunchTimer.isItTime()) {
        launchVal = true;
        mainState.launch.enabled = true;
        Serial.println("LAUNCH!!!!!!!!!!!!!");
      } else if (tripperVal) {
        tripperTrapMode();
      } else {
        upset_mainState();
      }
      tripperMessage(tripperVal, launchVal);
      
      
    }
    
    buttonCheckTimer.resetTimer();
  }
}

void switchRowMessage(bool rainVal, bool  blendWaveVal, bool  rippleVal, bool antsVal) {
  String SRMsgTemplate = "{\"poMessageType\": 10,  \"rainVal\": LOW,  \"blendWaveVal\": LOW,  \"rippleVal\": LOW,  \"antsVal\": LOW,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(SRMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 10;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_root["rainVal"] = rainVal;
  nm_root["blendWaveVal"] = blendWaveVal;
  nm_root["rippleVal"] = rippleVal;
  nm_root["antsVal"] = antsVal;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

//void goForLaunchMessage() {
//  String launchMsgTemplate = "{\"poMessageType\": 16,  \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
//
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& nm_root = jsonBuffer.parseObject(launchMsgTemplate);
//
//  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
//  
//  mainState.lastUpdate = mainState.nextUpdate;
//  mainState.nextUpdate = gotime;
//  
//  nm_root["poMessageType"] = 16;
//  nm_root["timestart"] = gotime;
//  nm_root["hue"] = mainState.hue;
//  
//  nm_msgp.remove(0);
//  nm_root.printTo(nm_msgp);
//  outbox = true;
//}

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
