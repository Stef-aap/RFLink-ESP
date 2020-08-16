// ***********************************************************************************
// Version 0.1, 17-07-2019, SM, checked by ...
//    - initial version
// ***********************************************************************************

#ifndef Sensor_ESP32_ADC_h
#define Sensor_ESP32_ADC_h 0.1

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_ESP32_ADC : public _Sensor_BaseClass {

public:
  //      String Help_Text = "#define Sensor_ESP_ADC" ;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_ESP32_ADC(adc_attenuation_t Attenuation, int Chan_0, int Chan_1 = -1, int Chan_2 = -1, int Chan_3 = -1,
                    int Chan_4 = -1, int Chan_5 = -1, int Chan_6 = -1, int Chan_7 = -1) {
    _Attenuation = Attenuation;
    for (int i = 0; i < 8; i++) {
      _Attenuations[i] = _Attenuation;
    }
    _Channels[0] = Chan_0;
    _Channels[1] = Chan_1;
    _Channels[2] = Chan_2;
    _Channels[3] = Chan_3;
    _Channels[4] = Chan_4;
    _Channels[5] = Chan_5;
    _Channels[6] = Chan_6;
    _Channels[7] = Chan_7;

    Version_Name = "V" + String(Sensor_ESP32_ADC_h) + "   Sensor_ESP32_ADC.h";
    Serial.println(Version_Name);

    // unsigned long Start = millis() ;
    for (int i = 0; i < 8; i++) {
      if (_Channels[i] >= 0) {
        _JSON_Short_Header += "ESP_ADC" + String(i);
        _JSON_Short_Header += "\t";
      }
    }

    Help_Text = "    For the moment this module only supports ADC1.\n\
    ADC2 is not usable if WiFi is used.\n\
    Standard methods: setup / loop / Print_Help / Get_JSON_Data\n\
_Sensor_ESP32_ADC ( adc_attenuation_t Attenuation, int Chan_0, int Chan_1 =-1, int Chan_2 =-1, int Chan_3 =-1, int Chan_4 =-1, int Chan_5 =-1, int Chan_6 =-1, int Chan_7 =-1  ) \n\
    Channel numbers: 32 t/m 39 (ADC1) \n\
int  Get_ADC  ( int Channel_Index ) : gets the last sampled value of the selected channel\n\
    Attenuation can be set for all channels through the constructor, and then for each channel individual \n\
void Set_Attenuation ( int Channel_Index, adc_attenuation_t Attenuation ) \n\
    Attenuation can be one of \n\
        ADC_0db   = 1.0  V \n\
        ADC_2_5db = 1.34 V \n\
        ADC_6db   = 1.5  V \n\
        ADC_11db  = 3.6  V  (limited by power supply 3.3 V) (default)";
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() { analogSetAttenuation(_Attenuation); }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.print("  Channels ADC1 : ");
    int N_Chan = 0;
    for (int i = 0; i < 8; i++) {
      if (_Channels[i] >= 0) {
        Serial.print(_Channels[i]);
        Serial.print(", ");
        N_Chan += 1;
      }
    }
    Serial.println();

    bool Attenuation_Equal = true;
    for (int i = 0; i < N_Chan; i++) {
      if (_Attenuations[i] != _Attenuation) {
        Attenuation_Equal = false;
      }
    }
    if (Attenuation_Equal) {
      Serial.print("  Attenuation = ");
      switch (_Attenuation) {
        case 0:
          Serial.print("0dB (<1.0 V), ");
          break;
        case 1:
          Serial.print("2.5dB (<1.34 V), ");
          break;
        case 2:
          Serial.print("6dB (<1.5 V), ");
          break;
        case 3:
          Serial.print("11dB (<3.3 V), ");
          break;
      }
    } else {
      Serial.print("  ATTENUATIONS = ");
      for (int i = 0; i < N_Chan; i++) {
        switch (_Attenuations[i]) {
          case 0:
            Serial.print("0dB (<1.0 V), ");
            break;
          case 1:
            Serial.print("2.5dB (<1.34 V), ");
            break;
          case 2:
            Serial.print("6dB (<1.5 V), ");
            break;
          case 3:
            Serial.print("11dB (<3.3 V), ");
            break;
        }
      }
    }
    Serial.println();

    Serial.println(Help_Text);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += "\"ESP32_ADC\":[";

    // unsigned long Start = millis() ;
    for (int i = 0; i < 8; i++) {
      if (_Channels[i] >= 0) {
        analogSetPinAttenuation(_Channels[i], _Attenuations[i]);
        _ADC_Buffer[i] = analogRead(_Channels[i]);
        JSON_Data += String(_ADC_Buffer[i]);
        JSON_Data += ",";

        JSON_Short_Data += String(_ADC_Buffer[i]);
        JSON_Short_Data += "\t";

        //          JSON_Short_Header += "ESP_ADC" + String ( i ) ;
        //          JSON_Short_Header += "\t" ;

        // Serial.print("ADC VALUE = " ) ;
        // Serial.println ( _ADC_Buffer [i] );
      }
    }
    // Serial.print ( millis() - Start ) ;

    JSON_Data.remove(JSON_Data.length() - 1);
    JSON_Data += "],";
  }

  // ***********************************************************************
  // ***********************************************************************
  void Set_Attenuation(int Channel_Index, adc_attenuation_t Attenuation) { _Attenuations[Channel_Index] = Attenuation; }

  // ***********************************************************************
  // ***********************************************************************
  int Get_ADC(int Channel_Index) {
    if ((Channel_Index >= 0) && (Channel_Index < 8)) {
      return _ADC_Buffer[Channel_Index];
    }
    return 0;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  int _Channels[8];
  adc_attenuation_t _Attenuations[8];
  adc_attenuation_t _Attenuation;
  int16_t _ADC_Buffer[8];
};

#endif
/*
analogSetAttenuation(attenuation): sets the input attenuation for all ADC pins. Default is ADC_11db. Accepted values:
analogReadResolution(resolution): set the sample bits and resolution. It can be a value between 9 (0 – 511) and 12 bits
(0 – 4095). Default is 12-bit resolution. analogSetWidth(width): set the sample bits and resolution. It can be a value
between 9 (0 – 511) and 12 bits (0 – 4095). Default is 12-bit resolution. analogSetCycles(cycles): set the number of
cycles per sample. Default is 8. Range: 1 to 255. analogSetSamples(samples): set the number of samples in the range.
Default is 1 sample. It has an effect of increasing sensitivity. analogSetClockDiv(attenuation): set the divider for the
ADC clock. Default is 1. Range: 1 to 255. adcAttachPin(pin): Attach a pin to ADC (also clears any other analog mode that
could be on). Returns TRUE or FALSE result. adcStart(pin), adcBusy(pin) and resultadcEnd(pin): starts an ADC convertion
on attached pin’s bus. Check if conversion on the pin’s ADC bus is currently running (returns TRUE or FALSE). Get the
result of the conversion: returns 16-bit integer.
*/
