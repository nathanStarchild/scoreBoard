//************************************************************
// Moonbase Code 2018
// Location: Taygeta
// Shape: Stellated Tetrahedron
// LEDs: 6 x 2m strips
// Interactivity: Hold For Enlightenment
//************************************************************

//************************************************************
//includes

//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 57;
const uint8_t nStrips = 6;
const uint16_t num_leds = stripLength * nStrips;
//uint16_t enlightenTime = 60000;
#define buttonPin 0    // The enlightenment Button pin
//************************************************************

//************************************************************
//Global variables
int directionLR[nStrips] = {1, 1, 1, 1, 1, 1};
int directionUD[nStrips] = {1, -1, 1, -1, 1, -1};
int directionIO[nStrips] = {1, -1, 1, -1, 1, -1};
int stripDirection[nStrips] = {1, -1, 1, -1, 1, -1};
uint16_t audienceSpot = 3;
uint16_t sweepSpot = 3;

MilliTimer enlightenButtonCheck(25);

uint8_t lastEnlightenVal = 0;
//************************************************************

//************************************************************
//functions
uint8_t checkEnlightenButton();
void enlightenmentCallback(uint8_t b);
void enlightenmentMessage(uint8_t b);

void pleiadianSetup() {
    pinMode(buttonPin,INPUT_PULLUP);
    enlightenment.stopTimer();
}

void checkInputs() {
  if (enlightenButtonCheck.isItTime()){
    uint8_t b = checkEnlightenButton();
    enlightenmentCallback(b);
    if (b != lastEnlightenVal && b!=0){
      enlightenmentMessage(b);
      lastEnlightenVal = b;
    }
    enlightenButtonCheck.resetTimer();
  }
}

uint8_t checkEnlightenButton() { 
  // Button timing variables
  static int debounce = 25;          // ms debounce period to prevent flickering when pressing or releasing the button
  static int DCgap = 250;            // max ms between clicks for a double click event
  static int holdTime = 5000;        // ms hold period: how long to wait for press+hold event
  
  // Button variables
  static boolean buttonVal = HIGH;   // value read from button
  static boolean buttonLast = HIGH;  // buffered value of the button's previous state
  static boolean DCwaiting = false;  // whether we're waiting for a double click (down)
  static boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
  static boolean singleOK = true;    // whether it's OK to do a single click
  static long downTime = -1;         // time the button was pressed down
  static long upTime = -1;           // time the button was released
  static boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
  static boolean waitForUp = false;        // when held, whether to wait for the up event
  static boolean holdEventPast = false;    // whether or not the hold event happened already
  static boolean holdCancel = false;

   uint8_t event = 0;
   long now = millis();
   buttonVal = digitalRead(buttonPin);
   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (now - upTime) > debounce)
   {
       downTime = now;
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;

//       Serial.println((now-upTime));
//       Serial.println(DConUp);

       if ((now-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (now - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = now;
           if (DConUp == false){ 
            DCwaiting = true;
           }
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
       if (holdCancel){
          event = 5;
          holdCancel = false;
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (now-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (now - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       } else {
//        event = 4;
        holdCancel = true;
       }
   }
   buttonLast = buttonVal;
   if (event!=0){
//   Serial.println(event);
   }
   return event;
}


void enlightenmentMessage(uint8_t b) { //send the data from the enlightenment button
  String enlightenMsgTemplate = "{\"poMessageType\": 2, \"enlightenVal\": 0, \"audience\": 0,  \"timestart\": 123456789, \"hue\": 124, \"patternStep\": 0}";
 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(enlightenMsgTemplate);

  uint32_t gotime = mesh.getNodeTime() + INTERVAL + INTERVAL;
  
  mainState.lastUpdate = mainState.nextUpdate;
  mainState.nextUpdate = gotime;
  
  nm_root["timestart"] = gotime;
  nm_root["hue"] = mainState.hue;
  nm_root["poMessageType"] = 2;
  nm_root["enlightenVal"] = b;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}


