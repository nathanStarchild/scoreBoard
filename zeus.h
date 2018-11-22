//************************************************************
//oonbase Code 2018
// Location: Fisher Price Remote
// Shape: none
// LEDs: none
// Interactivity: Master control
//************************************************************

//************************************************************
//includes
#define ALWAYS_INPUTS
#define OLED_SCREEN
#define I2C_ADDRESS 0x3C
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "ESP8266WiFi.h"
#include <SparkFunSX1509.h> // Include SX1509 library
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 1;//42
const uint8_t nStrips = 1;//1
const uint16_t num_leds = stripLength * nStrips;

const String shapeNames[8] = {"Maia", "Caleano", "Electra", "Taygeta", "Pleione", "Merope", "Alcyone", "Asterope"};
//************************************************************

//functions
void pleiadianSetup();
void checkInputs();
String getRSSI();
void topline();
void oledmsg();
void essentialGuardian();
void codeMaster();
void moonbaseMonitor();
void shapeShifter();
void patternRecognition();
void warnScreamerOffline();

void tranquilityMode();
void nextPalette();
void patternsOff();
void copyme_message();
void upset_mainState();

void tripperMessage(bool switchVal, bool launchVal);
void enlightenMessage(uint8_t enVal);
void sweepMessage(long sweepValue, uint8_t switchesValue);
void tranquilityMessage();
void nextPaletteMessage();
void controllerMessage(uint8_t audienceVal, bool messagingVal, bool enabledVal);
void mapMessage(uint16_t b, uint8_t n);
void antsMessage();
void tooFlashyMessage();
void patternsOffMessage();
void houseLightsMessage();
void holdingPatternMessage(uint8_t holdingVal);


//************************************************************
//Global variables
int directionLR[nStrips] = {1};
int directionUD[nStrips] = {1};
int directionIO[nStrips] = {1};
int stripDirection[nStrips] = {1};
uint16_t audienceSpot = 8;
uint8_t sweepSpot = 8;

SSD1306AsciiWire oled;
uint16_t messagetally=0;
//const char* SSID = "PleiadianOasis";
//const String SSSID = "Moonbase";
String SSID = MESH_SSID;
const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address
SX1509 io; // Create an SX1509 object to be used throughout

MilliTimer statusScreen(2000);
MilliTimer debounce(150);
MilliTimer newModeTimer(3000);
MilliTimer mapTimer(4000);
MilliTimer newMessageTimer(30000);

uint16_t buttons = 0;
String secondline = "<------------------->";
uint8_t screenMode = 99;
bool newMode = false;
//************************************************************

void pleiadianSetup() {
  Wire.begin();                
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);

  #if INCLUDE_SCROLLING == 0
  #error INCLUDE_SCROLLING must be non-zero. Edit SSD1306Ascii.h
  #elif INCLUDE_SCROLLING == 1
  // Scrolling is not enable by default for INCLUDE_SCROLLING set to one.
  oled.setScroll(false);
  #else  // INCLUDE_SCROLLING
  // Scrolling is enable by default for INCLUDE_SCROLLING greater than one.
  oled.setScroll(false);
  #endif

    if (!io.begin(SX1509_ADDRESS)) //start button multiplexer chip
    {
      oled.println("SX1509 Failed.");
      delay(2000) ;
    }
    io.writeWord(0x0E,0xFFFF); // set all buttons to inputs
    io.writeWord(0x06,0xFFFF); //set all buttons to pullup
        
    topline();
    oled.println(" `_ ,  /\\");
    oled.println("-(_)- /_|\\ MOONBASE");
    oled.println(" ,  `/_|__\\  2018");
    oled.println("    /_|__|_\\");
    oled.println("-_-/_|__|__|\\-_-_-_-_-_");
    oled.println(getRSSI());

//    oled.printf("does printf %s?\n", "work");
    
    Serial.println("Moonbase Remote");
}

