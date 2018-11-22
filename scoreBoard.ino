//************************************************************
// Moonbase Code 2018
// Location: Burning Seed
//************************************************************

//The palettes are stopping the oled screen from working
//uncomment for zeus
//#define NO_PALETTES


//************************************************************
// Imports, shared
#include <painlessMesh.h>
#include "FastLED.h"
#include "millitimer.h"
#include "pleiadianPalettes.h"
#include "pleiadianPatterns.h"
//************************************************************

//************************************************************
//Compile time Constants, shared
#define   LED  2       // GPIO number of connected LED, ON ESP-12 IS GPIO2
#define DATA_PIN 14
#define CLOCK_PIN 3
#define FASTLED_ESP8266_D1_PIN_ORDER

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define INTERVAL 40000 //25fps
#define BORED_TIME 600000000 //10 mins if no new network message
const uint16_t enlightenTime = 60000; //ms

#define   MESH_SSID       "Moonbase"
#define   MESH_PASSWORD   "whoblocksthesunshine"
#define   MESH_PORT       1045

#define NUM_LEDS 745
#define NUM_LEDS_OUT 741
#define NUM_LEDS_CONTROLLER 4
//************************************************************

////************************************************************
////Compile time Constants, specific
//const uint8_t stripLength = 42;//42
//const uint8_t nStrips = 2;//1
//const uint16_t NUM_LEDS = stripLength * nStrips;
//#define   ENLIGHTEN_DURATION 1000 //enlightenment/crazytown duration in frames
//const uint16_t enlightenTime = 60000; //ms
////************************************************************

//************************************************************
//Global variables, shared
CRGB leds[NUM_LEDS];
CRGB ledsController[NUM_LEDS_CONTROLLER];
CRGB ledsOut[NUM_LEDS_OUT];
painlessMesh  mesh;

MilliTimer offline_pattern(3 * 60000); //1000 seconds
MilliTimer boredTimer(6 * 60000); //bored timer, change if no messages or controller input
MilliTimer glitterTimer(10000); //how long glitter runs for
MilliTimer enlightenment(enlightenTime); //enlightenment button hold time
MilliTimer paletteBlendTimer(100); //how often to perform palette blending steps when moving to new palette
MilliTimer paletteCycleTimer(5 * 60000); // how often to move to the next palette
MilliTimer testingTimer(10000);
MilliTimer tripperTrapTimer(5 * 60000); //how long to stay in tripper trap mode
MilliTimer batteryUpdateTimer(9*60000); //how long to wait before assuming the voltage screamer isn't coming back online

const float PHI = 1.61803398875;


bool calc_delay = false;
SimpleList<uint32_t> nodes;
uint32_t sendMessageTime = 0;

uint16_t aoIndex = 0;
uint16_t frameCount = 0;
uint8_t stepRate = 1;
uint8_t fadeRate = 100;
bool controllerEnabled = true;
bool messagingEnabled = true;
bool holdingPatternLockdown = false;
int launchProgress = 0;

int batteryHealth = 99;
bool screamerOnline = true;

bool alone = true;
bool inbox = false;
bool onFlag;
String msgp;
String nm_msgp;
bool outbox = false;
bool broadcastError = false;
//TODO add enlightenment to template
uint32_t last_message = 0;
uint32_t phonebook[] = {0, 0, 2134672332, 2134672332, 2134674826, 2134674826, 99, 99, 99, 99, 99, 99, 99, 99, 99};

// 3C83CC 3C8D8A
//odd numbers are children.

uint8_t primes[4] = {5, 3, 2, 1};
uint8_t sPrimes[9] = {2, 3, 5, 7, 11, 13, 17, 19, 23};

//************************************************************


//************************************************************
// load the code specific to this shape
//#include "aquariusControl.h"
//#include "pleiadian.h"
//#include "atlas.h"
//#include "pleione.h"
//#include "alcyone.h"
//#include "maia.h"/
//#include "merope.h"
//#include "elektra.h"
//#include "taygeta.h"
//#include "caleano.h"
//#include "asterope.h"
//#include "zeus.h"?/
//#include "hermes.h"
//#include "tomThumb.h"
//#include "rachelFest/.h"
#include "newName.h"//
//************************************************************

//Load any other files that need the above 
#include "rippler.h"

//Ripplers setup
//struct rippleStruct {//nope
//  bool enabled;
//  Rippler theRippler;
//}

const uint8_t nRipples = 10;
uint8_t nRipplesEnabled = 0;
Rippler ripples[nRipples];

void setup() {
  //************************************************************
  //Shared
  Serial.begin(9600);//moved to end of setup
  Serial.println("please");

  pinMode(LED, OUTPUT);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see startup messages
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  phonebook[0] = mesh.getNodeId(); //?
  phonebook[1] = mesh.getNodeId(); //?

  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(ledsOut, NUM_LEDS_OUT);
  randomSeed(analogRead(A0));

  currentBlending = LINEARBLEND;
  currentPalette = CloudColors_p;
  targetPalette = CloudColors_p;
  
   addRipple();
  //************************************************************

  //************************************************************
  //Specific

  pleiadianSetup();

  //************************************************************
  //
  //    Serial.begin(9600);
}

void loop() {

  mesh.update();

  uint32_t currentTime = mesh.getNodeTime();
  uint32_t cycleTime;

  //Pattern running

  if ( (int) mainState.nextUpdate - (int) currentTime < 0) {
    combineLeds();
    FastLED.show();
    mainState.lastUpdate = mainState.nextUpdate;
    mainState.nextUpdate = mainState.lastUpdate + INTERVAL;
    mainState.stale = true;
    //chuck.update();
  }

  if (inbox) {
    last_message = millis();
    processPOmessage();

//  Serial.println("about to ifdef");
  #ifdef OLED_SCREEN
//      Serial.println("about to call oledmsg()");
      oledmsg();
  #endif

    inbox = false;
  }

  if (mainState.stale) {
    updatePatterns(); //fill the buffer up with the next lot of patterns.
  }

  // run the blinking status light
  onFlag = true;
  cycleTime = currentTime % BLINK_PERIOD;
  for (uint8_t i = 0; i < (mesh.getNodeList().size() + 1); i++) {
    uint32_t onTime = BLINK_DURATION * i * 2;
    if (cycleTime > onTime && cycleTime < onTime + BLINK_DURATION)
      onFlag = false;
  }
  digitalWrite(LED, onFlag);

  mesh.update();
  if (inbox) {
    last_message = millis();
    processPOmessage();
  #ifdef OLED_SCREEN
      oledmsg();
  #endif
    inbox = false;
  }
  
  //pattern running
  currentTime = mesh.getNodeTime();
  if ( (int) mainState.nextUpdate - (int) currentTime < 0) {
    combineLeds();
    FastLED.show();
    mainState.lastUpdate = mainState.nextUpdate;
    mainState.nextUpdate = mainState.lastUpdate + INTERVAL;
    mainState.stale = true;
    //chuck.update
  }
  if (mainState.stale) {
    updatePatterns(); //fill the buffer up with the next lot of patterns.
  }

  housekeeping();
}

void housekeeping() {
  bool error;

  //delay calcs
  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }
  uint32_t currentTime = mesh.getNodeTime();

  if (glitterTimer.isItTime()) {// hmmmmm I dunno.
    mainState.glitter.enabled = false;
    glitterTimer.stopTimer();
  }

  if (mesh.getNodeList().size() < 1 && !alone) { //check if we are in radio silence
    alone = true;
    offline_pattern.resetTimer();
  }

  //TODO offline_pattern timer
//  if (alone && offline_pattern.isItTime() && !holdingPatternLockdown) { //solo mode pattern changes
//  //    bulbState();
//    upset_mainState();
//    offline_pattern.resetTimer();
//  }

  //  if (!alone && (int32_t) last_message + BORED_TIME - (int32_t) millis() < 0) {//comparing microseconds and milliseconds
