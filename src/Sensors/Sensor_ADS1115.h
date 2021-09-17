// ***********************************************************************************
// Version 0.5, 26-07-2018, SM, checked by ...
//    - Get_JSON_Data appends directly to a global string
//
// Version 0.4, 21-07-2018, SM, checked by ...
//    - JSON, starts with content and ends with comma (not the curley brackets anymore)
//    - JSON:  "Time_ms"  changed to "ADC_Time_ms"
//
// Version 0.3, 15-07-2018, SM, checked by ...
//    - added an extra create with no parameters all
//
// Version 0.2, 27-06-2018, SM, checked by ...
//    - added multiple creators
//    - added differential setup
//    - added Testroutine, to measure signals with a scope
//
// Version 0.1, 30-03-2018, SM, checked by ...
//    - initial version
// ***********************************************************************************

#ifndef Sensor_ADS1115_h
#define Sensor_ADS1115_h 0.5

#include "Sensor_Base.h"
//#include <Adafruit_ADS1015.h>
#include "Hardware/Stef_Adafruit_ADS1015.h"

const char *Signal[] = {"Signal-1", "Signal-2", "Signal-3", "Signal-4"};

// ***********************************************************************************
// https://os.mbed.com/teams/PQ_Hybrid_Electrical_Equipment_Team/code/ADS1015/file/71b44421e736/Adafruit_ADS1015.h/
// ***********************************************************************************
class _Sensor_ADS1115 : public _Sensor_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // My_Address should be either 1 ... 4  (I2C Address = 0x47 + My_Number)
  // or the real I2C Address  0x48 ... 0x4B
  // ***********************************************************************
  _Sensor_ADS1115(String Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_ADS1115(int My_Address = 1, int N_Chan = 2, int Sample_Time_ms = 1000, bool Diff = false,
                  adsGain_t Gain = GAIN_ONE) {
    Serial.println("V" + String(Sensor_ADS1115_h) + "   Sensor_ADS1115_h");
    _Sample_Time_ms = Sample_Time_ms;
    _N_Chan = N_Chan;
    _Gain = Gain;
    _My_Address = My_Address;
    _Diff = Diff;
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
    switch (_My_Address) {
      case 1:
        _My_Address = 0x48;
        break;
      case 2:
        _My_Address = 0x49;
        break;
      case 3:
        _My_Address = 0x4A;
        break;
      case 4:
        _My_Address = 0x4B;
        break;
      default:
        _My_Address = _My_Address;
    }
    _ADC = Adafruit_ADS1115(_My_Address);
    Wire_Print();

    Help_Text = "#define Sensor_ADS1115  <Address>\n\
#define Sensor_ADS1115  <Address>  <N_Chan>  <Sample_Time_ms>  <Differential>\n\
    <Address> the real I2C address or \n\
              1 for 0x48, 2 for 0x49, 3 for 0x4A, 4 for 0x4B\n\
    <Differential> false/true single-ended/differential AD conversion";
    Print_Help();

    for (int i = 0; i < _N_Chan; i++) {
      _JSON_Short_Header = "ADC" + String(i + 1) + "\t";
    }
  }

  // ***********************************************************************
  //#define ADS1015_REG_CONFIG_PGA_MASK     (0x0E00)
  //#define ADS1015_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
  //#define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
  //#define ADS1015_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
  //#define ADS1015_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
  //#define ADS1015_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
  //#define ADS1015_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16
  // ***********************************************************************
  void setup() {
    _ADC.setGain(_Gain); // +/- 4.096V  1 bit = 0.125mV
    _ADC.begin();
    Print_Help();
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _My_Address = Settings.Get_Set_Default_Int("ADS1115 I2C Address", 0x48, Force);
    _N_Chan = Settings.Get_Set_Default_Int("ADS1115 N Chan", 2, Force);
    _Sample_Time_ms = Settings.Get_Set_Default_Int("ADS1115 Sample[ms]", 1000, Force);
    _Diff = Settings.Get_Set_Default_Int("ADS1115 Diff", false, Force);

    int Gain = Settings.Get_Set_Default_Int("ADS1115 Gain", 1, Force);
    _Gain = (adsGain_t)(512 * Gain);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int I2C = Settings.Read_Int("ADS1115 I2C Address");
    int NChan = Settings.Read_Int("ADS1115 N Chan");
    int Period = Settings.Read_Int("ADS1115 Sample[ms]");
    int Diff = Settings.Read_Int("ADS1115 Diff");
    int Gain = Settings.Read_Int("ADS1115 Gain");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "ADS1115 I2C Address") {
        if (New_Value_Int != I2C) {
          _My_Settings_Buffer["ADS1115 I2C Address"] = New_Value_Int;
          _My_Address = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "ADS1115 N Chan") {
        if (New_Value_Int != NChan) {
          _My_Settings_Buffer["ADS1115 N Chan"] = New_Value_Int;
          _N_Chan = New_Value_Int;
        }
      } else if (My_Webserver.argName(i) == "ADS1115 Sample[ms]") {
        if (New_Value_Int != Period) {
          _My_Settings_Buffer["ADS1115 Sample[ms]"] = New_Value_Int;
          _Sample_Time_ms = New_Value_Int;
        }
      } else if (My_Webserver.argName(i) == "ADS1115 Diff") {
        if (New_Value_Int != Diff) {
          _My_Settings_Buffer["ADS1115 Diff"] = New_Value_Int;
          _Diff = New_Value_Int;
        }
      } else if (My_Webserver.argName(i) == "ADS1115 Gain") {
        if (New_Value_Int != Gain) {
          _My_Settings_Buffer["ADS1115 Gain"] = New_Value_Int;
          _Gain = (adsGain_t)(512 * New_Value_Int);

          _ADC.setGain(_Gain);
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    char s[4];
    Serial.print("\n#define Sensor_ADS1115  Address=0x");
    sprintf(s, "%02x", _My_Address);
    Serial.print(s);
    Serial.print(",   N_Chan=");
    Serial.print(_N_Chan);
    Serial.print(",  Sample_Time[ms]=");
    Serial.print(_Sample_Time_ms);
    Serial.print(",  Differential=");
    Serial.print(_Diff);
    Serial.println("        <<<< Current");
    Serial.println(Help_Text);
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
  void loop() {
    if (millis() - Sample_Time_Last >= _Sample_Time_ms) {
      Sample_Time_Last += _Sample_Time_ms;

      if (_ADC_Buffer_Pointer == 0) {
        _Start_Time = millis();
      }

      //**************************************************************
      // read all ADC channels and append to the buffer:
      // 20 msec = 2 channels (can be done faster)
      // 40 msec = 4 channels (can be done faster)
      //**************************************************************
      if (_Diff) {
        _ADC_Buffer[_ADC_Buffer_Pointer] = _ADC.readADC_Differential_0_3();
        _ADC_Buffer_Pointer += 1;
        if (_N_Chan > 1) {
          _ADC_Buffer[_ADC_Buffer_Pointer] = _ADC.readADC_Differential_1_3();
          _ADC_Buffer_Pointer += 1;
          if (_N_Chan > 2) {
            _ADC_Buffer[_ADC_Buffer_Pointer] = _ADC.readADC_Differential_2_3();
            _ADC_Buffer_Pointer += 1;
          }
        }
      } else {
        for (int Chan = 0; Chan < _N_Chan; Chan++) {
          _ADC_Buffer[_ADC_Buffer_Pointer + Chan] = _ADC.readADC_SingleEnded(Chan);
        }
        _ADC_Buffer_Pointer += _N_Chan;
      }
    }
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  int16_t Test(int Chan) { return _ADC.readADC_SingleEnded(Chan); }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  int Get_JSON_Length() { return _ADC_Buffer_Pointer; }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"ADC_Time_ms\":";
    JSON_Data += String(_Start_Time);
    JSON_Data += ", \"N_Chan\":";
    JSON_Data += String(_N_Chan);
    JSON_Data += ", \"T_Samp\":";
    JSON_Data += String(_Sample_Time_ms);
    JSON_Data += ", \"Signals\":[";
    for (int i = 0; i < _N_Chan; i++) {
      JSON_Data += "\"";
      JSON_Data += Signal[i];
      JSON_Data += "\",";
    }
    JSON_Data.remove(JSON_Data.length() - 1);
    JSON_Data += "],";

    JSON_Data += "\"ADC\":[";
    for (int i = 0; i < _ADC_Buffer_Pointer; i++) {
      JSON_Data += String(_ADC_Buffer[i]);
      JSON_Data += ",";
    }
    JSON_Data.remove(JSON_Data.length() - 1);
    JSON_Data += "],";

    // ****************************************************************
    // For the time being chosen to only display 1 sample per channel here
    // ****************************************************************
    for (int i = 0; i < _N_Chan; i++) {
      JSON_Short_Data += String(_ADC_Buffer[i]);
      JSON_Short_Data += "\t";
    }

    _ADC_Buffer_Pointer = 0;
  }

  // ***********************************************************************
  // ***********************************************************************
  void Set_Gain(adsGain_t Gain) { _ADC.setGain(Gain); }

  // ***********************************************************************
  // ***********************************************************************
  // we want a second one with just signals names (NChan is number of signal names)
  void Set_Channels(int N_Channels) { _N_Chan = N_Channels; }

  // ***********************************************************************
private:
  // ***********************************************************************
  int _N_Chan = 2;
  bool _Diff = false;
  adsGain_t _Gain = GAIN_ONE;
  int _Sample_Time_ms = 1000;
  int16_t _ADC_Buffer[200];
  int _ADC_Buffer_Pointer = 0;
  unsigned long _Start_Time;
  Adafruit_ADS1115 _ADC;
  int _My_Address = 0;
};

#endif