void checkInputs() {
  static uint16_t lastButtons = 0;
  if( statusScreen.isItTime() ) { //screen refreshing
    oled.clear(0, oled.displayWidth(), 0,1);
    topline();
    statusScreen.resetTimer();
  }
  
  if (mapTimer.isItTime()) { //part of the shape shifter...
    mapTimer.stopTimer();
    mapMessage(0, 0);
  }
  
  if (debounce.isItTime()){ //check buttons every so often to avoid bounce
    debounce.resetTimer();
    buttons =  ~io.readWord(0x10);
    if (buttons == lastButtons) {
      buttons = 0;
    } else {
      lastButtons = buttons;
    }

    uint16_t modeVal = screenMode;
    switch(buttons) {
      case 2048:
        modeVal = 0;
        break;
      case 4096:
        modeVal = 1;
        break;
      case 8192:
        modeVal = 2;
        break;
      case 16384:
        modeVal = 3;
        break;
    }
    if (modeVal != screenMode) {
      screenMode = modeVal;
      newModeTimer.startTimer();
      newMode = true;
    }

    switch(screenMode) {
      case 0:
        essentialGuardian();
        break;
      case 1:
        moonbaseMonitor();
        break;
      case 2:
        codeMaster();
        break;
      case 3:
        shapeShifter();
        break;
      case 4:
        patternRecognition();
        break;
    }
    

//  switch(buttons) {
//    case 0 : //no button pushed
//      break; 
//    case 16384 : //Button "red"  
//      tripperMessage(true);
//      secondline.remove(0);
//      secondline.concat("Tripper Trap ON");
//    break;
//    case 2048 : //Button "green"   
//      tripperMessage(false);
//      secondline.remove(0);
//      secondline.concat("Tripper Trap OFF");
//    break;
//    case 4096 : //Button "purple"   
//      secondline.remove(0);
//      secondline.concat("ABC");
//    break;
//    case 8192 : //Button "yellow"   
//      secondline.remove(0);
//      secondline.concat("Muzak");
//    break;
//   case 1 : //Button "0"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed (0)");
//    break;
//    case 2 : //Button "1"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed (1)");
//    break;
//    case 4 : //Button "2"   
//      enlightenMessage(3);
//      secondline.remove(0);
//      secondline.concat("Enlightenment NOW");
//    break;
//    case 8 : //Button "3"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed: (3)");
//    break;
//    case 16 : //Button "4"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed: (4)");
//    break;
//    case 32 : //Button "5"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed: (5)");
//    break;
//    case 64 : //Button "6"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed: (6)");
//    break;
//    case 128 : //Button "7"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed: (7)");
//    break;
//    case 256 : //Button "8"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed: (8)");
//    break;
//    case 512 : //Button "9"   
//      secondline.remove(0);
//      secondline.concat("Button Pushed: (9)");
//    break;
//    default :
//      secondline.remove(0);
//      secondline.concat("Button data: ");
//      secondline.concat(buttons);
//    break;    
//   }

  }
  
}

String getRSSI() {
  byte available_networks = WiFi.scanNetworks();

  for (int network = 0; network < available_networks; network++) {
    //if (strcmp(WiFi.SSID(network), target_ssid) == 0) {
      if (SSID.equals(WiFi.SSID(network))) {
      
      return String("SignalStrength ") + WiFi.RSSI(network) + "dBm";
    }
  }
  return String("Signal not found.");
}

void topline() {  //The top two amber lines of the display
  int i;

//  oled.print("Nodes: ");
//  oled.print(mesh.getNodeList().size());
//  oled.print(" msgs: ");
//  oled.print(messagetally);
//  oled.println();
  oled.printf("Nodes: %d msgs: %d\n", mesh.getNodeList().size(), messagetally);
  oled.println(secondline.c_str());
   
}

void warnScreamerOffline() {
  oled.clear();
  topline();
  oled.println();
  oled.println("    !!!WARNING!!!");
  oled.println("Battery Monitor is");
  oled.println("       OFFLINE");
}

