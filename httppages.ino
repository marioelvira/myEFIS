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
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"ap\" onclick=\"apmode();\"> Access Point</label>";
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"st\" onclick=\"stmode();\" checked> Station</label>";
  }
  else
  {
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"ap\" onclick=\"apmode();\" checked> Access Point</label>";
    html = html + "<label><input type=\"radio\" name=\"wifimode\" value=\"st\" onclick=\"stmode();\"> Station</label>";
  }
  
  html = html + "<label>SSID";
  if (wifiMode == STATION_MODE)
    html = html + "<select id='ssid' name='ssid'>";
  else
    html = html + "<select id='ssid' name='ssid' disabled>";

  if ((String)(ssid) == "")
    html = html + " <option value=\"Select\"> Select </option>";
  else
    html = html + " <option value=\"" + (String)(ssid) + "\">" + (String)(ssid) + "</option>";
  for (int i = 0; i < n; ++i)
  {
    if (String(WiFi.SSID(i)) != String(ssid))
      html = html + " <option value=\"" + (String)(WiFi.SSID(i)) + "\">" + (String)(WiFi.SSID(i)) + "</option>";
  }  
  html = html + "</select></label>";
  
  if (wifiMode == STATION_MODE)
    html = html + "<label>Password <input id='pass' type=\"text\" maxlength=\"30\" value=\"" + String(password) + "\" name=\"pass\"/></label>";
  else
    html = html + "<label>Password <input id='pass' type=\"text\" maxlength=\"30\" value=\"" + String(password) + "\" name=\"pass\"/ disabled></label>";
  
  html = html + "</div>";

  // Red
  html = html + "<div class=\"section\"><span>2</span>IP Settings</div>";
  html = html + "<div class=\"inner-wrap\">";

  if (ipMode == DHCP_MODE)
  {
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"dhcp\" onclick=\"dhcpmode();\" checked> DHCP</label>";
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"ipfx\" onclick=\"fipmode();\"> Fix IP</label>";
    html = html + "<label>IP Address <input id='ipaddress' type=\"text\" maxlength=\"16\" value=\"" + String(ipAddress.toString()) + "\" name=\"ipaddress\"/ disabled></label>";
    html = html + "<label>Mask <input id='mask' type=\"text\" maxlength=\"16\" value=\"" + String(netMask.toString()) + "\" name=\"mask\"/ disabled></label>";
    html = html + "<label>Gateway <input id='gateway' type=\"text\" maxlength=\"16\" value=\"" + String(gateWay.toString()) + "\" name=\"gateway\"/ disabled></label>";
  }
  else
  {
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"dhcp\" onclick=\"dhcpmode();\"> DHCP</label>";
    html = html + "<label><input type=\"radio\" name=\"ipmode\" value=\"ipfx\" onclick=\"fipmode();\" checked> Fix IP</label>";
    html = html + "<label>IP Address <input id='ipaddress' type=\"text\" maxlength=\"16\" value=\"" + String(ipAddress.toString()) + "\" name=\"ipaddress\"/></label>";
    html = html + "<label>Mask <input id='mask' type=\"text\" maxlength=\"16\" value=\"" + String(netMask.toString()) + "\" name=\"mask\"/></label>";
    html = html + "<label>Gateway <input id='gateway' type=\"text\" maxlength=\"16\" value=\"" + String(gateWay.toString()) + "\" name=\"gateway\"/></label>";
  }
  html = html + "</div>";
                        
  html = html + "<div class=\"button-section\">";
  html = html + "  <input type=\"submit\" value=\"Save\">";
  html = html + "  <a href=\"index.htm\"><input type=\"button\" value=\"Back\"></a>";
  html = html + "</div>";
  
  html = html + "</div>";
  html = html + "</div>";

  html = html + "<script>";
  html = html + "function apmode() {";
  html = html + "  document.getElementById('ssid').disabled = true;";
  html = html + "  document.getElementById('pass').disabled = true;";
  html = html + "}";
  html = html + "function stmode() {";
  html = html + "  document.getElementById('ssid').disabled = false;";
  html = html + "  document.getElementById('pass').disabled = false;";
  html = html + "}";
  html = html + "function dhcpmode() {";
  html = html + "  document.getElementById('ipaddress').disabled = true;";
  html = html + "  document.getElementById('mask').disabled = true;";
  html = html + "  document.getElementById('gateway').disabled = true;";
  html = html + "}";
  html = html + "function fipmode() {";
  html = html + "  document.getElementById('ipaddress').disabled = false;";
  html = html + "  document.getElementById('mask').disabled = false;";
  html = html + "  document.getElementById('gateway').disabled = false;";
  html = html + "}";
  
  html = html + "</script>";
  
  html = html + "</form>";
  html = html + "</div>";

  html = html + "</body> ";
  html = html + "</html>";

  httpServer.send (200, "text/html", html);
  
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
