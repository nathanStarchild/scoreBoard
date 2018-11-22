//************************************************************
// Pleiadian Oasis Code 2018
// Location: Battery
// Shape: none
// LEDs: none
// Interactivity: health status
//************************************************************

//************************************************************
//includes
//#define OLED_SCREEN
#define ALWAYS_INPUTS
#define I2C_ADDRESS 0x3C
#include <Wire.h>
//#include "SSD1306Ascii.h"
//#include "SSD1306AsciiWire.h"
//#include "ESP8266WiFi.h"
//#include <SparkFunSX1509.h> // Include SX1509 library
//************************************************************

//************************************************************
//Compile time Constants
const uint8_t stripLength = 1;
const uint8_t nStrips = 1;
const uint16_t num_leds = stripLength * nStrips;
//************************************************************

//functions
void batteryMessage(int sensorValue);

//************************************************************
//Global variables
int directionLR[nStrips] = {1};
int directionUD[nStrips] = {1};
int directionIO[nStrips] = {1};
int stripDirection[nStrips] = {1};
uint16_t audienceSpot = 9;
uint8_t sweepSpot = 9;
//uint16_t messagetally=0;
////const char* SSID = "PleiadianOasis";
//String SSID = MESH_SSID;

MilliTimer checkVoltage(11000);
//float voltageLevel=0;


float mv;
float mh;
float bv;
float bh;
//************************************************************


void pleiadianSetup() {
 //set analog input
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V):
  float voltage = sensorValue * (3.2 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);

  
  //from calibration tests
  //measured voltage (by multimeter) v_1 = 12.71v
  //sensor value s_1 = 162
  //v_2 = 5v
  //s_2 = 63
  // v = m*s + b
  //m = (v_2 - v_1)/(s_2 - s_1) = 0.0778788
  //b = v_1 - m*s_1 = 0.093636;

  //health is based on the assumption that 12.9V = 100% full
  //and 9.5V is 0%;
  // H = m*v + b
  // m = 100/(12.9 - 9.5) = 29.41176
  // b = -(m*9.5) = -279.41176


  //calibrate the sensor by recording two sensor readings at two different voltages
  //v = m * s + b
  float v1 = 12.71;
  int s1 = 162;
  float v2 = 5;
  int s2 = 63;
  Serial.printf("%f %f %d %d %f %f\n", v1, v2, s1, s2, v1 - v2, float(s1 - s2));

  mv = (v1 - v2) / (float)(s1 - s2);
  bv = v1 - (mv * s1);
  Serial.printf("mv: %f, bv: %f\n", mv, bv);

  //define the battery health by entering a valtage for 100% and 0% health
  //h = m * v + b
  int h1 = 100;
  v1 = 12.9;
  int h2 = 0;
  v2 = 9.5;

  mh = (float)((h1 - h2))/(v1 - v2);
  bh = h1 - (mh * v1);
  Serial.printf("mh: %f, bh: %f\n", mh, bh);

  Serial.printf("10.5V = %f%% health\n", (mh * 10.5 + bh));
  
}

void checkInputs() {

  if( checkVoltage.isItTime() ) {
  checkVoltage.resetTimer();
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V):
  //float voltage = (sensorValue - 161) * 10 / 7;
  // print out the value you read:
  //voltageLevel = (uint8_t)voltage;
  batteryMessage( sensorValue );  
  }
}


void batteryMessage(int sensorValue) { 
  String batteryMsgTemplate = "{\"poMessageType\": 99,  \"timestart\": 123456789, \"batteryHealth\": 99, \"batteryVoltage\": 99, \"rawValue\": 99 }";
  uint32_t gotime = mesh.getNodeTime();
  char voltString[5] = "    ";
  DynamicJsonBuffer jsonBuffer;
  JsonObject& nm_root = jsonBuffer.parseObject(batteryMsgTemplate);

  float voltage = mv * sensorValue + bv;
  int health = (int) (mh * voltage + bh);
  
  dtostrf(voltage, 5, 2, voltString);
  if(voltString[0]==' ') {
    voltString[0]='0';
  }

  Serial.printf("Sensor Value: %d, Voltage: %f, Health: %d\n", sensorValue, voltage, health);
  
  nm_root["batteryHealth"] = health;
  nm_root["batteryVoltage"] = voltString;
  nm_root["rawValue"] = sensorValue;
  nm_root["timestart"] = gotime;
  
  nm_msgp.remove(0);
  nm_root.printTo(nm_msgp);
  outbox = true;
}

