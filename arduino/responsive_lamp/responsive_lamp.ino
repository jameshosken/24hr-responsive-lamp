/*
  NeoPixel Candle using ColorConverter

  This sketch uses the Adafruit DotStar library to drive a strip of
  WS2812 LEDs and the ColorConverter library to to HSI to RGB conversion.
  The pixels should fade slowly from orange to red and back cyclically.

  This assumes you're using RGB NeoPixels and not RGBW NeoPixels.

  created 28 Jun 2018
  by Tom Igoe
*/
#include <Adafruit_NeoPixel.h>
#include <ColorConverter.h>
#include "secrets.h"
#include "colours.h"

#define NUMPIX 16
#define PIXPIN 6

/////////////////
// WIFI CONFIG //
/////////////////

#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoHttpClient.h>

bool debug_verbose = true;
int demoCounter = 0;

const char ssid[] = secret_ssid;      //  your network SSID (name)
const char pass[] = secret_password;   // your network password

int status = WL_IDLE_STATUS;
WiFiServer server(80);

// HUE CONFIG
//const char hueHubIP[] = "192.168.1.5";
const char hueHubIP[] = "128.122.151.168";
const String hueUserName = "Ra52iwspJn7fBcU8kKzsaGSqiASydGCkOkPDJqRH"; // hue bridge READINGname
const int bulb = 5;

WiFiClient wifi;
HttpClient httpClient = HttpClient(wifi, hueHubIP);

bool isConnected = false;

//RTC CONFIG
#include <WiFiUdp.h>
#include <RTCZero.h>

RTCZero rtc;
int GMT = -4; //change this to adapt it to your time zone

/////////////////////
// END WIFI CONFIG //
/////////////////////

///////////////////
// TIMING CONFIG //
///////////////////

int currentHour = -1;

///////////////////////
// END TIMING CONFIG //
///////////////////////



//NEOPIX
const int neoPixelPin = 6;
const int pixelCount = 16;    // number of pixels
int change = 1;              // increment to change hue by
//END NEOPIX

//INTERACTIONS

//Define enum for how the lamp is controlled: via timer, READING mode, or CANDLE mode (no connection)
enum control {
  TIMING,
  READING,
  CANDLE,
  SUNRISE,
  SUNSET,
  DEMO
};



control controlSystem = TIMING;
control prevControlSystem = CANDLE; //Useful for detecting changes

// Keep data from server to determine behaviour,
// Keep prev data from server to determine if change has been made.
char userControlData = ' ';
char prevUserControlData = ' ';

const char TIMINGChar = 'T';
const char READINGChar = 'R';
const char CANDLEChar = 'C';
const char SUNRISEChar = 'M';   //Morning
const char SUNSETChar = 'E';    //Evening
const char DEMOChar = 'D';    //Evening  
/////////////////////
//END INTERACTIONS //
/////////////////////

////////////////////////
// CANDLE (CANDLE MODE) //
////////////////////////

// bruPeriod, briMin, briMax, huePeriod, hueMin, hueMax
int candleValues[NUMPIX][6];


// set up strip:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIX, PIXPIN, NEO_GRB + NEO_KHZ800);
ColorConverter converter;

//int h = 10;         // hue
//int s = 100;        // saturation
//int i = 100;        // intensity

int hsi[NUMPIX][3];

void setup() {
  pixSetup();
  setupWiFi();
}


void loop() {

  if (isConnected) {
    connectedLoop();
  } else {
    disconnectedLoop();
  }

  // wait 4 seconds
}

void connectedLoop() {

  //Listen for server
  //Take value and parse it into behaviour
  handleServer();

  handleUserControlData();

  //Handle Update

  switch (controlSystem) {
    case TIMING:
      handleTimingControl();
      break;
    case READING:
      handleREADINGControl();
      break;
    case CANDLE:
      handleCANDLEControl();
      break;
    case SUNSET:
      handleSUNSETControl();
      break;
    case SUNRISE:
      handleSUNRISEControl();
      break;
    case DEMO:
      handleDEMOControl();
      break;
    default:
      break;
  }


  //Use behaviour from above to determine

  //Case DEFAULT (Candle behaviour)

  //Case FADING (fade to val)



  // wait 4 seconds
  //sendRequest(bulb, "on", "false");  // turn light off

}

