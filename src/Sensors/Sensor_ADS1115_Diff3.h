// ***********************************************************************************
// Version 0.2, 13-06-2019, SM, checked by ...
//    - Get_Name_Version () added
//    - JSON_Short_Data added
//
// Version 0.1, 13-05-2019, SM, checked by ...
//    - initial version
// ***********************************************************************************

#ifndef Sensor_ADS1115_Diff3_h
#define Sensor_ADS1115_Diff3_h 0.2

#include "Hardware/Stef_Adafruit_ADS1015.h"
#include "Sensor_Base.h"

// ***********************************************************************************
// https://os.mbed.com/teams/PQ_Hybrid_Electrical_Equipment_Team/code/ADS1015/file/71b44421e736/Adafruit_ADS1015.h/
// ***********************************************************************************
class _Sensor_ADS1115_Diff3 : public _Sensor_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // My_Address should be either 1 ... 4  (I2C Address = 0x47 + My_Number)
  // or the real I2C Address  0x48 ... 0x4B
  // ***********************************************************************
  _Sensor_ADS1115_Diff3(String Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_ADS1115_Diff3(int My_Address = 1, adsGain_t Gain = GAIN_ONE) {
    Version_Name = "V" + String(Sensor_ADS1115_Diff3_h) + "  ──────  Sensor_ADS1115_Diff3.h";
    Serial.println("\n──────  CREATE  ──────  " + Version_Name);
    _Gain = Gain;
    _My_Address = My_Address;

    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    // ***************************************************************
    // create the ADC-devices, 4 is the maximum
    // Adafruit_ADS1115   ADC          ;  // 0x48 (1001000) ADR -> GND
    // Adafruit_ADS1115   ADC ( 0x49 ) ;  // 0x49 (1001001) ADR -> VDD
    // Adafruit_ADS1115   ADC ( 0x4A ) ;  // 0x4A (1001010) ADR -> SDA
    // Adafruit_ADS1115   ADC ( 0x4B ) ;  // 0x4B (1001011) ADR -> SCL
    // ***************************************************************
    switch (this->_My_Address) {
      case 1:
        this->_My_Address = 0x48;
        break;
      case 2:
        _My_Address = 0x49;
        break;
      case 3:
        this->_My_Address = 0x4A;
        break;
      case 4:
        this->_My_Address = 0x4B;
        break;
      default:
        this->_My_Address = _My_Address;
    }

    switch (this->_My_Address) {
      case 0x48:
        this->_Name1 = "ADC10";
        this->_Name2 = "ADC11";
        this->_Name3 = "ADC12";
        break;
      case 0x49:
        this->_Name1 = "ADC20";
        this->_Name2 = "ADC21";
        this->_Name3 = "ADC22";
        break;
      case 0x4A:
        this->_Name1 = "ADC30";
        this->_Name2 = "ADC31";
        this->_Name3 = "ADC32";
        break;
      case 0x4B:
        this->_Name1 = "ADC40";
        this->_Name2 = "ADC41";
        this->_Name3 = "ADC42";
        break;
    }
    _ADC = Adafruit_ADS1115(this->_My_Address);
    Wire_Print();

    this->_JSON_Short_Header = this->_Name1 + '\t' + this->_Name2 + '\t' + this->_Name3 + '\t';
    this->_JSON_Long_Header = this->_JSON_Short_Header;
  }

  // ***********************************************************************
  //#define ADS1015_REG_CONFIG_PGA_MASK     (0x0E00)
  //#define ADS1015_REG_CONFIG_PGA_6_144V   (0x0000) =0*512 // +/-6.144V range = Gain 2/3
  //#define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200) =1*512 // +/-4.096V range = Gain 1
  //#define ADS1015_REG_CONFIG_PGA_2_048V   (0x0400) =2*512 // +/-2.048V range = Gain 2 (default)
  //#define ADS1015_REG_CONFIG_PGA_1_024V   (0x0600) =3*512 // +/-1.024V range = Gain 4
  //#define ADS1015_REG_CONFIG_PGA_0_512V   (0x0800) =4*512 // +/-0.512V range = Gain 8
  //#define ADS1015_REG_CONFIG_PGA_0_256V   (0x0A00) =5*512 // +/-0.256V range = Gain 16
  // ***********************************************************************
  void setup() {
    _ADC.setGain(_Gain);
    _ADC.begin();
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _My_Address = Settings.Get_Set_Default_Int("ADS1115-d3 I2C Address", 0x48, Force);
    _Sample_Time_ms = Settings.Get_Set_Default_Int("ADS1115-d3 Sample[ms]", 1000, Force);

    int Gain = Settings.Get_Set_Default_Int("ADS1115-d3 Gain", 1, Force);
    _Gain = (adsGain_t)(512 * Gain);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int I2C = Settings.Read_Int("ADS1115-d3 I2C Address");
    int Period = Settings.Read_Int("ADS1115-d3 Sample[ms]");
    int Gain = Settings.Read_Int("ADS1115-d3 Gain");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "ADS1115-d3 I2C Address") {
        if (New_Value_Int != I2C) {
          _My_Settings_Buffer["ADS1115-d3 I2C Address"] = New_Value_Int;
          _My_Address = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "ADS1115-d3 Sample[ms]") {
        if (New_Value_Int != Period) {
          _My_Settings_Buffer["ADS1115-d3 Sample[ms]"] = New_Value_Int;
          _Sample_Time_ms = New_Value_Int;
        }
      } else if (My_Webserver.argName(i) == "ADS1115-d3 Gain") {
        if (New_Value_Int != Gain) {
          _My_Settings_Buffer["ADS1115-d3 Gain"] = New_Value_Int;
          _Gain = (adsGain_t)(512 * New_Value_Int);
          _ADC.setGain(_Gain);
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (millis() - _Sample_Time_Last >= _Sample_Time_ms) {
      _Sample_Time_Last = millis();

      int16_t ADC0 = _ADC.readADC_Differential_0_3();
#ifdef ADS1511_Unreliable
      ADC0 = _ADC.readADC_Differential_0_3();
#endif
      delay(1);
      int16_t ADC1 = _ADC.readADC_Differential_1_3();
#ifdef ADS1511_Unreliable
      ADC1 = _ADC.readADC_Differential_1_3();
#endif
      delay(1);
      int16_t ADC2 = _ADC.readADC_Differential_2_3();
#ifdef ADS1511_Unreliable
      ADC2 = _ADC.readADC_Differential_2_3();
#endif

      // The following does not work, not even with yield (0) before and after
      _ADC0 += ADC0;
      _ADC1 += ADC1;
      _ADC2 += ADC2;

      _NSample += 1;
    }
  }

  // ***********************************************************************
  //#define ADS1015_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
  //#define ADS1015_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
  //#define ADS1015_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
  //#define ADS1015_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
  //#define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
  //#define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
  //#define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
  //#define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3
  // ***********************************************************************

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    char s[4];
    Serial.print("I2C-Address = 0x");
    sprintf(s, "%02x", _My_Address);
    Serial.println(s);

    Serial.print("Gain        = ");
    Serial.println(_Gain / 4);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    // He always has to get through this procedure, even if we don't have samples
    if (_NSample <= 0) _NSample = 1;

    JSON_Data += "\"" + this->_Name1 + "\":";
    JSON_Data += (_ADC0 / _NSample);
    JSON_Data += ", \"" + this->_Name2 + "\":";
    JSON_Data += (_ADC1 / _NSample);
    JSON_Data += ", \"" + this->_Name3 + "\":";
    JSON_Data += (_ADC2 / _NSample);
    JSON_Data += ",";

    JSON_Short_Data += (_ADC0 / _NSample);
    JSON_Short_Data += "\t";
    JSON_Short_Data += (_ADC1 / _NSample);
    JSON_Short_Data += "\t";
    JSON_Short_Data += (_ADC2 / _NSample);
    JSON_Short_Data += "\t";

    _NSample = 0;
    _ADC0 = 0;
    _ADC1 = 0;
    _ADC2 = 0;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  adsGain_t _Gain = GAIN_ONE;
  Adafruit_ADS1115 _ADC;
  int _My_Address = 0;
  int32_t _ADC0 = 0;
  int32_t _ADC1 = 0;
  int32_t _ADC2 = 0;
  int _NSample = 0;
  unsigned long _Sample_Time_Last = 0;
  unsigned long _Sample_Time_ms = 1000;

  String _Name1;
  String _Name2;
  String _Name3;
};

#endif
