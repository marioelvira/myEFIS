
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "e2prom.h"
#include "http.h"
#include "io.h"
#include "ip.h"
#include "main.h"
#include "wifi.h"

///////////////////
// IO definition //
///////////////////
int ioOut;
int ioLed;
int ioIn;

/////////////////////
// Airspeed Sensor //
/////////////////////
unsigned long AirspeedSTick = 0;
int AirspeedPin = A0;   // TODO
int AirspeedSBuffer = 0;
int AirspeedSCounter = 0;

int IAS = 0;            // Airspeed

/////////////////////
// Flight recorder //
/////////////////////
int R_IAS_Sensor = 0;
int R_IAS_avg = 0;
int R_QNH = 1013;
int R_ALT_Current = 0;
int R_ALT_avg = 0;
int R_VSI_avg = 0;

///////////
// Wi-Fi //
///////////
unsigned long wifiAPTick = 0;
unsigned long wifiLEDTick = 0;

///////////
// Wi-Fi //
///////////

#if (_WRITE_SSID_EEPROM_ == 1)
const char* ssidSt = SSIDST;
const char* passwordSt = PSKST;
#endif

// Station Mode
char ssid[WIFI_SSID_MAX];
char password[WIFI_PSWD_MAX];

IPAddress wifiIP;

// Accesspoint Mode
const char* ssidAp = SSIDAP;
const char* passwordAp = PSKAP;

int wifiStatus;
int wifiMode;

////////////////
// IP Address //
////////////////
int       ipMode;

IPAddress ipAddress (192, 168, 43, 200);
IPAddress gateWay   (192, 168, 43, 1);
IPAddress netMask   (255, 255, 255, 0);

byte mac[6];

/////////////////
// Device Name //
/////////////////
#define DEVICENAME      "myEFIS"
char* deviceName = DEVICENAME;

/////////////////
// Http server //
/////////////////
ESP8266WebServer httpServer(HTTP_PORT);

String outState = "OFF";
String inState = "OFF";

int httpStatus;

//////////
// JSON //
//////////
unsigned long jsonCurrentTime = millis();
unsigned long jsonPreviousTime = 0;

int airSpeed = 34;
int rollAngle = 30;
int pitchAngle = 50;
int altitute = 1500;
int QNH = 1023;
int turnAngle = 10;
int heading = 90;
int vario = 2;


//============//
// MAIN SETUP //
//============//
void setup(void)
{ 
  // IO setup
  _IOSetup();
  
  #if (_SERIAL_DEBUG_ == 1)
  delay(5000);  // 5 secs
  Serial.begin(115200);
  Serial.println("");
  #endif

  // Config setup
  _ConfigSetup();

  // Wi-Fi setup
  _WifiSetup();

  // Http setup
  _HttpSetup();
}

//===========//
// MAIN LOOP //
//===========//
void loop()
{
  _IOLoop();
  _WifiLoop();

  _WifiLedLoop();
  _JsonLoop();

  if ((wifiStatus == WIFI_ON_ACCESSPOINT) ||
      (wifiStatus == WIFI_STATION_CONNECTED))
  {
    _HttpLoop();
  }
}
