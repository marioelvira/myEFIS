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
