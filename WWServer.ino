
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

///////////
// Wi-Fi //
///////////
unsigned long wifiCurrentTime = millis();
unsigned long wifiPreviousTime = 0;

unsigned long wifiCurrentAPTime = millis();
unsigned long wifiPreviousAPTime = 0;

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
#define DEVICENAME      "device_"// "deviceHostal.com"  // TODO
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

void _ConfigSetup(void)
{
  #if (_READ_EEPROM_ == 1)
  _ReadEEPROM();
  delay(1000);
  #endif
    
  // Config setup
  #if (_ERASE_EEPROM_ == 1)
    _ResetEEPROM();
    delay(1000);
  #endif

  // Read config from EEPROM
  _readCONFIG();

  #if (_READ_EEPROM_ == 1)
  delay(1000);
  _ReadEEPROM();
  #endif
}

void _readCONFIG (void)
{
  int i;
  #if (_WRITE_SSID_EEPROM_ == 1)
  int j;
  #endif
  byte val[4];
  
  EEPROM.begin(512);
    
  i = EEPROM.read(EEPROM_ADD_OK);
  
  // Si NO esta grabada la configuracion...
  if (i != EEPROM_VAL_OK)
  {
    #if (_SERIAL_DEBUG_ == 1)
    Serial.println("Config NOK");
    #endif
  
    EEPROM.write(EEPROM_ADD_OK,      EEPROM_VAL_OK);

    EEPROM.write(EEPROM_ADD_IP_MODE, EEPROM_VAL_IP_MODE);
    EEPROM.write(EEPROM_ADD_IP1,     EEPROM_VAL_IP1);
    EEPROM.write(EEPROM_ADD_IP2,     EEPROM_VAL_IP2);
    EEPROM.write(EEPROM_ADD_IP3,     EEPROM_VAL_IP3);
    EEPROM.write(EEPROM_ADD_IP4,     EEPROM_VAL_IP4);
    EEPROM.write(EEPROM_ADD_MASK1,   EEPROM_VAL_MASK1);
    EEPROM.write(EEPROM_ADD_MASK2,   EEPROM_VAL_MASK2);
    EEPROM.write(EEPROM_ADD_MASK3,   EEPROM_VAL_MASK3);
    EEPROM.write(EEPROM_ADD_MASK4,   EEPROM_VAL_MASK4);
    EEPROM.write(EEPROM_ADD_GATE1,   EEPROM_VAL_GATE1);
    EEPROM.write(EEPROM_ADD_GATE2,   EEPROM_VAL_GATE2);
    EEPROM.write(EEPROM_ADD_GATE3,   EEPROM_VAL_GATE3);
    EEPROM.write(EEPROM_ADD_GATE4,   EEPROM_VAL_GATE4);
  
    EEPROM.write(EEPROM_ADD_WIFI_MODE, EEPROM_VAL_WIFI_MODE);
  
    for (i = 0; i < WIFI_SSID_MAX; i++)
      EEPROM.write(EEPROM_ADD_WIFI_SSID + i, 0);

    #if (_WRITE_SSID_EEPROM_ == 1)
    j = strlen(ssidSt);
    for (i = 0; i < j; i++)
      EEPROM.write(EEPROM_ADD_WIFI_SSID + i, ssidSt[i]);
    #endif

    for (i = 0; i < WIFI_PSWD_MAX; i++)
      EEPROM.write(EEPROM_ADD_WIFI_PSWD + i, 0);

    #if (_WRITE_SSID_EEPROM_ == 1)
    j = strlen(passwordSt);
    for (i = 0; i < j; i++)
      EEPROM.write(EEPROM_ADD_WIFI_PSWD + i, passwordSt[i]);
    #endif

    EEPROM.commit();    //Store data to EEPROM
  }
  else
  {
    #if (_SERIAL_DEBUG_ == 1)
    Serial.println("Config OK");
    #endif
  }
  
  // IP Mode
  ipMode = EEPROM.read(EEPROM_ADD_IP_MODE);
  
  #if (_SERIAL_DEBUG_ == 1)
  if (ipMode == FIXIP_MODE)
    Serial.println("IP Mode: FIX IP");
  else
    Serial.println("IP Mode: DHCP");
  #endif
  
  if (ipMode == FIXIP_MODE)
  {
    // Ip Address
    val[0] = EEPROM.read(EEPROM_ADD_IP1);
    val[1] = EEPROM.read(EEPROM_ADD_IP2);
    val[2] = EEPROM.read(EEPROM_ADD_IP3);
    val[3] = EEPROM.read(EEPROM_ADD_IP4);
    ipAddress = IPAddress(val[0], val[1], val[2], val[3]);
    
    // Mask
    val[0] = EEPROM.read(EEPROM_ADD_MASK1);
    val[1] = EEPROM.read(EEPROM_ADD_MASK2);
    val[2] = EEPROM.read(EEPROM_ADD_MASK3);
    val[3] = EEPROM.read(EEPROM_ADD_MASK4);
    netMask = IPAddress(val[0], val[1], val[2], val[3]);

    // Gateway
    val[0] = EEPROM.read(EEPROM_ADD_GATE1);
    val[1] = EEPROM.read(EEPROM_ADD_GATE2);
    val[2] = EEPROM.read(EEPROM_ADD_GATE3);
    val[3] = EEPROM.read(EEPROM_ADD_GATE4);
    gateWay = IPAddress(val[0], val[1], val[2], val[3]);
  }

  #if (_SERIAL_DEBUG_ == 1)
  Serial.print("IP: ");
  Serial.print(ipAddress);
  Serial.print(" Mask: ");
  Serial.print(netMask);
  Serial.print(" Gateway: ");
  Serial.println(gateWay);
  #endif
  
  // Wi-Fi Mode
  wifiMode = EEPROM.read(EEPROM_ADD_WIFI_MODE);

  #if (_SERIAL_DEBUG_ == 1)
  if (wifiMode == STATION_MODE)
    Serial.println("Wi-Fi Mode: Station");
  else
    Serial.println("Wi-Fi Mode: Access point");
  #endif

  if (wifiMode == STATION_MODE)
  {
    for (i = 0; i < WIFI_SSID_MAX; i++)
      ssid[i] = char(EEPROM.read(EEPROM_ADD_WIFI_SSID + i));

    #if (_SERIAL_DEBUG_ == 1)
    Serial.print("SSID: ");
    Serial.println(ssid);
    #endif

    for (i = 0; i < WIFI_SSID_MAX; i++)
      password[i] = char(EEPROM.read(EEPROM_ADD_WIFI_PSWD + i));

    #if (_SERIAL_DEBUG_ == 1)
    Serial.print("PASSWORD: ");
    Serial.println(password);
    #endif
  }

}