void oledmsg(){ //what to do when we receive an network message
  messagetally++;
  if (screenMode == 1) {
    newMessageTimer.startTimer();
    oled.clear();
    topline();
  
    int mtype;
    DynamicJsonBuffer jsonBuffer;
    JsonObject& po_root = jsonBuffer.parseObject(msgp);
    if (po_root.success()) {
//      oled.print("JSOK ");
     }
     else {
      oled.print("BAD JSON ");
      return;
     }
      
    if (po_root.containsKey("poMessageType")) {  //Check its a Pleadian style message
      mtype = po_root["poMessageType"].as<int>();
      oled.printf("Type: %d\n", mtype);
//      oled.print(mtype);
//      oled.print(" Frm ");
//      oled.println((uint32_t)(po_root["sender"]),HEX);
      Serial.print("Received msg: ");
      Serial.println(msgp);
      
      uint8_t pattern_p;
      int oplength;
      int opspeed;
      uint8_t opaudience;
      int oplighten;
      uint8_t bleep;
      uint8_t paletteArray[48] = {};
      uint8_t tval;
      float bval;
      String enAction = "";
      
      switch(mtype) {
        case 1 :        //A standard patternState message
//          pattern_p =  (0x1 & (bool)(po_root["wave"]["enabled"])) | ((0x1 & (bool)(po_root["tail"]["enabled"])) << 1) | ((0x1 & (bool)(po_root["breathe"]["enabled"])) << 2) | ((0x1 & (bool)(po_root["glitter"]["enabled"]))<<3) | ((0x1 & (bool)(po_root["crazytown"]["enabled"]))<<4) | ((0x1 & (bool)(po_root["enlightenment"]["enabled"]))<<5) | ((0x1 & (bool)(po_root["ripple"]["enabled"]))<<6);
//          oled.print(" Aud: ");
//          oled.println((uint16_t)po_root["audience"],HEX);
//          oled.print("Patterns ");
//          oled.println(pattern_p,BIN);
            oled.printf("Copy Me Message");
          break;
          
        case 2 : //enlightenment message
          switch((uint8_t)po_root["enlightenVal"]) {
            case 1:
              enAction = "Satori";
              break;
            case 2:
              enAction = "Distraction";
              break;
            case 3:
              enAction = "Attempting...";
              break;
            case 5:
              enAction = "Not Achieved";
              break;
          }
          oled.printf("Enlightenment: \n      %s", enAction.c_str());
        break;
        case 3 : //map buttons message
          oled.println("Map Buttons msg");
          oplength = po_root["plength"];
          opspeed = po_root["pspeed"];
          opaudience = po_root["audience"];
          oled.print("Map Audience: ");
          oled.println(opaudience);
          oled.print("Speed/Lngth ");
          oled.print(opspeed);
          oled.print("/");
          oled.println(oplength);
        break;
        case 4 : //palette designer message
          oled.println("Palette Designer msg"); 
          oled.printf("Someone designed a\nnew palette!");
          break;
        case 5:
          tval=po_root["switchVal"];
          oled.println("Tripper Trap msg");
          if (po_root["launchVal"]) {
            oled.println("Launch Sequence\ninitiated!!!!!!");
          } else if (po_root["switchVal"]) {
            oled.println("Tripper Trap Enabled");
          } else {
            oled.println("Tripper Trap Disabled");
          }
        break;
        case 6: //sweep
          oled.println("Sweep enabled");
          oled.print("Switches: ");
          oled.println((uint8_t)po_root["plength"], BIN);
          oled.printf("Rate: %d", po_root["pspeed"]);
          break;

        case 7://random analog
          oled.println("Random Analog Msg");  
          break;

        case 8: // nextPalette
          oled.println("Next Palette");
          break;
  
        case 9: //pro joy
          oled.printf("Joystick Message");
          break;
  
        case 10: //switchRow Message
          oled.println("Switch Row Message");
    //      mainState.rain.enabled = po_root["rainVal"];
    //      mainState.blendwave.enabled = po_root["blendWaveVal"];
    //      mainState.ripple.enabled = po_root["rippleVal"];
    //      mainState.ants.enabled = po_root["antsVal"];
          break;

        case 11: // wiimote
            
          break;
      
        case 12:// wiimote
          break;

        case 13: // disable controller can only be sent by zeus
            oled.println("Disable controller?!?!?");
          break;

        case 14: //go into holding pattern mode can only be sent by zeus
            oled.println("holding pattern?!?!?");
          break;

        case 15:
            oled.println("Tranquility mode");
          break;

        case 16: //ants
            oled.println("Ants!");
         break;
        case 17: //too Flashy can only be sent by zeus
            oled.println("Too Flashy?!?!?");
          break;
        case 18: //patterns off
            oled.println("All Patterns Off");
            oled.println("Ripple should kick in");
          break;
        case 19: //house lights
            oled.println("House Lights");
          break;
        case 99:
          tval=po_root["batteryHealth"];
          oled.print("Battery Health: ");
          oled.println(tval);
          bval = po_root["batteryVoltage"];
          oled.print("Battery Volt: ");
          oled.println(bval);
          tval=po_root["rawValue"];
          oled.print("sense: ");
          oled.println(tval);
        break;
        default :
          oled.println("Unknown message type");
          oled.println(msgp);
        break;
      }
    }
  }
}

