//////////////
// Airspeed //
//////////////
void Airspeed()
{
  int   airspeedSensorValue = 0;
  int   airspeedSensorShift = 520;
  int   airspeedSensorMax = 998;
  float airDensity = 1.2;       // kg/m^3
  float airPressure = 0;
  float mps2knots = 1.94;       // convert meter per seconds to knots

  

/*
  if (AirspeedSensorValueTimer == 0)
  {
    AirspeedSensorValueTimer = millis();
    AirspeedSensorValueBuffer = 0;
    AirspeedSensorValueCount = 0;
  }
  
  // this buffering techique should be changed. 
  // short running vaerage should be better
  if (millis() - AirspeedSensorValueTimer < BufferTimer)
  {
    R_IAS_Sensor = analogRead(AirspeedPin);
    AirspeedSensorValueBuffer += R_IAS_Sensor;
    AirspeedSensorValueCount += 1;
    
  } else {
    
    AirspeedSensorValue = AirspeedSensorValueBuffer / AirspeedSensorValueCount;

    //Serial.print("AirspeedSensorValueCount = ");
    //Serial.println(AirspeedSensorValueCount);
   
    //Max pressure for MPXV7002 is 2Kpa (112 knots)
    //Map sensor data between AirspeedSensorShift and AirspeedSensorMax to pressure between 0 and 2000 Pa
    //AirPressure = map(AirspeedSensorValue, AirspeedSensorShift, AirspeedSensorMax, 0, 2000);

    //Max pressure for MPXV5010DP is 10Kpa (250 knots)
    //Map sensor data between AirspeedSensorShift and AirspeedSensorMax to pressure between 0 and 10000 Pa
    AirPressure = map(AirspeedSensorValue, AirspeedSensorShift, AirspeedSensorMax, 0, 10000);
 
    if (AirPressure < 0)
      AirPressure = 0;

    //Serial.print("Pressure = ");
    //Serial.println(AirPressure);
 
    IAS = mps2knots * sqrt(2*AirPressure/AirDensity);

    R_IAS_avg = IAS;
    //Serial.print("IAS = ");
    //Serial.println(IAS); 

    if (IAS < 20)
     IAS = 0;

    textAreaSPEED.ClearArea();
    textAreaSPEED.Printf("%03d", IAS); 

    AirspeedSensorValueTimer = 0;
  }
  */
}