void _ResetEEPROM() {

  #if (_SERIAL_DEBUG_ == 1)
  Serial.println("Erasing E2PROM 512 size... ");
  #endif

  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0xFF);
  }
  
  EEPROM.commit();    //Store data to EEPROM
}

void _ReadEEPROM() {

  int j = 0;

  #if (_SERIAL_DEBUG_ == 1)
  Serial.println("Reading E2PROM 512 size... ");
  #endif

  EEPROM.begin(512);
   
  for (int i = 0; i < 512; i++) {

    #if (_SERIAL_DEBUG_ == 1)
    j = EEPROM.read(i);
    if (j < 16)
      Serial.print("0");
    Serial.print(j, HEX);
    #endif
  }

  #if (_SERIAL_DEBUG_ == 1)
  Serial.println(" ");
  Serial.println("End ...");
  #endif  
}

void _WifiSetup(void)
{
  // Wi-Fi status
  wifiStatus = WIFI_START_ACCESSPOINT;
}

void _HttpSetup(void)
{
  // Http status
  httpStatus = HTTP_INIT;
}

void _HttpEnd(void)
{
  httpServer.stop();
}

//////////////
// IO steup //
//////////////
void _IOSetup(void)
{ 
  pinMode(DI_LED, OUTPUT);
  digitalWrite(DI_LED, DI_OFF);
  ioLed = IO_OFF;

  pinMode(DI_OUT, OUTPUT);
  digitalWrite(DI_OUT, DI_OFF);
  ioOut = IO_OFF;

  pinMode(DI_IN, INPUT);
  ioIn = IO_OFF;
}

