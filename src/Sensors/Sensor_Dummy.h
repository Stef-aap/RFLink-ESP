
// Version 0.3, 12-12-2019, SM
//     - OLED display support added
// Version 0.2, 29-07-2019, SM
//     - Get_JSON_Data removed from loop
// Version 0.1

#ifndef Sensor_Dummy_h
#define Sensor_Dummy_h 0.3

#include "Sensor_Base.h"

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_Dummy : public _Sensor_BaseClass {

public:
  String Device_Firmware = "deviceID My first sensor";

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_Dummy(int Display_X = -1, int Display_Y = -1) {
    this->_Display_X = Display_X;
    this->_Display_Y = Display_Y;
    Version_Name = "V" + String(Sensor_Dummy_h) + "  ------  Sensor_Dummy.h";
    Serial.println("\n------  CREATE  ------  " + Version_Name);
    _JSON_Short_Header = "Dummy Sawtooth\tDummy_Noise\t";
    _JSON_Long_Header = _JSON_Short_Header;
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    _Saw_Tooth = 0; // random ( 10000 ) ;
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    _Saw_Tooth += 1;
    _Saw_Tooth %= 99;
    int Noise = random(0, 100);

    JSON_Data += " \"Dummy_Sawtooth\":";
    JSON_Data += String(_Saw_Tooth);
    JSON_Data += ",";
    JSON_Data += " \"Dummy_Noise\":";
    JSON_Data += String(Noise);
    JSON_Data += ",";

    JSON_Short_Data += String(_Saw_Tooth);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(Noise);
    JSON_Short_Data += "\t";
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  unsigned int _Saw_Tooth = 0;
  int _Display_X;
  int _Display_Y;
};

#endif
