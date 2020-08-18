
// Version 0.3

#ifndef Sensor_BME280_h
#define Sensor_BME280_h 0.3

// ***********************************************************************************
// ***********************************************************************************
// Version 0.2, 25-05-2018, SM
//    - initialization wasn't done correctly
//
// Version 0.1, 20-04-2018, SM
//    - initial version
//    - debug_out is used as in the orginal software
// ***********************************************************************************

#include "Sensor_Base.h"
#include "SparkFunBME280.h"

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_BME280_SF : public _Sensor_BaseClass {

public:
  //    String Help_Text = "#define Sensor_BME280_SF  <Address>\n\
    <Address> the real I2C address or\n\
              1 for 0x76 (default), 2 for 0x77" ;

  // ***********************************************************************
  // Creator,
  // My_Number should be either 1 ... 2  (I2C Address = 0x75 + My_Number)
  // or the real I2C Address  0x76, 0x77
  // ***********************************************************************
  _Sensor_BME280_SF() {
    _I2CAddress = 1;
    Help_Text = "#define Sensor_BME280_SF  <Address>\n\
      Serial.println ( " V " + String ( Sensor_BME280_h ) + " Sensor_BME280_h " ) ;
  }
  _Sensor_BME280_SF(int My_Address) {
    _I2CAddress = My_Address;
    Help_Text = "#define Sensor_BME280_SF  <Address>\n\
      Serial.println ( " V " + String ( Sensor_BME280_h ) + " Sensor_BME280_h " ) ;
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    _BME.reset();
    _BME.begin();
    _BME.settings.commInterface = I2C_MODE;
    switch (_I2CAddress) {
      case 1:
        _I2CAddress = 0x76;
        break;
      case 2:
        _I2CAddress = 0x77;
        break;
    }
    _BME.settings.I2CAddress = _I2CAddress;

    // ************************************************
    // Don't know what the defaults are
    // So not sure if all these settings are necessary
    // ************************************************
    //_BME.settings.runMode = 3; //Normal mode
    _BME.settings.runMode = 1; // Forced mode
    _BME.settings.tStandby = 0;
    _BME.settings.filter = 0;
    _BME.settings.tempOverSample = 1;
    _BME.settings.pressOverSample = 1;
    _BME.settings.humidOverSample = 1;
    delay(10);
    _BME.begin();
    Print_Help();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.print("\n#define Sensor_BME280_SF  0x");
    char s[4];
    sprintf(s, "%02x", _I2CAddress);
    Serial.print(s);
    Serial.println("        <<<< Current");
    Serial.println(Help_Text);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"Time_ms\":";
    JSON_Data += String(millis());
    JSON_Data += ", \"BME280_Temperature\":";
    JSON_Data += String(_BME.readTempC());
    JSON_Data += ", \"BME280_Pressure\":";
    JSON_Data += String((int)(_BME.readFloatPressure() / 100));
    JSON_Data += ", \"BME280_Humidity\":";
    JSON_Data += String((int)_BME.readFloatHumidity());
    JSON_Data += ",";
  }

  // ***********************************************************************
  // Get all the sampled data as a partial JSON string, suited for Luftdaten
  // ***********************************************************************
  String Get_JSON_LuftData() {
    // ********************************************************
    // it's absolutly necessairy to read temperature first
    // because it's needed for an accurate pressure measurement
    // ********************************************************
    _JSON_Sample = "";
    _JSON_Sample += Value2Json("BME280_temperature", String(_BME.readTempC()));
    _JSON_Sample += Value2Json("BME280_pressure", String((int)_BME.readFloatPressure()));
    _JSON_Sample += Value2Json("BME280_humidity", String((int)_BME.readFloatHumidity()));
    Serial.println(_BME.readTempC());
    return _JSON_Sample;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  BME280 _BME;
  int _I2CAddress = 0;
};

#endif