//////////////////////
// IO state machine //
//////////////////////
void _IOLoop()
{
  if (ioLed == IO_OFF)
    digitalWrite(DI_LED, DI_OFF);
  else
    digitalWrite(DI_LED, DI_ON);

  if (ioOut == IO_OFF)
  digitalWrite(DI_OUT, DI_OFF);
  else
    digitalWrite(DI_OUT, DI_ON);

  if (digitalRead(DI_IN))
    ioIn = IO_OFF;
  else
    ioIn = IO_ON;
}

/////////////////////////
// Wi-Fi state machine //
/////////////////////////
void _WifiLoop()
{
  switch (wifiStatus)
  {
    case WIFI_START_ACCESSPOINT:

      #if (_SERIAL_DEBUG_ == 1)
      Serial.println("Access point mode start...");
      Serial.print("SSID: ");
      Serial.println(ssidAp);
      Serial.print("PASSWORD: ");
      Serial.println(passwordAp);
      #endif
    
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssidAp, passwordAp);
  
      wifiIP = WiFi.softAPIP();
      
      #if (_SERIAL_DEBUG_ == 1)
      Serial.print("AP IP address: ");
      Serial.println(wifiIP);
      #endif

      // Http setup
      _HttpSetup();

      wifiCurrentAPTime = millis();
      wifiPreviousAPTime = wifiCurrentAPTime;
        
      wifiStatus = WIFI_ON_ACCESSPOINT;
      break;

    case WIFI_ON_ACCESSPOINT:
      // Station mode...
      if (wifiMode == STATION_MODE)
      {       
        wifiCurrentAPTime = millis();
        if (wifiCurrentAPTime - wifiPreviousAPTime >= WIFI_ACCESSPOINT_TIMEOUT)
        {
          _HttpEnd();

          wifiStatus = WIFI_START_STATION;
        }
      }
      // Access point mode...
      else
      {
        wifiCurrentAPTime = millis();
        wifiPreviousAPTime = wifiCurrentAPTime;
      }      
      break;
    
    case WIFI_START_STATION:
    
      WiFi.begin(ssid, password);
      WiFi.macAddress(mac);
     
      WiFi.disconnect();          // Prevent connecting to wifi based on previous configuration
      WiFi.hostname(deviceName);  // DHCP Hostname (useful for finding device for static lease)
    
      if (ipMode == FIXIP_MODE)
        WiFi.config(ipAddress, netMask, gateWay);

      WiFi.begin(ssid, password);
      WiFi.mode(WIFI_STA);        // WiFi mode station (connect to wifi router only)

      #if (_SERIAL_DEBUG_ == 1)
      Serial.println("Station Mode start...");
      Serial.print("SSID: ");
      Serial.println(ssid);
      Serial.print("PASSWORD: ");
      Serial.println(password);
      #endif

      wifiStatus = WIFI_STATION_CONNECTING;
      break;
      
    case WIFI_STATION_CONNECTING:
      // If no connected...
      if (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
    
        #if (_SERIAL_DEBUG_ == 1)
        Serial.print(".");
        #endif
      }
      // If connected...
      else
      {

        ipAddress = WiFi.localIP();
        gateWay   = WiFi.gatewayIP();
        netMask   = WiFi.subnetMask();
                
        #if (_SERIAL_DEBUG_ == 1)
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        if (ipMode == FIXIP_MODE)
          Serial.println("Fix IP Mode");
        else
          Serial.println("DHCP Mode");
        Serial.print("IP address: ");
        Serial.println(ipAddress);
        Serial.print("Mask: ");
        Serial.println(netMask);
        Serial.print("Gateway: ");
        Serial.println(gateWay);
        Serial.print("MAC: ");
        Serial.print(mac[5],HEX);
        Serial.print(":");
        Serial.print(mac[4],HEX);
        Serial.print(":");
        Serial.print(mac[3],HEX);
        Serial.print(":");
        Serial.print(mac[2],HEX);
        Serial.print(":");
        Serial.print(mac[1],HEX);
        Serial.print(":");
        Serial.println(mac[0],HEX);
        #endif

        // Http setup
        _HttpSetup();
      
        wifiStatus = WIFI_STATION_CONNECTED;
      }
      break;

    case WIFI_STATION_CONNECTED:
      // If no connected...
      if (WiFi.status() != WL_CONNECTED)
      {
        _HttpEnd();
        
        wifiStatus = WIFI_START_STATION;
      }

      break;
  }
}