void essentialGuardian() {
  if (newMode) {
    oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
    oled.printf("\n      ESSENTIAL\n      GUARDIAN");
    newMode = false;
  } else if (newModeTimer.isItTime()) {
    oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
    secondline.remove(0);
    secondline.concat("Essential Guardian");
    oled.printf("1) Tranquil 2) Random\n3) Tripper 4) Sweep\n5) Ants 6) tooFlashy\n7) Sync 8) Astral\n");
    oled.printf("9) New Palette\n0) House Lights"); 
    newModeTimer.stopTimer();
  }
  switch(buttons) {
   case 0 : //no button pushed
      break; 
   case 1 : //Button "0"   
      houseLightsMessage();
      secondline.remove(0);
      secondline.concat("Button Pushed (0)");
    break;
    case 2 : //Button "1"   
      tranquilityMessage();
      secondline.remove(0);
      secondline.concat("Button Pushed (1)");
    break;
    case 4 : //Button "2" upset_mainState()  
      tripperMessage(false, false);
      secondline.remove(0);
      secondline.concat("Button Pushed (2)");
    break;
    case 8 : //Button "3"   
      tripperMessage(true, false);
      secondline.remove(0);
      secondline.concat("Button Pushed: (3)");
    break;
    case 16 : //Button "4"   
      sweepMessage(60, 6);
      secondline.remove(0);
      secondline.concat("Button Pushed: (4)");
    break;
    case 32 : //Button "5"   
      antsMessage();
      secondline.remove(0);
      secondline.concat("Button Pushed: (5)");
    break;
    case 64 : //Button "6"   
      tooFlashyMessage();
      secondline.remove(0);
      secondline.concat("Button Pushed: (6)");
    break;
    case 128 : //Button "7"  
      upset_mainState();
      copyme_message(); 
      secondline.remove(0);
      secondline.concat("Button Pushed: (7)");
    break;
    case 256 : //Button "8"   
      patternsOffMessage();//this will trigger the ripple to be enabled
      secondline.remove(0);
      secondline.concat("Button Pushed: (8)");
    break;
    case 512 : //Button "9"   
      nextPaletteMessage();
      secondline.remove(0);
      secondline.concat("Button Pushed: (9)");
    break;
    default :
      secondline.remove(0);
      secondline.concat("Button data: ");
      secondline.concat(buttons);
    break;    
   }
  
}

void moonbaseMonitor() {
  if (newMode) {
    oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
    oled.printf("\n       MOONBASE\n      MONITOR");
    newMode = false;
  } else if (newModeTimer.isItTime()) {
    oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
    secondline.remove(0);
    secondline.concat("Moonbase Monitor");
    newModeTimer.stopTimer();
  } else if (newMessageTimer.isItTime() || buttons == 1) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\nBattery Level: %d\nBattery monitor:\n  ", batteryHealth);
      if (screamerOnline) {
        oled.print("Online");
      } else {
        oled.print("OFFLINE!");
      }
      newMessageTimer.stopTimer();
  }
}

