
// Version 0.2,  15-08-2019, SM
//    - JSON_Short_Data added

#ifndef Sensor_Witty_LDR_h
#define Sensor_Witty_LDR_h 0.2

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_Witty_LDR : public _Sensor_BaseClass {
public:
  // ***********************************************************************
  // Creators
  // ***********************************************************************
  _Sensor_Witty_LDR() {
    Version_Name = "V" + String(Sensor_Witty_LDR_h) + "   Sensor_Witty_LDR.h";
    Serial.println("CREATE    " + Version_Name);

    _JSON_Short_Header = "LDR\t";
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    int LDR = analogRead(A0);

    JSON_Data += " \"LDR\":";
    JSON_Data += LDR;
    JSON_Data += ",";

    JSON_Short_Data += String(LDR);
    JSON_Short_Data += "\t";
  }
};
#endif