/////////////////////////
// Wi-Fi state machine //
/////////////////////////
void _WifiLedLoop()
{
  switch (wifiStatus)
  {
    case WIFI_START_ACCESSPOINT:
      wifiCurrentTime = millis();
      wifiPreviousTime = wifiCurrentTime;
      ioLed = IO_OFF;
      break;

    case WIFI_ON_ACCESSPOINT:
      wifiCurrentTime = millis();
      if (wifiCurrentTime - wifiPreviousTime >= WIFI_BLINK_ACCESSPOINT)
      {
        if (ioLed == IO_OFF)
          ioLed = IO_ON;
        else
          ioLed = IO_OFF;

        wifiPreviousTime = wifiCurrentTime;

        #if (_SERIAL_DEBUG_ == 1)
        if (wifiMode == STATION_MODE)
          Serial.print(".");
        #endif
      } 
      break;

    case WIFI_START_STATION:
    case WIFI_ON_STATION:
    case WIFI_STATION_CONNECTING:
      wifiCurrentTime = millis();
      if (wifiCurrentTime - wifiPreviousTime >= WIFI_BLINK_STATION)
      {
        if (ioLed == IO_OFF)
          ioLed = IO_ON;
        else
          ioLed = IO_OFF;

        wifiPreviousTime = wifiCurrentTime;
      } 
      break;

    case WIFI_STATION_CONNECTED:
      ioLed = IO_ON;
      break;
  }
}


///////////
// Pages //
///////////

void _serveCSS()
{
  httpServer.send (200, "text/css", cssSTYLE);
}

void _serveMAIN()
{
  String html = "";
  
  html = "<!DOCTYPE HTML><html>";
  html = html + "<title>Network Settings</title>";
  html = html + "<head>";
  html = html + "<link rel=\"icon\" href=\"data:,\">";
  html = html + "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />";
  html = html + "</head>";

  html = html + "<body>";
  html = html + "<div class=\"myform\">";
  html = html + "<h1>MyEFIS+ #Status<span>ESP8266 EFIS project</span></h1>";

  html = html + "<p>IN Status <span id=\"INValue\">...</span></p>";
  html = html + "<p>OUT Status <span id=\"OUTValue\">...</span></p>";
  html = html + "  <p><input type=\"button\" value=\"Change\" onclick=\"sendOUT(2)\"><p>";
  html = html + "  <p><a href=\"settings.htm\"><input type=\"button\" value=\"Settings\"></a><p>";
  html = html + "</div>";
  html = html + "<script>";
  html = html + "function sendOUT(out) {";
  html = html + "  var xhttp = new XMLHttpRequest();";
  html = html + "  xhttp.onreadystatechange = function() {";
  html = html + "    if (this.readyState == 4 && this.status == 200) {";
  html = html + "      document.getElementById(\"OUTState\").innerHTML = this.responseText;";
  html = html + "   }";
  html = html + "  };";
  html = html + "  xhttp.open(\"GET\", \"setOUT?OUTstate=\"+out, true);";
  html = html +  "  xhttp.send();";
  html = html + "}";
  html = html + "setInterval(function() {";
  html = html + "  getOUT();";
  html = html + "  getIN();";
  html = html +  "}, 1000);";
  html = html +  "function getOUT() {";
  html = html +  "  var xhttp = new XMLHttpRequest();";
  html = html + "  xhttp.onreadystatechange = function() {";
  html = html + "    if (this.readyState == 4 && this.status == 200) {";
  html = html + "      document.getElementById(\"OUTValue\").innerHTML = this.responseText;";
  html = html + "   }";
  html = html +  "  };";
  html = html + "  xhttp.open(\"GET\", \"readOUT\", true);";
  html = html + "  xhttp.send();";
  html = html + "}";
  html = html + "function getIN() {";
  html = html + "  var xhttp = new XMLHttpRequest();";
  html = html + "  xhttp.onreadystatechange = function() {";
  html = html + "    if (this.readyState == 4 && this.status == 200) {";
  html = html +  "      document.getElementById(\"INValue\").innerHTML = this.responseText;";
  html = html + "   }";
  html = html + "  };";
  html = html + "  xhttp.open(\"GET\", \"readIN\", true);";
  html = html +  "  xhttp.send();";
  html = html + "}";
  html = html + "</script>";

  html = html + "</form>";
  html = html + "</div>";
  
  html = html + "</body> ";
  html = html + "</html>";

  httpServer.send (200, "text/html", html);
}