//  if (!alone && (int32_t) last_message + BORED_TIME - currentTime < 0 && !holdingPatternLockdown) {//do something new if new network instructions received for a while
//    upset_mainState();
//  }

  if (boredTimer.isItTime()  && !holdingPatternLockdown) {//
    //send a broadcast pattern message every so often
    upset_mainState();
//    copyme_message();
//    shout.setInterval(random(5, 20) * 60000 * 9);
    boredTimer.resetTimer();
  }

  if (paletteCycleTimer.isItTime()) {
    nextPalette();
    paletteCycleTimer.resetTimer();
  }

  if (paletteBlendTimer.isItTime()) {//blend to the new palette palette
    uint8_t maxChanges = 48;
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
    paletteBlendTimer.resetTimer();
  }

  //  if (testingTimer.isItTime()) {//blend to the new palette palette
  //    static int testingCounter = 1;
  //    static int cnt2 = 1;
  //    Serial.printf("length: %d; speed: %d\n", cnt2, testingCounter);
  //    patternsOff();
  //    mainState.hue=0;
  //    mainState.patternStep=0;
  //
  //    mainState.wave.enabled = true; //wave is now for tripper trap mode only
  //  //  mainState.wave.plength = random8(1,10);
  //  //  mainState.wave.pspeed = (int) random(0,10) - 5;
  //    mainState.wave.plength = cnt2;
  //    mainState.wave.pspeed = testingCounter;
  //    testingCounter++;
  //    testingCounter = testingCounter % 10;
  //    if (testingCounter == 0) {
  //      testingCounter++;
  //      cnt2++;
  //    }
  //    testingTimer.resetTimer();
  //  }

//  if (testingTimer.isItTime()) {
//    patternsOff();
//    controllerEnabled = false;
////    upset_mainState();
////    mainState.ants.enabled = true;
////    mainState.ants.pspeed = ((random8(0, 33)) % 21) + 1;
////    mainState.ants.plength = random8(1, 57);
//
//    Serial.printf("pspeed: %d, plength: %d\n", mainState.ants.pspeed, mainState.ants.plength);
//
//  
//    testingTimer.resetTimer();
//  }
  

  manageBatteryHealth();
  
  #ifdef ALWAYS_INPUTS
      controllerEnabled = true;
      messagingEnabled = true;
  #endif
  
  if (controllerEnabled) {
  //    Serial.println("checking from housekeeping");
    checkInputs();
  }

  if (tripperTrapTimer.isItTime()) {
    upset_mainState();
    tripperTrapTimer.stopTimer();
  }

  if (nothingIsOn()) {
    shootingStars();
  }

  if (outbox && messagingEnabled) {
    broadcastError = mesh.sendBroadcast(nm_msgp);
  }
  outbox = false;
}

void proveMyHypothesis() {
//  static const uint8_t n = 4;
  static double p[4];
  static int counter[4];
  static uint8_t vals[4] = {50, 100, 150, 200};
  static uint8_t rna[4];
  static int trials = 0;

  for (int i=0; i<4; i++) {
    rna[i] = random8();
    if (rna[i] > vals[i]) {
      counter[i]++;
    }
  }
  trials++;
  if (trials % 1000 == 0) {
    Serial.printf("\n\nafter %d trials:\n", trials);
    for (int i=0; i<4; i++) {
      Serial.printf("outcome: %d, expected: %d\n", counter[i], (float)(trials * (255-vals[i]))/((float)256.0));
    }
  }
}

void manageBatteryHealth() {
  static int lastHealth = 99;
  if (batteryUpdateTimer.isItTime()) { //no battery message for a 9 minutes. The screamer is offline.
    batteryUpdateTimer.resetTimer();
    
    #ifdef OLED_SCREEN
      warnScreamerOffline();
    #endif
    screamerOnline = false;
    //12 hours sunset-sunrise = 720 mins <-> 80% battery
    // => 9 mins = 1% battery;
    batteryHealth -= 1;
    Serial.printf("Screamer offline, battery health estimate: %d\n", batteryHealth);
  }
  if (batteryHealth != lastHealth) {
    if (batteryHealth < 50 && lastHealth >= 50) { //battery has gone below 50%, start saving power
      mainState.powerSaver.enabled = true;
      mainState.powerSaver.plength = 1;
    }
    if (batteryHealth < 30 && lastHealth >= 40) { //battery has gone below 30%, save more power
      mainState.powerSaver.enabled = true;
      mainState.powerSaver.plength = 2;
    }
    if (batteryHealth < 15 && lastHealth >= 30) { //shit got real. Lockdown in holding pattern.
      holdingPatternMode(1);
    }
    //but what if the screamer went offline and now it's back to correct the record
    if (batteryHealth > 15 && lastHealth <= 30) { //shit didn't real. unlock holding pattern.
      holdingPatternMode(0);
      mainState.powerSaver.enabled = true;
      mainState.powerSaver.plength = 2;
    }
    if (batteryHealth > 30 && lastHealth <= 40) { //battery isn't below 30%, cancel saving more power
      mainState.powerSaver.enabled = true;
      mainState.powerSaver.plength = 1;
    }
    if (batteryHealth > 50 && lastHealth <= 50) { //battery isn't below 50%, cancel power saving
      mainState.powerSaver.enabled = false;
    }
    lastHealth = batteryHealth;
  }
}

void combineLeds() {
  int num_leds_controller = NUM_LEDS_CONTROLLER;
  if (audienceSpot == 7) { //no controller lights for the guardian costume
    num_leds_controller = 0;
  }
  for (int i=0; i<NUM_LEDS_OUT; i++) {
    if (i < num_leds_controller) {
      ledsOut[i] = ledsController[i];
    } else {
      ledsOut[i] = leds[i - num_leds_controller];
    }
  }
}

void upset_mainState() {
  //randomise wave, tail, breathe, hue=0, patternStep=0
  //add faderate
  nextPalette();
  fadeRate = random8(10, 240);
  stepRate = max(1, random8(0, 15) - 10);

  mainState.hue = 0;
  mainState.patternStep = 0;

  mainState.wave.enabled = false; //wave is now for tripper trap mode only
  mainState.wave.plength = (random8() > 100);
  // mainState.wave.pspeed = (int) random(0,10) - 5;
  mainState.wave.pspeed = 1;

  mainState.tail.enabled = (random8() > 200);
  mainState.tail.plength = random(20, 120);
  mainState.tail.pspeed = random(0, 6) - 3;
  if (mainState.tail.pspeed == 0) {
    mainState.tail.pspeed = 1;
  }

  //  mainState.breathe.enabled = (random8() > 50);
    mainState.breathe.enabled = false;
  mainState.breathe.plength = random8(50, 240);
  //  mainState.breathe.pspeed = (int) random(10,30);
  mainState.breathe.pspeed = random8(10, 30);

  mainState.glitter.enabled = (random8() > 220);
  //  mainState.glitter.enabled = false;
  mainState.glitter.plength = random8(50, 255);
  mainState.glitter.pspeed = (int) random(0, 2) - 1;

  mainState.rain.enabled = (random8() > 100);
  mainState.rain.plength = random8(3, 30);
  mainState.rain.pspeed = random8(10, 50);
  //  mainState.rain.enabled = false;

  mainState.ripple.enabled = (random8() > 160);
  //  mainState.ripple.enabled = false;

  mainState.blendwave.enabled =  (random8() > 200);
  //  mainState.blendwave.enabled =  false;

//  mainState.holdingPattern.enabled =  false;

  mainState.ants.enabled = (random8() > 200);
  mainState.ants.pspeed = ((random8(0, 33)) % 21) + 1;
  mainState.ants.plength = random8(1, 57);

//  if (random8() > 210) {
//    tripperTrapMode();
//  }

//  if (random8() > 230) {
//  //    enlightenmentAchieved();
//  }
}

void bulbState() {
  //randomise wave, tail, breathe, hue=0, patternStep=0
  nextPalette();
  patternsOff();

  mainState.hue = 0;
  mainState.patternStep = 0;

  mainState.wave.enabled = false; //wave is now for tripper trap mode only

  mainState.tail.enabled = (random8() > 100);
  mainState.tail.plength = random(20, 120);
  mainState.tail.pspeed = (int) (random(0, 30) - 15) / 12;

  mainState.breathe.enabled = false;

  mainState.glitter.enabled = (random8() > 210);
  mainState.glitter.plength = random8();
  mainState.glitter.pspeed = (int) random(0, 2) - 1;

  mainState.rain.enabled = (random8() > 100);
  mainState.rain.plength = random8(3, 30);

  mainState.ripple.enabled = (random8() > 160);

  mainState.blendwave.enabled =  (random8() > 240);

  mainState.holdingPattern.enabled = (random8() > 180);
  //choose a new targetPalette
}

void holdingPatternMode(uint8_t n) {
  if (n == 0) {
    upset_mainState();
    holdingPatternLockdown = false;
    controllerEnabled = true;
  } else {
    //go into holding pattern, disable all other patterns
    mainState.patternStep = 0;
    patternsOff();
    stepRate = 1;
    mainState.holdingPattern.enabled = true;
    holdingPatternLockdown = true;
    controllerEnabled = false;
  }
}