void shapeShifter() {
  static uint8_t audience = 0;
  bool enabledVal = true;
  bool messagingOnlyVal = true;
  bool newMessage = false;
  
  if (newMode) {
    oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
    oled.printf("\n          SHAPE\n       SHIFTER");
    newMode = false;
  } else if (newModeTimer.isItTime()) {
    oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
    secondline.remove(0);
    secondline.concat("Shape Shifter");
    oled.printf("     %s\n1) Disable Messages\n2) Enable Messages\n4) Disable Controller\n5) Enable Controller\n0) Next", shapeNames[audience].c_str());
    newModeTimer.stopTimer();
  }
  switch(buttons) {
   case 0 : //no button pushed
      break; 
   case 1 : //Button "0"   
      audience = (audience + 1) % 8;
      mapMessage(0x1<<audience, 7);
      mapTimer.startTimer();
      oled.clear(0, oled.displayWidth(), 2, 2);
      oled.printf("     %s", shapeNames[audience].c_str());
      secondline.remove(0);
      secondline.concat("Button Pushed (0)");
    break;
    case 2 : //Button "1" 
      controllerMessage(audience, true, false);
      secondline.remove(0);
      secondline.concat("Button Pushed (1)");
    break;
    case 4 : //Button "2" upset_mainState()  
      controllerMessage(audience, true, true);
      secondline.remove(0);
      secondline.concat("Button Pushed (2)");
    break;
    case 16 : //Button "4"   
      controllerMessage(audience, false, false);
      secondline.remove(0);
      secondline.concat("Button Pushed: (4)");
    break;
    case 32 : //Button "5"  
      controllerMessage(audience, false, true);
      secondline.remove(0);
      secondline.concat("Button Pushed: (5)");
    break;
    default :
      secondline.remove(0);
      secondline.concat("Button data: ");
      secondline.concat(buttons);
    break;    
   }
}

void patternRecognition(){
  
}