void _serveSETTINGS()
{
  //int mobile = 0;
  String html = "";

  int n = WiFi.scanNetworks();
  
  html = "<!DOCTYPE HTML><html>";
  html = html + "<title>Network Settings</title>";
  html = html + "<head>";
  html = html + "<link rel=\"icon\" href=\"data:,\">";
  html = html + "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />";
  //html = html + "<meta name='apple-mobile-web-app-capable' content='yes' />";
  //html = html + "<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />";
  html = html + "</head>";

  html = html + "<body>";
  html = html + "<div class=\"myform\">";
  html = html + "<h1>MyEFIS+ #Network settings<span>ESP8266 EFIS project</span></h1>";
  //html = html + "<form method=\"post\">";
  html = html + "<form method='get' action='networSettings'>";

  // Wi-Fi
  html = html + "<div class=\"section\"><span>1</span>Wi-Fi Settings</div>";
  html = html + "<div class=\"inner-wrap\">";

  if (wifiMode == STATION_MODE)
  {
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"ap\"> Access Point</label>";
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"st\" checked> Station</label>";
  }
  else
  {
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"ap\" checked> Access Point</label>";
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"st\"> Station</label>";
  }
  
  //html = html + "<label>SSID">
  //html = html + "<select id='wifi' name='wifi'>";
  //html = html + " <option value=\"\" selected>Selet</option>";
  //for (int i = 0; i < n; ++i)
  //  html = html + " <option value=\"" + (String)(WiFi.SSID(i)) + "\">" + (String)(WiFi.SSID(i)) + "</option>";    
  //html = html + "</select></label>";

  html = html + "<label>SSID <input type=\"text\" maxlength=\"30\" value=\"" + String(ssid) + "\" name=\"ssid\"/></label>";
  html = html + "<label>Password <input type=\"text\" maxlength=\"30\" value=\"" + String(password) + "\" name=\"pass\"/></label>";

  html = html + "</div>";
  // End

  // Red
  html = html + "<div class=\"section\"><span>2</span>IP Settings</div>";
  html = html + "<div class=\"inner-wrap\">";

  if (ipMode == DHCP_MODE)
  {
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"dhcp\" checked> DHCP</label>";
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"ipfx\"> Fix IP</label>";
  }
  else
  {
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"dhcp\"> DHCP</label>";
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"ipfx\" checked> Fix IP</label>";
  }

  html = html + "<label>IP Address <input type=\"text\"  maxlength=\"16\" value=\"" + String(ipAddress.toString()) + "\" name=\"ipaddress\"/></label>";
  html = html + "<label>Mask <input type=\"text\" maxlength=\"16\" value=\"" + String(netMask.toString()) + "\" name=\"mask\"/></label>";
  html = html + "<label>Gateway <input type=\"text\" maxlength=\"16\" value=\"" + String(gateWay.toString()) + "\" name=\"gateway\"/></label>";

  html = html + "</div>";
  // End
                        
  html = html + "<div class=\"button-section\">";
  html = html + "  <input type=\"submit\" value=\"Save\">";
  html = html + "  <a href=\"index.htm\"><input type=\"button\" value=\"Back\"></a>";
  html = html + "</div>";
  
  html = html + "</div>";
  html = html + "</div>";
  html = html + "</form>";
  html = html + "</div>";

  html = html + "</body> ";
  html = html + "</html>";

  httpServer.send (200, "text/html", html);
  
}