void tripperTrapMode() {
  //wave, tail, breathe, hue=0, patternStep=0
  patternsOff();
  mainState.hue = 0;
  mainState.patternStep = 0;

  mainState.wave.enabled = true; //wave is now for tripper trap mode only
  mainState.wave.plength = random8(1, 5);
  mainState.wave.pspeed = random8(1, 10);


  mainState.tail.enabled = true;
  mainState.tail.plength = random(20, 120);
  mainState.tail.pspeed = (int) (random(0, 30) - 15) / 12;

  mainState.breathe.enabled = (random8() > 50);
  mainState.breathe.plength = random8(50, 240);
  mainState.breathe.pspeed = random8(10, 30);

  tripperTrapTimer.startTimer();
}

void tranquilityMode() {
  patternsOff();
  targetPalette = OceanColors_p;
  paletteCycleIndex = 0;
  mainState.rain.enabled = true;
  mainState.rain.decay = 100;
  mainState.rain.pspeed = 50;
  mainState.rain.plength = 10;
  stepRate = 3;
  fadeRate = 50;
}

void shootingStars() {
  patternsOff();
  mainState.ripple.enabled = true;
  stepRate = 1;
  fadeRate = 50;
}

void powerSavingMode(uint8_t n) {
  
}

//void warnScreamerOffline() {
//  
//}

void patternsOff() {
  //turn off all the patterns, turn something on after calling it!
  mainState.wave.enabled = false;
  mainState.tail.enabled = false;
  mainState.breathe.enabled = false;
  mainState.glitter.enabled = false;
  mainState.crazytown.enabled = false;
  mainState.enlightenment.enabled = false;
  mainState.ripple.enabled = false;
  mainState.blendwave.enabled = false;
  mainState.rain.enabled = false;
  mainState.holdingPattern.enabled = false;
  mainState.mapPattern.enabled = false;
  mainState.paletteDisplay.enabled = false;
  mainState.sweep.enabled = false;
  mainState.skaters.enabled = false;
  mainState.ants.enabled = false;
  mainState.houseLights.enabled = false;
}

bool nothingIsOn() {
  return !(
    mainState.wave.enabled ||
    mainState.tail.enabled ||
    mainState.breathe.enabled ||
    mainState.glitter.enabled ||
    mainState.crazytown.enabled ||
    mainState.enlightenment.enabled ||
    mainState.ripple.enabled ||
    mainState.blendwave.enabled ||
    mainState.rain.enabled ||
    mainState.holdingPattern.enabled ||
    mainState.mapPattern.enabled ||
    mainState.paletteDisplay.enabled ||
    mainState.sweep.enabled ||
    mainState.skaters.enabled ||
    mainState.ants.enabled || 
    mainState.houseLights.enabled ||
    mainState.launch.enabled
  );
}

void nextPalette() {
  paletteCycleIndex = (paletteCycleIndex + 1) % nPalettes;
  targetPalette = cyclePalettes[paletteCycleIndex];
  Serial.printf("palette: %d\n", paletteCycleIndex);
}

void addRipple() {
  Serial.println("adding ripple");
  if (nRipplesEnabled < nRipples) {
    nRipplesEnabled++;
    for (int i=0; i<nRipplesEnabled; i++) {
      ripples[i].enable();
    }
  } else {
    ripplesOff();
  }
}

void ripplesOff() {
  for (int i=0; i<nRipples; i++) {
    ripples[i].disable();
  }
  nRipplesEnabled = 0;
  addRipple(); //keep one ripple on
}

void copyme_message() { //use the current pattern state to create a network message

  String patternMsgTemplate = "{\"poMessageType\": 1, \"sender\": \"jsonTEST\",  \"audience\": 0xFF,  "
    "\"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0, \"stepRate\": 1, \"fadeRate\": 100, \"paletteIndex\": 1, " 
    "\"wave\": {\"enabled\": false, \"speed\": 10, \"length\": 5, \"decay\": 6}, "
    "\"tail\": {\"enabled\": false, \"speed\": 10, \"length\": 7, \"decay\": 8}, "
    "\"breathe\": {\"enabled\": false, \"speed\": 9, \"length\": 10, \"decay\": 10}, "
    "\"glitter\": {\"enabled\": false, \"speed\": 9, \"length\": 10, \"decay\": 10}, "
    "\"crazytown\": {\"enabled\": false, \"speed\": 9, \"length\": 10, \"decay\": 10}, "
    "\"ripple\": {\"enabled\": false, \"speed\": 9, \"length\": 10, \"decay\": 10}, "
    "\"blendwave\": {\"enabled\": false, \"speed\": 9, \"length\": 10, \"decay\": 10}, "
    "\"rain\": {\"enabled\": false, \"speed\": 9, \"length\": 10, \"decay\": 10}, "
    "\"holdingPattern\": {\"enabled\": false, \"speed\": 9, \"length\": 10, \"decay\": 10}, "
    "\"mapPattern\": {\"enabled\": false, \"speed\": 0, \"length\": 10, \"decay\": 0}, "
    "\"paletteDisplay\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"sweep\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"dimmer\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"skaters\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"poleChaser\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"powerSaver\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"ants\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"launch\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1}, "
    "\"houseLights\": {\"enabled\": false, \"speed\": 1, \"length\": 1, \"decay\": 1} "
    "}";


  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(patternMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;

  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;

  nm_root["sender"] = mesh.getNodeId();
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["stepRate"] = stepRate;
  nm_root["fadeRate"] = fadeRate;
  nm_root["paletteIndex"] = paletteCycleIndex;

  nm_root["wave"]["enabled"] = mainState.wave.enabled;
  nm_root["wave"]["speed"] = mainState.wave.pspeed;
  nm_root["wave"]["length"] = mainState.wave.plength;
  nm_root["wave"]["decay"] = mainState.wave.decay;

  nm_root["tail"]["enabled"] = mainState.tail.enabled;
  nm_root["tail"]["speed"] = mainState.tail.pspeed;
  nm_root["tail"]["length"] = mainState.tail.plength;
  nm_root["tail"]["decay"] = mainState.tail.decay;

  nm_root["breathe"]["enabled"] = mainState.breathe.enabled;
  nm_root["breathe"]["speed"] = mainState.breathe.pspeed;
  nm_root["breathe"]["length"] = mainState.breathe.plength;
  nm_root["breathe"]["decay"] = mainState.breathe.decay;

  nm_root["glitter"]["enabled"] = mainState.glitter.enabled;
  nm_root["glitter"]["speed"] = mainState.glitter.pspeed;
  nm_root["glitter"]["length"] = mainState.glitter.plength;
  nm_root["glitter"]["decay"] = mainState.glitter.decay;

  nm_root["crazytown"]["enabled"] = mainState.crazytown.enabled;
  nm_root["crazytown"]["speed"] = mainState.crazytown.pspeed;
  nm_root["crazytown"]["length"] = mainState.crazytown.plength;
  nm_root["crazytown"]["decay"] = mainState.crazytown.decay;

  nm_root["ripple"]["enabled"] = mainState.ripple.enabled;
  nm_root["ripple"]["speed"] = mainState.ripple.pspeed;
  nm_root["ripple"]["length"] = mainState.ripple.plength;
  nm_root["ripple"]["decay"] = mainState.ripple.decay;

  nm_root["blendwave"]["enabled"] = mainState.blendwave.enabled;
  nm_root["blendwave"]["speed"] = mainState.blendwave.pspeed;
  nm_root["blendwave"]["length"] = mainState.blendwave.plength;
  nm_root["blendwave"]["decay"] = mainState.blendwave.decay;

  nm_root["rain"]["enabled"] = mainState.rain.enabled;
  nm_root["rain"]["speed"] = mainState.rain.pspeed;
  nm_root["rain"]["length"] = mainState.rain.plength;
  nm_root["rain"]["decay"] = mainState.rain.decay;

  nm_root["holdingPattern"]["enabled"] = mainState.holdingPattern.enabled;
  nm_root["holdingPattern"]["speed"] = mainState.holdingPattern.pspeed;
  nm_root["holdingPattern"]["length"] = mainState.holdingPattern.plength;
  nm_root["holdingPattern"]["decay"] = mainState.holdingPattern.decay;
  
  nm_root["mapPattern"]["enabled"] = mainState.mapPattern.enabled;
  nm_root["mapPattern"]["speed"] = mainState.mapPattern.pspeed;
  nm_root["mapPattern"]["length"] = mainState.mapPattern.plength;
  nm_root["mapPattern"]["decay"] = mainState.mapPattern.decay;
  
  nm_root["paletteDisplay"]["enabled"] = mainState.paletteDisplay.enabled;
  nm_root["paletteDisplay"]["speed"] = mainState.paletteDisplay.pspeed;
  nm_root["paletteDisplay"]["length"] = mainState.paletteDisplay.plength;
  nm_root["paletteDisplay"]["decay"] = mainState.paletteDisplay.decay;
  
  nm_root["sweep"]["enabled"] = mainState.sweep.enabled;
  nm_root["sweep"]["speed"] = mainState.sweep.pspeed;
  nm_root["sweep"]["length"] = mainState.sweep.plength;
  nm_root["sweep"]["decay"] = mainState.sweep.decay;
  
  nm_root["dimmer"]["enabled"] = mainState.dimmer.enabled;
  nm_root["dimmer"]["speed"] = mainState.dimmer.pspeed;
  nm_root["dimmer"]["length"] = mainState.dimmer.plength;
  nm_root["dimmer"]["decay"] = mainState.dimmer.decay;
  
  nm_root["skaters"]["enabled"] = mainState.skaters.enabled;
  nm_root["skaters"]["speed"] = mainState.skaters.pspeed;
  nm_root["skaters"]["length"] = mainState.skaters.plength;
  nm_root["skaters"]["decay"] = mainState.skaters.decay;
  
  nm_root["powerSaver"]["enabled"] = mainState.powerSaver.enabled;
  nm_root["powerSaver"]["speed"] = mainState.powerSaver.pspeed;
  nm_root["powerSaver"]["length"] = mainState.powerSaver.plength;
  nm_root["powerSaver"]["decay"] = mainState.powerSaver.decay;
  
  nm_root["ants"]["enabled"] = mainState.ants.enabled;
  nm_root["ants"]["speed"] = mainState.ants.pspeed;
  nm_root["ants"]["length"] = mainState.ants.plength;
  nm_root["ants"]["decay"] = mainState.ants.decay;
  
  nm_root["launch"]["enabled"] = mainState.launch.enabled;
  nm_root["launch"]["speed"] = mainState.launch.pspeed;
  nm_root["launch"]["length"] = mainState.launch.plength;
  nm_root["launch"]["decay"] = mainState.launch.decay;
  
  nm_root["houseLights"]["enabled"] = mainState.houseLights.enabled;
  nm_root["houseLights"]["speed"] = mainState.houseLights.pspeed;
  nm_root["houseLights"]["length"] = mainState.houseLights.plength;
  nm_root["houseLights"]["decay"] = mainState.houseLights.decay;

  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
  Serial.println(nm_msgp);
}

