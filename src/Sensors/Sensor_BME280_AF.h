
// Version 0.8, 02/26/2020, SM ...
//    - Removed time_ms in JSON
//
// Version 0.7
//    - Temperature compensation is also possible with creation
//    - RH compensation based on T compensation
//
// Version 0.6
//    - Temperature compensation
//
// Version 0.5
//    - added file storage support
//
// Version 0.4
//    - Domoticz output added
//
// Version 0.3, 07/26/2018, SM, checked by ...
//    - Get_JSON_Data appends directly to a global string
//
// Version 0.2

#ifndef Sensor_BME280_AF_h
#define Sensor_BME280_AF_h 0.8

#include "Sensor_Base.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h> // ??

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_BME280_AF : public _Sensor_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // My_Number should be either 1 ... 2  (I2C Address = 0x75 + My_Number)
  // or the real I2C Address  0x76, 0x77
  // ***********************************************************************
  _Sensor_BME280_AF(char *Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }

  _Sensor_BME280_AF(int My_Address = 0x76, int T_Compensation = 0) {
    _I2CAddress = My_Address;
    this->_T_Compensation = T_Compensation;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    Version_Name = "V" + String(Sensor_BME280_AF_h) + "  ======  Sensor_BME280_AF.h";
    Serial.println("CREATE    " + Version_Name);

    _JSON_Short_Header = "BME_T\tBME_P\tBME_RH\t";
    _JSON_Long_Header = "BME280_Temperature\tBME280_Pressure\tBME280_Humidity\t";
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
    bool Status = _BME.begin(_I2CAddress);
    if (not Status) {
      Serial.print("No BME280 found at address : 0x");
      char s[4];
      sprintf(s, "%02x", _I2CAddress);
      Serial.println(s);
      Wire_Print();
    }
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _I2CAddress = Settings.Get_Set_Default_Int("BME280 I2C Address", 0x76, Force);
    _T_Compensation = Settings.Get_Set_Default_Int("BME280 Temp Compensation", -3, Force);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int I2C_Address = Settings.Read_Int("BME280 I2C Address");
    _T_Compensation = Settings.Read_Int("BME280 Temp Compensation");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "BME280 I2C Address") {
        if (New_Value_Int != I2C_Address) {
          _My_Settings_Buffer["BME280 I2C Address"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "BME280 Temp Compensation") {
        if (New_Value_Int != _T_Compensation) {
          _My_Settings_Buffer["BME280 Temp Compensation"] = New_Value_Int;
          _T_Compensation = New_Value_Int;
          Restart = false;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.print("I2C-Address    = 0x");
    char s[4];
    sprintf(s, "%02x", _I2CAddress);
    Serial.println(s);

    Serial.print("T-Compensation = ");
    Serial.println(_T_Compensation);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    float Temperature = _BME.readTemperature() + _T_Compensation;
    int Pressure = (int)(_BME.readPressure() / 100);
    int Humidity = (int)_BME.readHumidity();

    if (this->_T_Compensation != 0) {
      if (Humidity < 40) {
        Humidity -= 2 * this->_T_Compensation;
      } else if (Humidity < 50) {
        Humidity -= 2.5 * this->_T_Compensation;
      } else {
        Humidity -= 3 * this->_T_Compensation;
      }
    }

    JSON_Data += " \"BME280_Temperature\":";
    JSON_Data += String(Temperature);
    JSON_Data += ", \"BME280_Pressure\":";
    JSON_Data += String(Pressure);
    JSON_Data += ", \"BME280_Humidity\":";
    JSON_Data += String(Humidity);
    JSON_Data += ",";

    JSON_Short_Data += String(Temperature, 1);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(Pressure);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(Humidity);
    JSON_Short_Data += "\t";
  }

#ifdef INCLUDE_RECEIVER_LUFTDATEN
  // ***********************************************************************
  // Get all the sampled data as a partial JSON string, suited for Luftdaten
  // ***********************************************************************
  String Get_JSON_LuftData() {
    // ********************************************************
    // it's absolutly necessairy to read temperature first
    // because it's needed for an accurate pressure measurement
    // ********************************************************
    _JSON_Sample = "";
    _JSON_Sample += Value2Json("BME280_temperature", String(_BME.readTemperature()));
    _JSON_Sample += Value2Json("BME280_pressure", String((int)_BME.readPressure()));
    _JSON_Sample += Value2Json("BME280_humidity", String((int)_BME.readHumidity()));
    return _JSON_Sample;
  }
#endif

  // ***********************************************************************
private:
  // ***********************************************************************
  Adafruit_BME280 _BME;
  int _I2CAddress;
  int _T_Compensation = 0;
};

#endif