void _serveJSON()
{
  String json= "";

  json = "[";
  
  json = json + "{ \"param\":\"airSpeed\", \"value\":" + String(airSpeed) + " }";
  json = json + ",";
  json = json + "{ \"param\":\"rollAngle\", \"value\":" + String(rollAngle) + " }";
  json = json + ",";
  json = json + "{ \"param\":\"pitchAngle\", \"value\":" + String(pitchAngle) + " }";
  json = json + ",";
  json = json + "{ \"param\":\"altitute\", \"value\":" + String(altitute) + " }";
  json = json + ",";
  json = json + "{ \"param\":\"QNH\", \"value\":" + String(QNH) + " }";
  json = json + ",";
  json = json + "{ \"param\":\"turnAngle\", \"value\":" + String(turnAngle) + " }";
  json = json + ",";
  json = json + "{ \"param\":\"heading\", \"value\":" + String(heading) + " }";
  json = json + ",";
  json = json + "{ \"param\":\"vario\", \"value\":" + String(vario) + " }";

  json = json + "]";

  httpServer.sendHeader("Access-Control-Allow-Origin","*");
  httpServer.send (200, "application/json", json);
}

void _setSETTINGS()
{
  String rwmode = httpServer.arg("wifimode");
  String rssid = httpServer.arg("ssid");
  String rpass = httpServer.arg("pass");
  
  String ripmode = httpServer.arg("ipmode");
  String ripaddress = httpServer.arg("ipaddress");
  String rmask = httpServer.arg("mask");
  String rgate = httpServer.arg("gateway");
    
  String content;
  int i, j, k, m;
  int error = 0;
  char schar;
  char sbuf[4];
  byte val[4];

  IPAddress localip;
  IPAddress localmask;
  IPAddress localgate;

  // Wi-Fi and IP configuration
  if (rwmode.length() > 0)
  {
    // Wi-Fi on Station mode
    if (rwmode == "st")
    {
      // must have a value
      if (rssid.length() == 0)
        error = 1;

      if (ripmode.length() == 0)
        error = 1;

      // must have a value
      if (ripmode == "ipfx")
      {
        // Must be 
        if ((ripaddress.length() == 0) ||
            (rmask.length() == 0)      ||
            (rgate.length() == 0))
          error = 1;
      }
      // DHCP mode
      //else {}
      
    }
    // Wi-Fi Access Point mode
    //else {}
    
  }
  else
    error = 1;

  // If no error on data...
  if (error == 0)
  {
     /////////////////////////
     // Wi-Fi configuration //
     /////////////////////////
     #if (_SERIAL_DEBUG_ == 1)
     Serial.println("Wi-Fi 2 eeprom:");
     #endif
     
     // AP mode
     if (rwmode == "ap")
       EEPROM.write(EEPROM_ADD_WIFI_MODE, ACCESSPOINT_MODE);
     else
     {
       // Station mode
       EEPROM.write(EEPROM_ADD_WIFI_MODE, STATION_MODE);
  
       // ssid
       for (i = 0; i < WIFI_SSID_MAX; i++)
         EEPROM.write(EEPROM_ADD_WIFI_SSID + i, 0);
       j = rssid.length();
       for (i = 0; i < j; i++)
         EEPROM.write(EEPROM_ADD_WIFI_SSID + i, rssid[i]);
       // password
       for (i = 0; i < WIFI_PSWD_MAX; i++)
         EEPROM.write(EEPROM_ADD_WIFI_PSWD + i, 0);
       j = rpass.length();
       for (i = 0; i < j; i++)
         EEPROM.write(EEPROM_ADD_WIFI_PSWD + i, rpass[i]);
       
       /////////////
       // IP Mpde //
       /////////////
       #if (_SERIAL_DEBUG_ == 1)
       Serial.println("IP 2 eeprom:");
       #endif
       // mode
       if (ripmode == "dhcp")
         EEPROM.write(EEPROM_ADD_IP_MODE, DHCP_MODE);
       else
       {
         EEPROM.write(EEPROM_ADD_IP_MODE, FIXIP_MODE);
  
         // IP
         j = ripaddress.length();
         k = 0;
         m = 0;
         for (i = 0; i < j; i++)
         {
           schar = ripaddress[i];
           if (schar == '.')
           {        
             sbuf[k] = 0;  // end of buffer
             val[m] = (byte)(atoi(sbuf)); // change to int
             k = 0;
             m++;
           }
           else
           {
             sbuf[k] = schar;
             k++;
           }
         }
         // last IP value
         sbuf[k] = 0;  // end of buffer
         val[m] = (byte)(atoi(sbuf)); // change to int
         localip = IPAddress(val[0], val[1], val[2], val[3]);
    
         EEPROM.write(EEPROM_ADD_IP1, val[0]);
         EEPROM.write(EEPROM_ADD_IP2, val[1]);
         EEPROM.write(EEPROM_ADD_IP3, val[2]);
         EEPROM.write(EEPROM_ADD_IP4, val[3]);
         
         // Mask
         j = rmask.length();
         k = 0;
         m = 0;
         for (i = 0; i < j; i++)
         {
           schar = rmask[i];
           if (schar == '.')
           {
             sbuf[k] = 0;  // end of buffer
             val[m] = (byte)(atoi(sbuf)); // change to int
             k = 0;
             m++;
           }
           else
           {
             sbuf[k] = schar;
             k++;
           }
         }
         // last IP value
         sbuf[k] = 0;  // end of buffer
         val[m] = (byte)(atoi(sbuf)); // change to int
         localmask = IPAddress(val[0], val[1], val[2], val[3]);
    
         EEPROM.write(EEPROM_ADD_MASK1, val[0]);
         EEPROM.write(EEPROM_ADD_MASK2, val[1]);
         EEPROM.write(EEPROM_ADD_MASK3, val[2]);
         EEPROM.write(EEPROM_ADD_MASK4, val[3]);
    
         // Gateway
         j = rgate.length();
         k = 0;
         m = 0;
         for (i = 0; i < j; i++)
         {
           schar = rgate[i];
           if (schar == '.')
           {
             sbuf[k] = 0;  // end of buffer
             val[m] = (byte)(atoi(sbuf)); // change to int
             k = 0;
             m++;
           }
           else
           {
             sbuf[k] = schar;
             k++;
           }
         }
         // last IP value
         sbuf[k] = 0;  // end of buffer
         val[m] = (byte)(atoi(sbuf)); // change to int
         localgate = IPAddress(val[0], val[1], val[2], val[3]);
    
         EEPROM.write(EEPROM_ADD_GATE1, val[0]);
         EEPROM.write(EEPROM_ADD_GATE2, val[1]);
         EEPROM.write(EEPROM_ADD_GATE3, val[2]);
         EEPROM.write(EEPROM_ADD_GATE4, val[3]);
       }

     }
     
     #if (_SERIAL_DEBUG_ == 1)
     // Wi-Fi configuration
     Serial.print("---->Wi-Fi mode: ");
     Serial.println(rwmode);
     Serial.print("---->Wi-Fi SSID: ");
     Serial.println(rssid);
     Serial.print("---->Wi-Fi Password: ");
     Serial.println(rpass);
     Serial.println("");

     // IP configuration
     Serial.print("---->Local IP: ");
     Serial.println(localip);
     Serial.print("---->Local mask: ");
     Serial.println(localmask);
     Serial.print("---->Local gateway: ");
     Serial.println(localgate);
     #endif
     
     EEPROM.commit();

     #if (_READ_EEPROM_ == 1)
     _ReadEEPROM();
     delay(1000);
     #endif
    
     // Read config from EEPROM
     _readCONFIG();

     // OK
     i = 200;
   }
   else
   {
     #if (_SERIAL_DEBUG_ == 1)
     Serial.println("Sending 404");
     #endif

     // Error
     i = 404;
   }

   content = "<!DOCTYPE html><html>";
   content += "<title>Settings</title>";
   
   if (i == 200)
     content += "<p>Settings OK: Saved</p>";
   else
     content += "<p>Settings Error: Not Saved</p>";

   content += "<a href=\"index.htm\"><< Back</a>";
   content += "</html>";

   httpServer.send(200, "text/html", content);
}

