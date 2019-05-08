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

const char ssid[] = secret_ssid;      //  your network SSID (name)
const char pass[] = secret_password;   // your network password

int status = WL_IDLE_STATUS;
WiFiServer server(80);

// HUE CONFIG
const char hueHubIP[] = "192.168.1.5";
const String hueUserName = "42AoOKmuvkxpCwDeU5F4Z2AhAz3jGYTtpfqRfvIp"; // hue bridge username

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

//Define enum for how the lamp is controlled: via timer, via user, or auto (no connection)
enum control {
  TIMING,
  USER,
  AUTO
};

enum behaviour {
  CANDLE,
  FADING,
  SUNRISE,
  SUNSET,
};

control controlSystem = TIMING;
control prevControlSystem = AUTO; //Useful for detecting changes

// Keep data from server to determine behaviour,
// Keep prev data from server to determine if change has been made.
char userControlData = ' ';
char prevUserControlData = ' ';

const char timingChar = 'T';
const char userChar = 'U';
const char autoChar = 'A';

/////////////////////
//END INTERACTIONS //
/////////////////////

////////////////////////
// AUTO (CANDLE MODE) //
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
    case USER:
      handleUserControl();
      break;
    case AUTO:
      handleAutoControl();
      break;
    default:
      break;
  }


  //Use behaviour from above to determine

  //Case DEFAULT (Candle behaviour)

  //Case FADING (fade to val)



  // wait 4 seconds
  //sendRequest(2, "on", "false");  // turn light off

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

void handleUserControl() {
  if(prevControlSystem != controlSystem){
    currentHour = -1;
    int userHSB[3] = HSB_DEEP_RED;
    setPhilipsHSB(userHSB[0], userHSB[1], userHSB[2]); 
    prevControlSystem = controlSystem;
  }
  
  if (millis() % 5000 == 0) {
    int s = random(0, 100);
    sendRequest(2, "sat", String(s));   // turn light on
    delay(1);
  }
}

void handleAutoControl() {
  if(prevControlSystem != controlSystem){
    //Changed!
    currentHour = -1;
    Serial.println("HANDLING AUTO");
    int autoHSB[3] = HSB_CANDLE;
    setPhilipsHSB(autoHSB[0], autoHSB[1], autoHSB[2]); 
    prevControlSystem = controlSystem;
  }
  
  updateCandle();
 
}


void handleUserControlData() {
  if (userControlData != prevUserControlData) {
    prevUserControlData = userControlData;

    switch (userControlData) {
      case timingChar:
        Serial.println("USING TIMING CONTROL");
        controlSystem = TIMING;
        break;
      case userChar:
        Serial.println("USING USER CONTROL");
        controlSystem = USER;
        break;
      case autoChar:
        Serial.println("USING AUTO CONTROL");
        controlSystem = AUTO;
        break;
      default:
        Serial.println("BEHAVIOUR");
        //Probably a behaviour char, do nothing.
        break;
    }
  }
}

void disconnectedLoop() {
  handleAutoControl();
}




