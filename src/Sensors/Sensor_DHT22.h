// ***********************************************************************************
// Version 0.2, 13-06-2019, SM, checked by ...
//    - Get_Name_Version () added
//    - JSON_Short_Data added
//
// Version 0.1, 06-05-2019, SM
//   - initial version

#ifndef Sensor_DHT22_h
#define Sensor_DHT22_h 0.2

#include "DHTesp.h"

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_DHT22 : public _Sensor_BaseClass {

public:
  DHTesp dht;
  int DHT_ms = 2000;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_DHT22() {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_DHT22(int DHT_Pin) {
    this->_DHT_Pin = DHT_Pin;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    Version_Name = "V" + String(Sensor_DHT22_h) + "  ======  Sensor_DHT22.h";
    Serial.println("CREATE    " + Version_Name);
    this->_JSON_Short_Header = "DHT22_T\tDHT22_RH\t";
    this->_JSON_Long_Header = this->_JSON_Short_Header;
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() { dht.setup(_DHT_Pin, DHTesp::DHT22); }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) { _DHT_Pin = Settings.Get_Set_Default_Int("DHT22 GPIO", -1, Force); }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int DHT_Pin = Settings.Read_Int("DHT22 GPIO");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "DHT22 GPIO") {
        if (New_Value_Int != DHT_Pin) {
          _My_Settings_Buffer["DHT22 GPIO"] = New_Value_Int;
          Restart = true;
          ;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (millis() - _Sample_Time_Last >= DHT_ms) {
      _Sample_Time_Last = millis();

      float Temperature = dht.getTemperature();
      float Humidity = dht.getHumidity();
      _DHT_T += Temperature;
      _DHT_RH += Humidity;
      _NSample += 1;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.print("DHT Pin = ");
    Serial.println(this->_DHT_Pin);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"DHT22_T\":";
    JSON_Data += (_DHT_T / _NSample);
    JSON_Data += ",";
    JSON_Data += " \"DHT22_RH\":";
    JSON_Data += (_DHT_RH / _NSample);
    JSON_Data += ",";

    JSON_Short_Data += String((_DHT_T / _NSample), 1);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(int(_DHT_RH / _NSample));
    JSON_Short_Data += "\t";

    _NSample = 0;
    _DHT_T = 0;
    _DHT_RH = 0;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  unsigned long _Sample_Time_Last;
  int _DHT_Pin;
  float _DHT_T = 0;
  float _DHT_RH = 0;
  int _NSample = 0;
};

#endif
