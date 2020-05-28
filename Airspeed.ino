#define AIRSPEEDS_MS      200
#define AIRSPEEDS_MAX     998
#define AIRSPEEDS_SHIFT   520
#define AIRDENSITY        1.225 // 1.225 kg/m^3
#define MPS2KNOTS         1.94  // m/s to knots
#define MPS2KMH           1.61  // m/s to km/s

//////////////
// Airspeed //
//////////////
void Airspeed (void)
{
  int   airspeedSValue = 0;
  float airPressure = 0;
  
  if (millis() - AirspeedSTick < AIRSPEEDS_MS)
  {
     R_IAS_Sensor = analogRead(AirspeedPin);
     AirspeedSBuffer += R_IAS_Sensor;
     AirspeedSCounter++;
  }
  else
  {
    airspeedSValue = AirspeedSBuffer / AirspeedSCounter;

    // Max pressure for MPXV7002 is 2Kpa (112 knots)
    //Map sensor data between AirspeedSensorShift and AirspeedSensorMax to pressure between 0 and 2000 Pa
    //airPressure = map(airspeedSValue, AIRSPEEDS_SHIFT, AIRSPEEDS_MAX, 0, 2000);

    // Max pressure for MPXV5010DP is 10Kpa (250 knots)
    //Map sensor data between AirspeedSensorShift and AirspeedSensorMax to pressure between 0 and 10000 Pa
    airPressure = map(airspeedSValue, AIRSPEEDS_SHIFT, AIRSPEEDS_MAX, 0, 10000);

    if (airPressure < 0)
      airPressure = 0;

    #if (_SERIAL_DEBUG_ == 1)
    Serial.print("Air Pressure: ");
    Serial.println(airPressure);
    #endif

    //IAS = MPS2KNOTS * sqrt(2*airPressure/AIRDENSITY);
    IAS = MPS2KMH * sqrt(2*airPressure/AIRDENSITY);

    #if (_SERIAL_DEBUG_ == 1)
    Serial.print("Air Speed: ");
    Serial.print(IAS);
    //Serial.println(" knots");
    Serial.println(" km/h");
    #endif

    if (IAS < 10)
     IAS = 0;
    
    AirspeedSTick = millis();
    AirspeedSBuffer = 0;
    AirspeedSCounter = 0;
  }
}