void _readIN()
{
  if (ioIn == IO_ON)
   inState = "ON";
  else
   inState = "OFF";
   
  httpServer.send(200, "text/plane", inState);
}

void _readOUT()
{
  if (ioOut == IO_ON)
   outState = "ON";
  else
   outState = "OFF";
   
  httpServer.send(200, "text/plane", outState);
}
 
void _setOUT()
{
 String t_state = httpServer.arg("OUTstate"); //Refer  xhttp.open("GET", "setOUT?OUTstate="+out, true);
 
 #if (_SERIAL_DEBUG_ == 1)
 Serial.println(t_state);
 #endif

 if(t_state == "2")
 {
   if (ioOut == IO_ON)
   {
     ioOut = IO_OFF;
     outState = "OFF";
   }
   else
   {
     ioOut = IO_ON;
     outState = "ON";
   }
 }
 else
 {
   ioOut = IO_OFF;
   outState = "OFF";
 }
 
 httpServer.send(200, "text/plane", outState); //Send web page
}

////////////////////////
// Http state machine //
////////////////////////
void _HttpLoop()
{
  switch (httpStatus)
  {
    case HTTP_INIT:

      // css Style
      httpServer.on("/style.css",       _serveCSS);

      // html pages
      httpServer.on("/",                _serveMAIN);
      httpServer.on("/index.htm",       _serveMAIN);
      httpServer.on("/settings.htm",    _serveSETTINGS);

      // acctions
      httpServer.on("/setOUT",          _setOUT);
      httpServer.on("/readOUT",         _readOUT);
      httpServer.on("/readIN",          _readIN);
      httpServer.on("/networSettings",  _setSETTINGS);

      // Json data
      httpServer.on("/data.json",       _serveJSON);

      httpServer.begin();

      #if (_SERIAL_DEBUG_ == 1)
      Serial.print("HTTP server started on ");
      Serial.print(HTTP_PORT);
      Serial.println(" port");      
      #endif
      httpStatus = HTTP_ONSERVE;
      break;
    
    case HTTP_ONSERVE:
      httpServer.handleClient();
      break;
  }
}

void _JsonLoop ()
{
  jsonCurrentTime = millis();
  if (jsonCurrentTime - jsonPreviousTime >= JSON_TICK)
  {
    airSpeed++;
    if (airSpeed > 100)
      airSpeed = 20;
    
    rollAngle++;
    if (rollAngle > 30)
      rollAngle = 0;
    
    pitchAngle++;
    if (pitchAngle > 30)
      pitchAngle = 0;

    altitute++;
    if (altitute > 10000)
      altitute = 1000;
    
    QNH++;
    if (QNH > 1030)
      QNH = 990;    
    
    turnAngle++;
    if (turnAngle > 30)
      turnAngle = -30;
    
    heading++;
    if (heading > 350)
      heading = 10;
    
    //vario++;
    //if (vario > 2);
    //  vario = 0;
        
    jsonPreviousTime = jsonCurrentTime;
  } 
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
