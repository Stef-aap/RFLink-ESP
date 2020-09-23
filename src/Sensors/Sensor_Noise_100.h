
// Version 0.3
//   - external watchdog support added
//
// Version 0.2
//   - loop-function removed
//   - Get_JSON_Data added
//
// Version 0.1

#ifndef Sensor_Noise_100_h
#define Sensor_Noise_100_h 0.3

#include "Sensor_Base.h"

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_Noise_100 : public _Sensor_BaseClass {
public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_Noise_100() {
    Version_Name = "V" + String(Sensor_Noise_100_h) + "   Sensor_Noise_100.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"Noise_100\":";
    JSON_Data += random(-100, 100);
    JSON_Data += ",";
  }
};

#endif