void updatePatterns() { //render the next LED state in the buffer using the current state variables.
  int i;
  int tailPos;
  CRGB glitterMode = CRGB::White;
  fadeToBlackBy(leds, NUM_LEDS, fadeRate);

  if (controllerEnabled) { //maybe change to if (controller_enabled)
    paletteFillController();
  } else {
    redController();
  }

  if (mainState.wave.enabled) {
    //update wave
    //speed is hue change
    //length is hue cycle length
    for (i = 0; i < num_leds; i++) {
      leds[i].setHue(((mainState.patternStep % 256) * mainState.wave.pspeed) + (i / mainState.wave.plength));
    }
  }

  if (mainState.blendwave.enabled) {
    paletteBlendwave();
  }

  if (mainState.ants.enabled) {
    ants();
  }

  if (mainState.mapPattern.enabled) {
    mapPattern();
  }

  if (mainState.holdingPattern.enabled) {
    holdingPattern();
  }

  if (mainState.ripple.enabled) {
//    ripple();
    for (int i=0; i<nRipples; i++) {
      if (ripples[i].enabled) {
//        Serial.printf("ripple " + i);
        ripples[i].animate();
      }
    }
  }

  if (mainState.crazytown.enabled) {
    mainState.crazytown.decay--;
    if (mainState.crazytown.decay > 90) {
      fadeToBlackBy(leds, NUM_LEDS, 255 - (uint8_t)(mainState.crazytown.decay - 90));
    }
    else if (mainState.crazytown.decay > 0) {
      fadeToBlackBy(leds, NUM_LEDS, 255);
    }
    if (mainState.crazytown.decay < 0) {
      int j;
      for (j = 300; j > 1; j--) {

        for (i = 0; i < num_leds; i++) {
          if (random8() < mainState.crazytown.plength) {
            leds[i].setHue(random8());
          }
          else  {
            leds[i] = CRGB::Black;
          }
          //mainState.nextUpdate = mainState.lastUpdate + (4000);
        }
        //mainState.crazytown.decay--;

        combineLeds();
        FastLED.show();
        delay(2);
      }

      mainState.crazytown.enabled = false;
      mainState.crazytown.decay = 0;
    }
  }

  if (mainState.rain.enabled) {
    rain();
  }

  if (mainState.skaters.enabled) {
    skaters();
  }

  if (mainState.houseLights.enabled) {
    houseLights();
  }
  
  if (mainState.paletteDisplay.enabled && audienceSpot == 4) {
    paletteDesignDisplay();
  }
  
  if (mainState.tail.enabled) {
    //speed has direction
    //length is how often a tail
    if (mainState.tail.pspeed >= 0) {
      tailPos = ( ((mainState.patternStep * mainState.tail.pspeed) ) %  mainState.tail.plength);
    }
    else {
      tailPos = ( (((int)abs(65535 - mainState.patternStep) * (int)abs(mainState.tail.pspeed)) ) % mainState.tail.plength);
    }
  //    tailPos = ( ((mainState.patternStep * mainState.tail.pspeed) ) %  mainState.tail.plength);
    for (i = 0; i < num_leds; i++) {
      leds[i] = tailScale(leds[i] , abs(tailPos - (i %  mainState.tail.plength)));
    }
  }

  if (mainState.glitter.enabled) {
    if (mainState.glitter.pspeed < 0) {
      glitterMode = CRGB::Black;
    }
    if (random8() < mainState.glitter.plength) {
      leds[ random16(num_leds) ] = ColorFromPalette(currentPalette, 36, 250);
      leds[ random16(num_leds) ] = ColorFromPalette(currentPalette, 36, 250);
      leds[ random16(num_leds) ] = ColorFromPalette(currentPalette, 36, 250);
      leds[ random16(num_leds) ] = ColorFromPalette(currentPalette, 36, 250);
      leds[ random16(num_leds) ] = ColorFromPalette(currentPalette, 36, 250);
      leds[ random16(num_leds) ] = ColorFromPalette(currentPalette, 36, 250);
    }
  }
  
  if (mainState.breathe.enabled) {
    //speed is tempo
    //length is depth of darkness
    nscale8_video(ledsOut, NUM_LEDS_OUT, ( sin8((mainState.breathe.pspeed * mainState.patternStep / 6)))); //pspeed is approx bpm
  }

  if (mainState.sweep.enabled) {
    sweep();
  }

  if (mainState.poleChaser.enabled) {
    poleChaserOff();
  }

  if (mainState.powerSaver.enabled) {
    powerSaver();
  }

  if (audienceSpot == 7) { //dim the guardian costume
    fade_video(leds, num_leds, 100);
  }

  if (mainState.dimmer.enabled) {//dimmer
  //  if (true) {
    fade_video(leds, num_leds, mainState.dimmer.plength * 25);
  //    fade_video(leds, num_leds, 150);
  }

  if (mainState.enlightenment.enabled) {
    enlightenmentBuildUp();
    //fade out more pixels progressively
    if (mainState.enlightenment.pspeed < 4) {
      for (int j = 0; j <= mainState.enlightenment.pspeed; j++) {
        uint8_t inc = primes[j];
        for (int i = 0; i < num_leds; i += inc) {
          uint8_t scale = 0;
          if (j == mainState.enlightenment.pspeed) {
            scale = 255 - mainState.enlightenment.decay;
          }
          leds[i].nscale8_video(scale);
        }
      }
    }
    if (mainState.enlightenment.pspeed == 4) {
      enlightenmentAchieved();
    }
  }

  if (mainState.launch.enabled) {
    launch();
  }

  if (false) {
    paletteFill();
  }

  alwaysOn();

  frameCount++;
  if (frameCount % stepRate == 0) {
    mainState.patternStep++;
  }
  //  Serial.printf("%d - %d\n", frameCount, mainState.patternStep);

  //  if (mainState.patternStep >= NUM_LEDS) {//boo
  //    mainState.patternStep=0;
  //  }

  mainState.stale = false;
} // updatePatterns()

//************************************************************
// Pattern functions, shared
CRGB tailScale (CRGB pixel, int pos) {
  switch (abs(pos)) {
    case 0:
      return (-pixel);
      break;
    case 2:
      return ( (pixel).nscale8(16) );
    default:
      return (pixel);
      break;
  }
}