void handleTimingControl() {
  if(prevControlSystem != controlSystem){
    int timingHSB[3] = HSB_NIGHT;
    pixFadeOut();
    setPhilipsHSB(timingHSB[0], timingHSB[1], timingHSB[2]); 
    prevControlSystem = controlSystem;
  }
  
  //Update every minute //Todo revert to minute
  if (millis() % 10000 == 0) {
    int h = getHour();
    if (h != currentHour) {
      currentHour = h;
      Serial.println(">>>>>>>>> Hour Change");
      handleTimingColours(currentHour);
    }
    delay(1); //only run once this millisecond

  }
}

void handleDEMOControl() {
  if(prevControlSystem != controlSystem){
    int timingHSB[3] = HSB_NIGHT;
    pixFadeOut();
    setPhilipsHSB(timingHSB[0], timingHSB[1], timingHSB[2]); 
    prevControlSystem = controlSystem;
  }
  
  //Update every minute //Todo revert to minute
  if (millis() % 10000 == 0) {
    demoCounter = (demoCounter + 1) % 24;
    handleTimingColours(demoCounter);
    delay(1); //only run once this millisecond

  }
}

void handleCANDLEControl() {
  if(prevControlSystem != controlSystem){
    //Changed!
    currentHour = -1;
    Serial.println("HANDLING CANDLE");
    sendRequest(bulb, "on", "true");
    int candleHSB[3] = HSB_CANDLE;
    setPhilipsHSB(candleHSB[0], candleHSB[1], candleHSB[2]); 
    prevControlSystem = controlSystem;
  }
  updateCandle();
}

void handleREADINGControl() {
  if(prevControlSystem != controlSystem){
    
    //Changed!
    currentHour = -1;
    Serial.println("HANDLING READING");
    sendRequest(bulb, "on", "true");
    int readingHSB[3] = HSB_WW;
    setPhilipsHSB(readingHSB[0], readingHSB[1], readingHSB[2]); 
    prevControlSystem = controlSystem;
    pixFadeOut();
  }
}

void handleSUNRISEControl() {
  if(prevControlSystem != controlSystem){
    //Changed!
    currentHour = -1;
    Serial.println("HANDLING SUNRISE");
    sendRequest(bulb, "on", "true");
    int sunriseHSB[3] = HSB_SUNRISE;
    setPhilipsHSB(sunriseHSB[0], sunriseHSB[1], sunriseHSB[2]);   
    pixSetSunrise();
    prevControlSystem = controlSystem;
  }
}

void handleSUNSETControl() {
  if(prevControlSystem != controlSystem){
    //Changed!
    currentHour = -1;
    Serial.println("HANDLING SUNRISE");
    sendRequest(bulb, "on", "true");
    int sunsetHSB[3] = HSB_SUNSET;
    setPhilipsHSB(sunsetHSB[0], sunsetHSB[1], sunsetHSB[2]); 
    pixSetSunset();
    prevControlSystem = controlSystem;
  }
}


void handleUserControlData() {
  if (userControlData != prevUserControlData) {
    Serial.println("\n>\n>\n>Change of control system");
    prevUserControlData = userControlData;

    switch (userControlData) {
      case TIMINGChar:
        Serial.println("USING TIMING CONTROL");
        controlSystem = TIMING;
        break;
      case READINGChar:
        Serial.println("USING READING CONTROL");
        controlSystem = READING;
        break;
      case CANDLEChar:
        Serial.println("USING CANDLE CONTROL");
        controlSystem = CANDLE;
        break;
      case SUNRISEChar:
        Serial.println("USING SUNRISE CONTROL");
        controlSystem = SUNRISE;
        break;
      case SUNSETChar:
        Serial.println("USING SUNSET CONTROL");
        controlSystem = SUNSET;
        break;
      case DEMOChar:
        Serial.println("USING SUNSET CONTROL");
        controlSystem = DEMO;
        break;
      default:
        Serial.println("NOT USEFUL DATA");
        //Probably a behaviour char, do nothing.
        break;
    }
  }
}

void disconnectedLoop() {
  handleCANDLEControl();
}




