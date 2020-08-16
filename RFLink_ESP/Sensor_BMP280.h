// Version 0.3
//    - Domoticz output toegevoegd
//
// Version 0.2
//    - Get_JSON_Data,  commented out, has to be chenged
//

#ifndef Sensor_BMP280_h
#define Sensor_BMP280_h 0.3

#include "Sensor_Base.h"
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_BMP280 : public _Sensor_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // My_Number should be either 1 ... 2  (I2C Address = 0x75 + My_Number)
  // or the real I2C Address  0x76, 0x77
  // ***********************************************************************
  _Sensor_BMP280(int My_Address = 0x76) {
    _I2CAddress = My_Address;
    Serial.println("V" + String(Sensor_BMP280_h) + "   Sensor_BMP280_h");
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    switch (_I2CAddress) {
      case 1:
        _I2CAddress = 0x76;
        break;
      case 2:
        _I2CAddress = 0x77;
        break;
    }
    bool Status = _BMP.begin(_I2CAddress);
    if (not Status) {
      Serial.print("No BMP280 found at address : 0x");
      char s[4];
      sprintf(s, "%02x", _I2CAddress);
      Serial.println(s);
      return;
    }
    Print_Help();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.print("_Sensor_BMP280 ( 0x");
    char s[4];
    sprintf(s, "%02x", _I2CAddress);
    Serial.print(s);
    Serial.println(" )        <<<< Current");
    Serial.println("_Sensor_BMP280  ( int I2C_Address = 0x76 )\n\
    I2C_Address = the real I2C address or\n\
                  1 for 0x76 (default), 2 for 0x77");
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"BMP280_Temperature\":";
    JSON_Data += String(_BMP.readTemperature());
    JSON_Data += ", \"BMP280_Pressure\":";
    JSON_Data += String((int)(_BMP.readPressure() / 100));
    JSON_Data += ",";
  }

#ifdef YES_INCLUDE_RECEIVER_LUFTDATEN
  // ***********************************************************************
  // Get all the sampled data as a partial JSON string, suited for Luftdaten
  // ***********************************************************************
  String Get_JSON_LuftData() {
    // ********************************************************
    // it's absolutly necessairy to read temperature first
    // because it's needed for an accurate pressure measurement
    // ********************************************************
    _JSON_Sample = "";
    _JSON_Sample += Value2Json("BMP280_temperature", String(_BMP.readTemperature()));
    _JSON_Sample += Value2Json("BMP280_pressure", String((int)_BMP.readPressure()));
    return _JSON_Sample;
  }
#endif

  // ***********************************************************************
  // ***********************************************************************
  /*
    bool Send_2_Domoticz () {
      int  IDx = 20 ;

      String Line = String (_BMP.readTemperature ()) + ";" ;
      Line += String ( (int)(_BMP.readPressure()/100) ) + ";0;0" ;

      return Send_1Par_Domoticz ( IDx, Line ) ;
    }
*/

  // ***********************************************************************
private:
  // ***********************************************************************
  Adafruit_BMP280 _BMP;
  int _I2CAddress;
};

#endif