void codeMaster() {
  static uint8_t konamiProgress = 0;
  static uint8_t piProgress = 0;
  static uint8_t countProgress = 0;
  static uint8_t count2Progress = 0;
  static uint8_t battery50Progress = 0;
  static uint8_t battery30Progress = 0;
  static uint8_t battery10Progress = 0;
  if (newMode) {
    oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
    oled.printf("\n        CODE\n        MASTER");
    newMode = false;
    secondline.remove(0);
    secondline.concat("Code Master");
  }
  if (buttons != 0) {
    //konami code: launch Sequence
    if ((konamiProgress == 0 && buttons == 4) ||
        (konamiProgress == 1 && buttons == 4) ||
        (konamiProgress == 2 && buttons == 256) ||
        (konamiProgress == 3 && buttons == 256) ||
        (konamiProgress == 4 && buttons == 16) ||
        (konamiProgress == 5 && buttons == 64) ||
        (konamiProgress == 6 && buttons == 16)
        ){
      konamiProgress++;
    } else if (konamiProgress == 7 && buttons == 64) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\n\n    LAUNCH!!!!!");    
      tripperMessage(true, true);  
      konamiProgress = 0;
    } else {
      konamiProgress = 0;
    }

    //PI to 8 digits: enlightenment
    if ((piProgress == 0 && buttons == 8) ||
        (piProgress == 1 && buttons == 2) ||
        (piProgress == 2 && buttons == 16) ||
        (piProgress == 3 && buttons == 2) ||
        (piProgress == 4 && buttons == 32) ||
        (piProgress == 5 && buttons == 512) ||
        (piProgress == 6 && buttons == 4)
        ){
      piProgress++;
    } else if (piProgress == 7 && buttons == 64) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\n\n    ENLIGHTENMENT!\n in T-60");
      enlightenMessage(3);  
      piProgress = 0;
    } else {
      piProgress = 0;
    }

    //1,1,2,2,3,3,4,4 - enable holding pattern lockdown
    if ((countProgress == 0 && buttons == 2) ||
        (countProgress == 1 && buttons == 2) ||
        (countProgress == 2 && buttons == 4) ||
        (countProgress == 3 && buttons == 4) ||
        (countProgress == 4 && buttons == 8) ||
        (countProgress == 5 && buttons == 8) ||
        (countProgress == 6 && buttons == 16)
        ){
      countProgress++;
    } else if (countProgress == 7 && buttons == 16) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\n\n    Holding Pattern\nEnabled");
      holdingPatternMessage(1);  
      countProgress = 0;
    } else {
      countProgress = 0;
    }

    //5,5,6,6,7,7,8,8 - disable holding pattern
    if ((count2Progress == 0 && buttons == 32) ||
        (count2Progress == 1 && buttons == 32) ||
        (count2Progress == 2 && buttons == 64) ||
        (count2Progress == 3 && buttons == 64) ||
        (count2Progress == 4 && buttons == 128) ||
        (count2Progress == 5 && buttons == 128) ||
        (count2Progress == 6 && buttons == 256)
        ){
      count2Progress++;
    } else if (count2Progress == 7 && buttons == 256) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\n\n    Holding Pattern\nDisabled");
      holdingPatternMessage(0);  
      count2Progress = 0;
    } else {
      count2Progress = 0;
    }

    //5,5,5,5,5,5,5,5 - set guardian costume battery to 50%
    if ((battery50Progress == 0 && buttons == 32) ||
        (battery50Progress == 1 && buttons == 32) ||
        (battery50Progress == 2 && buttons == 32) ||
        (battery50Progress == 3 && buttons == 32) ||
        (battery50Progress == 4 && buttons == 32) ||
        (battery50Progress == 5 && buttons == 32) ||
        (battery50Progress == 6 && buttons == 32)
        ){
      battery50Progress++;
    } else if (battery50Progress == 7 && buttons == 32) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\n\n    Guardian Power\n   50%%");
      batteryHealth = 49;
      battery50Progress = 0;
    } else {
      battery50Progress = 0;
    }

    //3,3,3,3,3,3,3,3 - guardian battery at 30%
    if ((battery30Progress == 0 && buttons == 8) ||
        (battery30Progress == 1 && buttons == 8) ||
        (battery30Progress == 2 && buttons == 8) ||
        (battery30Progress == 3 && buttons == 8) ||
        (battery30Progress == 4 && buttons == 8) ||
        (battery30Progress == 5 && buttons == 8) ||
        (battery30Progress == 6 && buttons == 8)
        ){
      battery30Progress++;
    } else if (battery30Progress == 7 && buttons == 8) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\n\n    Guardian Power\n   30%%");
      batteryHealth = 39;
      battery30Progress = 0;
    } else {
      battery30Progress = 0;
    }

    //1,1,1,1,1,1,1,1 - guardian battery at 10%
    if ((battery50Progress == 0 && buttons == 32) ||
        (battery50Progress == 1 && buttons == 32) ||
        (battery50Progress == 2 && buttons == 32) ||
        (battery50Progress == 3 && buttons == 32) ||
        (battery50Progress == 4 && buttons == 32) ||
        (battery50Progress == 5 && buttons == 32) ||
        (battery50Progress == 6 && buttons == 32)
        ){
      battery50Progress++;
    } else if (battery50Progress == 7 && buttons == 32) {
      oled.clear(0, oled.displayWidth(), 2, oled.displayRows());
      oled.printf("\n\n    Guardian Power\nCritically Low!\n   10%%");
      batteryHealth = 29;
      battery50Progress = 0;
    } else {
      battery50Progress = 0;
    }

    
  }
  
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

void enlightenMessage(uint8_t enVal) { //send the data from the enlightenment button
  String tripperMsgTemplate = "{\"poMessageType\": 5,  \"enlightenVal\": 3,  \"audience\": 0xFF,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(tripperMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 2;
  nm_root["audience"] = 0xff;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_root["enlightenVal"] = enVal;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
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

  nextPalette();
  
  nm_root["poMessageType"] = 8;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["paletteIndex"] = paletteCycleIndex;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

void controllerMessage(uint8_t audienceVal, bool messagingVal, bool enabledVal) {
  String controllerMsgTemplate = "{\"poMessageType\": 13, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0, \"messagingOnly\": true, \"enabled\": true}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(controllerMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 13;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["audience"] = audienceVal;
  nm_root["messagingOnly"] = messagingVal;
  nm_root["enabled"] = enabledVal;
  
  
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

void houseLightsMessage(){
  String houseLightsMsgTemplate = "{\"poMessageType\": 19, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(houseLightsMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 19;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
  
}

void holdingPatternMessage(uint8_t holdingVal) {
  String holdingMsgTemplate = "{\"poMessageType\": 14, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0, \"holdVal\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(holdingMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["poMessageType"] = 14;
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["holdVal"] = holdingVal;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