void enlightenmentBuildUp() {
  float progress = (float) enlightenment.elapsed() / (float) enlightenTime;
  uint8_t attainment = 0; //Sotapanna - stream-enterer
  if (progress > 0.25) { //Sakadagami - once returner
    attainment = 1;
  }
  if (progress > 0.5) { //Anagami - non-returner
    attainment = 2;
  }
  if (progress > 0.75) { //Arahant - Deserving
    attainment = 3;
  }
  uint8_t decayVal = (progress * 255 * 4);
  if (progress >= 1) {
    //enlightenment attained!
    decayVal = 255;
    attainment = 4;
  }
  decayVal = ease8InOutApprox(decayVal);
  mainState.enlightenment.decay = decayVal;
  if (mainState.enlightenment.pspeed != attainment) {
    mainState.enlightenment.pspeed = attainment;
  }

}

void enlightenmentAchieved() {
  enlightenment.stopTimer();
  mainState.enlightenment.enabled = false;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fill_solid(ledsController, NUM_LEDS_CONTROLLER, CRGB::Black);
  combineLeds();
  FastLED.show();
  delay(3000);

  //lightning
  uint8_t frequency = 5;                                       // controls the interval between strikes
  uint8_t flashes = 8;                                          //the upper limit of flashes per strike
  unsigned int dimmer = 1;
  uint8_t ledstart;                                             // Starting location of a flash
  uint8_t ledlen; // Length of a flash

  for (int fl = 0; fl < 60; fl++) {
    ledstart = random16(num_leds);                               // Determine starting location of flash
    ledlen = random8(num_leds - ledstart);                      // Determine length of flash (not to go beyond NUM_LEDS-1)

    for (int flashCounter = 0; flashCounter < random8(3, flashes); flashCounter++) {
      if (flashCounter == 0) dimmer = 5;                        // the brightness of the leader is scaled down by a factor of 5
      else dimmer = random8(1, 3);                              // return strokes are brighter than the leader

      fill_solid(leds + ledstart, ledlen, CHSV(255, 0, 255 / dimmer));
      combineLeds();
      FastLED.show();                       // Show a section of LED's
      delay(random8(2, 15));                                    // each flash only lasts 4-10 milliseconds
      fill_solid(leds + ledstart, ledlen, CHSV(255, 0, 0));     // Clear the section of LED's
      combineLeds();
      FastLED.show();

      if (flashCounter == 0) delay (random8(10));                       // longer delay until next flash after the leader

      delay(random8(10));                                   // shorter delay between strokes
    } // for()

    delay(random8(frequency * 4));                            // delay between strikes
  }
  //fade back on in flashes
  for (int k = 1; k <= 1; k++) {
    uint8_t fadeOn = 0;
    uint16_t fadeBase = 0;
    while (fadeBase < 255) {
      fadeBase += k;
      fadeOn = ease8InOutApprox(fadeBase);
      fill_solid(leds, NUM_LEDS, CHSV(128, 180, fadeOn));
      combineLeds();
      FastLED.show();
      delay(6);
    }
    fill_solid(leds, NUM_LEDS, CHSV(128, 180, 255));
      combineLeds();
    FastLED.show();
    delay(k * 50);
  }
  delay(2000);
  mainState.glitter.enabled = true;
  glitterTimer.startTimer();
}

void ripple() {
  //  fadeToBlackBy(leds, NUM_LEDS, 20);                             // 8 bit, 1 = slow, 255 = fast
  static int rippleLoc;
  static uint8_t colour;                                               // Ripple colour is randomized.
  static int center = 0;                                               // Center of the current ripple.
  static int rippleStep = -1;                                          // -1 is the initializing step.
  static uint8_t myfade = 255;                                         // Starting brightness.
  switch (rippleStep) {
    case -1:                                                          // Initialize ripple variables.
      center = random(num_leds);
      colour = random8();
      rippleStep = 0;
      break;
    case 0:
      leds[center] = ColorFromPalette(currentPalette, colour, myfade, currentBlending);
      if (frameCount % stepRate == 0) {
        rippleStep ++;
      }
      break;
    case 108:                                                    // At the end of the ripples.
      if (random8(1,10)==5) {
        rippleStep = -1;
      }
      break;
    default:                                                          // Middle of the ripples.
      rippleLoc = ease8InOutApprox(rippleStep);
      leds[(center + rippleLoc + num_leds) % num_leds] += ColorFromPalette(currentPalette, colour, myfade / (rippleLoc + 1) * 2, currentBlending); // Simple wrap from Marc Miller
      leds[(center - rippleLoc + num_leds) % num_leds] += ColorFromPalette(currentPalette, colour, myfade / (rippleLoc + 1) * 2, currentBlending);
      
      if (frameCount % stepRate == 0) {
        rippleStep ++;// Next step.
      }
      break;
  } // switch step
} // ripple()

void blendwave() {
  static CRGB clr1;
  static CRGB clr2;
  static uint8_t blendSpeed;
  static uint8_t loc1;
  static uint8_t loc2;
  static uint8_t ran1;
  static uint8_t ran2;
  blendSpeed = beatsin8(6, 0, 255);
  clr1 = blend(CHSV(beatsin8(3, 0, 255), 255, 255), CHSV(beatsin8(4, 0, 255), 255, 255), blendSpeed); //TODO get colour from palette instead
  clr2 = blend(CHSV(beatsin8(4, 0, 255), 255, 255), CHSV(beatsin8(3, 0, 255), 255, 255), blendSpeed);
  loc1 = beatsin8(10, 0, num_leds - 1);
  fill_gradient_RGB(leds, 0, clr2, loc1, clr1);
  fill_gradient_RGB(leds, loc1, clr2, num_leds - 1, clr1);
} // blendwave()

void paletteBlendwave() {
  static CRGB clr1;
  static CRGB clr2;
  static uint8_t blendSpeed;
  static uint8_t loc1;
  static uint8_t loc2;
  static uint8_t ran1;
  static uint8_t ran2;
  //  blendSpeed = beatsin8(4, 0, 255);
  blendSpeed = sin8(4 * mainState.patternStep / 6);
  //  clr1 = blend(ColorFromPalette(currentPalette, beatsin8(2, 0, 255), 255), ColorFromPalette(currentPalette, beatsin8(3, 0, 255), 255), blendSpeed); 
  //  clr2 = blend(ColorFromPalette(currentPalette, beatsin8(3, 0, 255), 255), ColorFromPalette(currentPalette, beatsin8(2, 0, 255), 255), blendSpeed);
  clr1 = blend(ColorFromPalette(currentPalette, sin8(2 * mainState.patternStep / 6), 255), ColorFromPalette(currentPalette, sin8(3 * mainState.patternStep / 6), 255), blendSpeed); 
  clr2 = blend(ColorFromPalette(currentPalette, sin8(3 * mainState.patternStep / 6), 255), ColorFromPalette(currentPalette, sin8(2 * mainState.patternStep / 6), 255), blendSpeed);
  //  loc1 = beatsin8(6, 0, num_leds - 1);
  loc1 = map(sin8(6 * mainState.patternStep / 6), 0, 255, 0, num_leds-1);
  fill_gradient_RGB(leds, 0, clr2, loc1, clr1);
  fill_gradient_RGB(leds, loc1, clr2, num_leds - 1, clr1);
} // blendwave()

void holdingPattern() {
  uint8_t fps = 1000000 / INTERVAL;
  uint8_t hPeriod = fps * 3; //every 3 seconds
  uint8_t hStep = mainState.patternStep % hPeriod;
  if (hStep == 0) {
    mainState.hue++;//use the hue as a palette index
  }
  for (int n = 0; n < nStrips; n++) {
    if (hStep < 11) {
      for (int j = stripLength - 3 - 11 + hStep; j < stripLength; j++) {
        int i = nX(n, j);
        leds[i] = ColorFromPalette(currentPalette, mainState.hue, 255);
      }
    }
    if ((hStep > 13 && hStep < 18) || (hStep > 20 && hStep < 27)) {
      for (int j = stripLength - 3; j < stripLength; j++) {
        int i = nX(n, j);
        leds[i] = ColorFromPalette(currentPalette, mainState.hue, 255);
      }
    }
  }
}

void alwaysOn() {
  if (mainState.patternStep % 144 == 0) {
    aoIndex++;
  }
  leds[aoIndex % num_leds] = ColorFromPalette(currentPalette, aoIndex % 256, 255);
}

void paletteFill() {
  for (int i = 0; i < num_leds; i++) {
    uint8_t pIdx = map(i, 0, num_leds - 1, 0, 255);
    leds[i] = ColorFromPalette(currentPalette, pIdx, 255);
  }
}

void rain() {
  static uint8_t dropPosition[30] = {0};
  static uint8_t dropSpeed[30] = {0};
  static uint16_t lastStep = mainState.patternStep;
  mainState.rain.plength = max((uint8_t) 3, mainState.rain.plength);
  //  fadeToBlackBy(leds, num_leds, 100);
  for (int nDrop = 0; nDrop < mainState.rain.plength; nDrop++) {
    if (dropSpeed[nDrop] == 0) {
      dropSpeed[nDrop] = random8(3, mainState.rain.pspeed);
    }
    if (dropPosition[nDrop] == 0) {
      dropPosition[nDrop] = random8(stripLength - 11, stripLength - 1);
    }
    if (mainState.patternStep != lastStep) {
      if (mainState.patternStep % dropSpeed[nDrop] == 0) {
        dropPosition[nDrop]--;
      }
    }
    //    uint8_t pos = ease8InOutApprox(dropPosition[nDrop]);
    //    uint8_t pos = map(dropPosition[nDrop], 0, 255, 0, stripLength-1);
    uint8_t pos = dropPosition[nDrop];
    for (int strip = 0; strip < nStrips; strip++) {
      int i = nX(strip, pos);
      leds[i] += ColorFromPalette(currentPalette, nDrop * 15, mainState.rain.decay);
    }
  }
  lastStep = mainState.patternStep;
}

void mapPattern() {
  for (int strip = 0; strip < nStrips; strip++) {
    for (int pos = stripLength - 1; pos >= 0; pos--) {
      int i = nX(strip, pos);
      leds[i] = ColorFromPalette(mapPalettes[mainState.mapPattern.plength - 1], (mainState.patternStep + i) * mainState.mapPattern.pspeed, 150);
    }
  }
}

void paletteDesignDisplay() {
  static uint8_t pos;
  pos = beatsin8(9, 0, stripLength);
  //  pos = (pos + stripLength/2) % stripLength;
  //  pos = pos % stripLength;
  leds[nX(0, pos)] = designColour;
  leds[nX(0, pos) + 1] = designColour;

  //current palette
  for (int ppos = 0; ppos < stripLength; ppos++) {
    uint8_t pIdx = map(ppos, 0, stripLength - 1, 0, 255);
    leds[nX(1, ppos)] = ColorFromPalette(currentDesignPalette, pIdx, 100);
  }

}

void sweep() {
  //  uint8_t sweepSpot = audienceSpot;
  long sweepDistance = abs(mainState.sweep.decay - (mainState.sweep.pspeed * sweepSpot));
  //  Serial.println(sweepDistance);
  uint8_t bright = map(sweepDistance, 0, 14 * mainState.sweep.pspeed, 255, 0);
  if (0x1 & mainState.sweep.plength){//rocker switch keeps or sets to palette
    fill_solid(leds, num_leds, ColorFromPalette(currentPalette, sweepSpot * 16));
  }
  if (mainState.sweep.plength & (0x1 << 2)){//toggle switch changes harsh or smooth
    if (sweepDistance < mainState.sweep.pspeed) {
      bright = 245;
    } else {
      bright = 10;
    }
  }
  if (mainState.sweep.plength & (0x1 << 1)) {//push switch sweeps darkness or lightness
    bright = 255 - bright;
  }
  //  Serial.println(bright);
  fade_video(leds, num_leds, bright);
  mainState.sweep.decay++;
  if (sweepDistance >= 14 * mainState.sweep.pspeed) {
    mainState.sweep.enabled = false;
  }
}

void skaters() {
  //  static uint8_t skaterPosition[num_leds/2] = {0};
  static uint8_t leaderPosition = 0;
  leaderPosition = (leaderPosition + mainState.skaters.pspeed) % stripLength;
  //  mainState.skaters.plength = min(mainState.skaters.plength, (int8_t) (9));
  for (int nSkater = 0; nSkater < mainState.skaters.plength; nSkater++) {
    uint8_t offset = sPrimes[8 - nSkater];
    for (int rep = 0; rep * offset < stripLength; rep++) {
      uint8_t pos = (leaderPosition + rep * offset) % stripLength;
      for (int strip = 0; strip < nStrips; strip++) {//eep triple nest for loop
        int i = nX(strip, pos);
  //          int i = 1;
        leds[i] = ColorFromPalette(cyclePalettes[(paletteCycleIndex + 5) % nPalettes], nSkater * 15, 255);
      }
    }
  }
}

void poleOff(int8_t n) { 
  leds[n*stripLength, (n+1)*stripLength - 1].fadeToBlackBy(255);
}

void poleChaserOff() {
  for (int pole=0; pole < nStrips; pole++) {
    if ((mainState.patternStep * mainState.poleChaser.pspeed) % nStrips == pole) {
      poleOff(pole);
    }
  }
}

void powerSaver() {
  static int start = 0;
  fade_video(leds, num_leds, 100);
  if (mainState.powerSaver.plength == 2) {
    //turn off every 7th led 
    for (int i=start; i<num_leds; i+=7) {
      leds[i].nscale8(0);
    }
  }
  if (mainState.patternStep % 244 == 0) {
    start = (start + 1) % 7;
  }
}

void ants() {
  static int leader = 0;
  static uint8_t pIdx = 13 * 16;
  uint8_t nAnts = mainState.ants.plength;
  uint8_t spacing = mainState.ants.pspeed;
  int pos;
  for (int i=0; i<nAnts; i++) {
    pos = leader - (i * spacing);
    if ((pos >= 0) && (pos <= stripLength)) {
      for (int strip = 0; strip < nStrips; strip++) {
        leds[nX(strip, pos)] = ColorFromPalette(currentPalette, pIdx, mainState.ants.decay);
      }
    }
  }
  leader++;
  if (pos > stripLength) {
    leader = 0;
  }
}

void launch() {
  static int sweepProgress = 0;
  static int sweepBase = 64;
  static int onFor = 72*6;
  static bool phase2 = false;
  static int phase2Progress = 0;
  static uint8_t lowCount = 0;
  uint8_t sweepRate = map(ease8InOutApprox(map(sweepBase, 0, 96, 0, 255)), 0, 255, 1, 96); //we want to use easing, which works on a scale of 0 to 255, but we are working on a scale of 3 to 96 instead.

  //stage 1 - fade out
  if (!phase2) {
    if (launchProgress <= 255/3) { // 255/3 = 85 frames (about 3 seconds)
      fadeToBlackBy(leds, num_leds, ease8InOutApprox(launchProgress*3));
    } else if (launchProgress < 85 + 25) { //stay black for 1 second
      fill_solid(leds, num_leds, CRGB::Black);
    } else if (launchProgress == 85 + 25) {
      fill_solid(leds, num_leds, CRGB::Black);
      patternsOff();
      mainState.paletteDisplay.enabled = false;
      controllerEnabled = false;
      fadeRate = 10;
      mainState.dimmer.plength = 1;
      sweepProgress = 0;
      targetPalette = CloudColors_p;
      if (sweepSpot == 6) {
        fadeRate = 150;
        mainState.breathe.enabled = true;
        mainState.breathe.pspeed = 30;
        mainState.ants.enabled = true;
        mainState.ants.plength = 62;
        mainState.ants.pspeed = 6;
      }
    } else if (sweepBase > 0) {
      if (sweepSpot != 6) {
        if ((sweepProgress >= sweepRate * sweepSpot) && (sweepProgress < (sweepRate * sweepSpot) + onFor)) {
          houseLights();
        }
      }
      sweepProgress++;
      if (sweepProgress == sweepRate * 6) {
        onFor = 3;
        fadeRate = min(fadeRate + 10, 255);
        sweepProgress = 0;
        lowCount++;
        mainState.ants.pspeed = sweepRate;
        if (sweepBase > 16) {
          sweepBase -= 4;
        } else if (sweepBase > 8) {
          sweepBase--;
        } else if ((sweepBase > 6) && (lowCount % 2 == 0)) {
          sweepBase--;
        } else if ((sweepBase > 4) && (lowCount % 4 == 0)) {
          sweepBase--;
        } else if ((sweepBase > 1) && (lowCount % 6 == 0)) {
          sweepBase--;
        } else if ((sweepBase == 1) && (lowCount % 12 == 0)) {
          sweepBase--;
        }
        if (sweepBase == 52) {
          mainState.tail.enabled = true;
          mainState.tail.plength = 255;
          mainState.tail.pspeed = 1;
        }
        if (sweepBase == 40) {
          mainState.tail.pspeed = 2;
        }
        if (sweepBase == 28) {
          mainState.tail.pspeed = 3;
        }
        if (sweepBase == 16) {
          mainState.tail.pspeed = 4;
        }
        Serial.printf("sweepBase: %d, sweepRate: %d\n", sweepBase, sweepRate);
      }
    } else {
      phase2 = true;
      houseLights();
      mainState.dimmer.plength = 0;
      fadeRate = 1;
      stepRate = 1;
      mainState.ants.enabled = true;
      mainState.ants.plength = 200;
      mainState.ants.pspeed = 6;  
      mainState.ants.decay = 150;
    }
  }

  if (phase2) {
    //5 seconds of house lights
    if (phase2Progress < 175) {
      houseLights();
    } else if ((phase2Progress % 2 == 0) && (fadeRate < 255)) {
      fadeRate++;
    } else if (phase2Progress < 1023) {
      if (phase2Progress % 75 == 0) {
        mainState.ants.pspeed += 4;
      }
    } else if (phase2Progress == 1023) {
      mainState.launch.enabled = false;
      tranquilityMode();
      mainState.ants.enabled = true;
      mainState.glitter.enabled = true;
      mainState.glitter.pspeed = 1;
      mainState.glitter.plength = 255;
      glitterTimer.startTimer();
      fadeRate = 10;
      controllerEnabled = true;
      phase2 = false;
      launchProgress = 0;
      sweepProgress = 0;
      sweepBase = 64;
      onFor = 72*6;
      phase2Progress = 0;
    }
    phase2Progress++;
  }

  launchProgress++;
}

void houseLights() {
  fill_solid(leds, num_leds, CRGB::White);
  if (num_leds > 400 || audienceSpot == 7) {
    for (int i=0; i<num_leds; i+=2) {
      leds[i] = CRGB::Black;
    }
  }
}

uint16_t nX(uint8_t n, uint8_t x) {
  uint16_t i;
  n = min(n, nStrips);
  x = min(x, stripLength);

  if ( stripDirection[n] == 1 ) {
    i = (n) * stripLength + x;
  } else {
    i = (n) * stripLength + stripLength - x - 1;
  }
  return i;
}


//*********************************************************
//Controller LEDS patterns
void paletteFillController() {
  for (int i = 0; i < NUM_LEDS_CONTROLLER; i++) {
    uint8_t pIdx = map(i, 0, NUM_LEDS_CONTROLLER - 1, 0, 255);
    ledsController[i] = ColorFromPalette(currentPalette, (pIdx + mainState.patternStep) % 256, 255);
  }
}

void redController() {
  for (int i = 0; i < NUM_LEDS_CONTROLLER; i++) {
    ledsController[i] = CRGB::Red;
  }
}

void powerSaverController() {
  fade_video(ledsController, NUM_LEDS_CONTROLLER, 100);
  ledsController[0] = CRGB::Black;
  if (mainState.powerSaver.plength == 2 || holdingPatternLockdown) {
    ledsController[2] = CRGB::Black;
    ledsController[3] = CRGB::Black;
  }
}
//************************************************************
//Messaging, shared
void processPOmessage() {
  int mtype;
  int enVal;
  uint16_t msgAudience;
  uint8_t paletteArray[48] = {};
  uint8_t bleep;
  uint8_t breatherMsgVal;
  uint32_t pInterval;
  uint8_t directionVal;
  int8_t skaterVal;
  bool rev;
  offline_pattern.resetTimer();

  DynamicJsonBuffer jsonBuffer;
  JsonObject& po_root = jsonBuffer.parseObject(msgp);
  if (po_root.success()) {
    Serial.printf("We reveived a json");
  }
  else {
  //    Serial.printf("Parse error on json msg");
  }

  if (po_root.containsKey("poMessageType")) {
    mtype = po_root["poMessageType"].as<int>();
    if (!mtype == 99) {
      boredTimer.resetTimer();
    }
  //    Serial.printf(" of poMessageType %i\n", mtype);
    //po_root.prettyPrintTo(Serial);
  }
  else {
    return ;
  }


  if (po_root["timestart"] > mesh.getNodeTime() && mtype != 99) {
    mainState.nextUpdate = po_root["timestart"].as<uint32_t>();
  }

  if (po_root.containsKey("patternStep")) {
    mainState.patternStep = po_root["patternStep"];
  }
  if (po_root.containsKey("hue")) {
    mainState.hue = po_root["hue"];
  }

  switch (mtype) {
    case 1 :

      //if (po_root["audience"] == 0xFF) {
      //this is a control message follow all examples
      //
      //      if (po_root["timestart"] > mesh.getNodeTime()){
      //        mainState.nextUpdate = po_root["timestart"].as<uint32_t>();
      //      }
      //
      //      mainState.patternStep = po_root["patternStep"];
      //      mainState.hue = po_root["hue"];

      stepRate = po_root["stepRate"];
      fadeRate = po_root["fadeRate"];
      paletteCycleIndex = po_root["paletteIndex"];
      targetPalette = cyclePalettes[paletteCycleIndex];
      paletteCycleTimer.resetTimer();

      mainState.wave.enabled = po_root["wave"]["enabled"];
      mainState.wave.pspeed = po_root["wave"]["speed"];
      mainState.wave.plength = po_root["wave"]["length"];
      mainState.wave.decay = po_root["wave"]["decay"];

      mainState.tail.enabled = po_root["tail"]["enabled"];
      mainState.tail.pspeed = po_root["tail"]["speed"];
      mainState.tail.plength = po_root["tail"]["length"];
      mainState.tail.decay = po_root["tail"]["decay"];

      mainState.breathe.enabled = po_root["breathe"]["enabled"];
      mainState.breathe.pspeed = po_root["breathe"]["speed"];
      mainState.breathe.decay = po_root["breathe"]["length"];
      mainState.breathe.decay = po_root["breathe"]["decay"];

      mainState.glitter.enabled = po_root["glitter"]["enabled"];
      mainState.glitter.pspeed = po_root["glitter"]["speed"];
      mainState.glitter.plength = po_root["glitter"]["length"];
      mainState.glitter.plength = po_root["glitter"]["decay"];

      mainState.crazytown.enabled = po_root["crazytown"]["enabled"];
      mainState.crazytown.pspeed = po_root["crazytown"]["speed"];
      mainState.crazytown.plength = po_root["crazytown"]["length"];
      mainState.crazytown.decay = po_root["crazytown"]["decay"];
      
      mainState.ripple.enabled = po_root["ripple"]["enabled"];
      mainState.ripple.pspeed = po_root["ripple"]["speed"];
      mainState.ripple.plength = po_root["ripple"]["length"];
      mainState.ripple.decay = po_root["ripple"]["decay"];
    
      mainState.blendwave.enabled = po_root["blendwave"]["enabled"];
      mainState.blendwave.pspeed = po_root["blendwave"]["speed"];
      mainState.blendwave.plength = po_root["blendwave"]["length"];
      mainState.blendwave.decay = po_root["blendwave"]["decay"];
    
      mainState.rain.enabled = po_root["rain"]["enabled"];
      mainState.rain.pspeed = po_root["rain"]["speed"];
      mainState.rain.plength = po_root["rain"]["length"];
      mainState.rain.decay = po_root["rain"]["decay"];
    
      mainState.holdingPattern.enabled = po_root["holdingPattern"]["enabled"];
      mainState.holdingPattern.pspeed = po_root["holdingPattern"]["speed"];
      mainState.holdingPattern.plength = po_root["holdingPattern"]["length"];
      mainState.holdingPattern.decay = po_root["holdingPattern"]["decay"];
      
      mainState.mapPattern.enabled = po_root["mapPattern"]["enabled"];
      mainState.mapPattern.pspeed = po_root["mapPattern"]["speed"];
      mainState.mapPattern.plength = po_root["mapPattern"]["length"];
      mainState.mapPattern.decay = po_root["mapPattern"]["decay"];
      
//      mainState.paletteDisplay.enabled = po_root["paletteDisplay"]["enabled"];
//      mainState.paletteDisplay.pspeed = po_root["paletteDisplay"]["speed"];
//      mainState.paletteDisplay.plength = po_root["paletteDisplay"]["length"];
//      mainState.paletteDisplay.decay = po_root["paletteDisplay"]["decay"];
      
      mainState.sweep.enabled = po_root["sweep"]["enabled"];
      mainState.sweep.pspeed = po_root["sweep"]["speed"];
      mainState.sweep.plength = po_root["sweep"]["length"];
      mainState.sweep.decay = po_root["sweep"]["decay"];
      
      mainState.dimmer.enabled = true;
      mainState.dimmer.pspeed = po_root["dimmer"]["speed"];
      mainState.dimmer.plength = po_root["dimmer"]["length"];
      mainState.dimmer.decay = po_root["dimmer"]["decay"];
      
      mainState.skaters.enabled = po_root["skaters"]["enabled"];
      mainState.skaters.pspeed = po_root["skaters"]["speed"];
      mainState.skaters.plength = po_root["skaters"]["length"];
      mainState.skaters.decay = po_root["skaters"]["decay"];
      
      mainState.powerSaver.enabled = po_root["powerSaver"]["enabled"];
      mainState.powerSaver.pspeed = po_root["powerSaver"]["speed"];
      mainState.powerSaver.plength = po_root["powerSaver"]["length"];
      mainState.powerSaver.decay = po_root["powerSaver"]["decay"];
      
      mainState.ants.enabled = po_root["ants"]["enabled"];
      mainState.ants.pspeed = po_root["ants"]["speed"];
      mainState.ants.plength = po_root["ants"]["length"];
      mainState.ants.decay = po_root["ants"]["decay"];
      
//      mainState.launch.enabled = po_root["launch"]["enabled"];
//      mainState.launch.pspeed = po_root["launch"]["speed"];
//      mainState.launch.plength = po_root["launch"]["length"];
//      mainState.launch.decay = po_root["launch"]["decay"];
      
      mainState.houseLights.enabled = po_root["houseLights"]["enabled"];
      mainState.houseLights.pspeed = po_root["houseLights"]["speed"];
      mainState.houseLights.plength = po_root["houseLights"]["length"];
      mainState.houseLights.decay = po_root["houseLights"]["decay"];

      
      mainState.stale = true;
      break;

    case 2: //enlightenment message
      enVal = po_root["enlightenVal"];
      //      Serial.println(enVal);
      enlightenmentCallback(enVal);
      break;

    case 3: //map buttons message
      msgAudience = po_root["audience"];
      if (msgAudience & (0x01 << audienceSpot)) {
        mainState.mapPattern.enabled = true;
        mainState.mapPattern.plength = po_root["plength"];
        mainState.mapPattern.pspeed = po_root["pspeed"];
      } else {
        mainState.mapPattern.enabled = false;
      }
      break;

    case 4: //palette designer message
      //turn the array into a palette
      bleep = po_root["newPalette"].asArray().copyTo(paletteArray);
      //      Serial.println(paletteArray);
//      for (int i = 0; i < 48; i++) {
        //        Serial.printf("%d: %d\n", i, paletteArray[i]);
//      }
      for (int i = 0; i < 16; i++) {
        int j = i * 3;
        targetPalette[i] = CRGB(paletteArray[j], paletteArray[j + 1], paletteArray[j + 2]);
        //        Serial.printf("%d: r:%d, g:%d, b:%d\n", i, targetPalette[i].red, targetPalette[i].green. targetPalette[i].blue);
      }
      break;

    case 5:
      if (po_root["launchVal"]) {
        mainState.launch.enabled = true;
      } else if (po_root["switchVal"]) {
        tripperTrapMode();
      } else {
        upset_mainState();
      }
      break;

    case 6: //sweep
      mainState.sweep.enabled = true;
      mainState.sweep.plength = po_root["plength"];
      mainState.sweep.pspeed = po_root["pspeed"];
      mainState.sweep.decay = 0;

      break;

    case 7://random analog
      mainState.dimmer.plength = po_root["dimmerVal"];
      breatherMsgVal  = po_root["breatherVal"];
      mainState.breathe.pspeed = map(breatherMsgVal, 0, 10, 1, 60);
      if (breatherMsgVal != 0) {
        mainState.breathe.enabled = true;
      } else {
        mainState.breathe.enabled = false;
      }
      stepRate = po_root["frameRateVal"];
  //      pInterval = map(po_root["paletteChangeVal"], 0, 10, 8000, 5*60000);
  //      Serial.println(pInterval);
  //      paletteCycleTimer.setInterval(pInterval);
      fadeRate = po_root["fadeRateVal"];
      fadeRate = (fadeRate  * 24) + 1;
      break;

    case 8: // nextPalette
      paletteCycleIndex = po_root["paletteIndex"];
      targetPalette = cyclePalettes[paletteCycleIndex];
      paletteCycleTimer.resetTimer();
      break;

    case 9: //pro joy
      skaterVal = po_root["msgVal"];
      if (skaterVal == 0) {
        mainState.skaters.enabled = false;
      } else {
        mainState.skaters.enabled = true;
        mainState.skaters.plength = abs(skaterVal);
        mainState.skaters.pspeed = 1;
      }
      directionVal = po_root["directionVal"];
      rev = po_root["rev"];
      switch (directionVal) {
        case 1:
          if (rev) {
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = -1 * directionIO[i];
            }
          } else {
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = directionIO[i];
            }
          }
        break;
        case 2:
          if (rev) {
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = -1 * directionLR[i];
            }
          } else {
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = directionLR[i];
            }
          }
        break;
        case 3:
          if (rev) {
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = -1 * directionUD[i];
            }
          } else {
            for (int i=0; i<nStrips; i++) {
              stripDirection[i] = directionUD[i];
            }
          }
        break;
      }
      
      break;

    case 10: //switchRow Message
//      upset_mainState(); //get new parameters
//      patternsOff();
      mainState.rain.enabled = po_root["rainVal"];
      mainState.blendwave.enabled = po_root["blendWaveVal"];
      mainState.ripple.enabled = po_root["rippleVal"];
      mainState.ants.enabled = po_root["antsVal"];
      break;

    case 11: // wiimote
      mainState.rain.enabled = false;
      mainState.tail.enabled = true;
      mainState.tail.pspeed = po_root["pspeed"];
      mainState.tail.plength = po_root["plength"];
      break;
      
    case 12:// wiimote
      mainState.tail.enabled = false;
      mainState.rain.enabled = true;
      mainState.rain.pspeed = po_root["pspeed"];
      mainState.rain.plength = po_root["plength"];
      break;

    case 13: // disable controller
      if (audienceSpot == po_root["audience"] || po_root["audience"] == 99) {
        if (po_root["messagingOnly"]) {
          Serial.printf("1 M: %d, C: %d", messagingEnabled, controllerEnabled);
          messagingEnabled = po_root["enabled"];
          Serial.printf("2 M: %d, C: %d", messagingEnabled, controllerEnabled);
        } else {
          Serial.printf("3 M: %d, C: %d", messagingEnabled, controllerEnabled);
          controllerEnabled = po_root["enabled"];
          Serial.printf("4 M: %d, C: %d", messagingEnabled, controllerEnabled);
        }
      }
      break;

    case 14: //go into holding pattern mode
      holdingPatternMode(po_root["holdVal"]);
      break;

    case 15:
      tranquilityMode();
      break;

    case 16: //ants
      patternsOff();
      mainState.ants.enabled = true;
      mainState.ants.plength = 20;
      mainState.ants.pspeed = 8;
      mainState.ants.decay = 150;
      stepRate = 1;
      fadeRate = 250;
      break;
    case 17: //too Flashy
      mainState.breathe.enabled = false;
      mainState.tail.enabled = false;
      mainState.glitter.enabled = false;
      fadeRate = 70;
      break;
    case 18: //patterns off
      patternsOff();
      break;
    case 19: //house lights
      mainState.houseLights.enabled = true;
      break;

    case 99: // battery status update
      if (audienceSpot == 7) {//the guardian costume is on a different battery
        break;
      }
      batteryHealth = po_root["batteryHealth"];
      batteryUpdateTimer.resetTimer();
      screamerOnline = true;
      break;

    default :
      Serial.printf("exited switch case at default");
      return;
  }
}

//************************************************************

//************************************************************
//pattern control callbacks, shared
void enlightenmentCallback(uint8_t enVal) {
  switch (enVal) {
    case 5:
      enlightenment.stopTimer();
      mainState.enlightenment.enabled = false;
      break;
    case 1:
      mainState.glitter.enabled = true;
      glitterTimer.startTimer();
      break;
    case 2:
      if (audienceSpot == 0) {
        mainState.tail.enabled = !mainState.tail.enabled;
        copyme_message();
      }
      break;
    case 3:
      enlightenment.startTimer();
      mainState.enlightenment.enabled = true;
      break;
  }
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  msgp = msg;
  inbox = true;
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  alone = false;
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
  //!!! deal with pattnerstate updatetime here
  mainState.nextUpdate = mesh.getNodeTime() + INTERVAL;
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

