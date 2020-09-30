// ***********************************************************************************
// INSTRUCTION:
// define OneWire_Pin, to include Sensor_Receiver.h
//   Any pin 2 to 12 (not 13) and A0 to A5
//
//   #define OneWire_Pin  2
//   #include "Sensor_Receiver.h"
//
// ***********************************************************************************
// Version 1.0, 13-06-2019, SM, checked by ...
//    - Get_Name_Version () added
//    - JSON_Short_Data added
//
// Version 0.9   04-05-2019, SM
//    - OneWire_Pin = 2 is defined here if it is not already defined
//
// Version 0.8 02-01-2019, SM
//    - External_Watchdog_Disarm added
//
// version 0.7
//    - other way of temperature correction: data [1] & 0xFB.
//
// Version 0.6
//    - as temperature. 60 Celsius, counting 64 Celsius
//
// Version 0.5
//    - other calculation of the temperature
//    - Domoticz support removed
//
// Version 0.4
//    - Domoticz output added
//
// Version 0.3, 26-07-2018, SM, checked by ...
//    - Get_JSON_Data appends directly to a global string
//
// Version 0.2
//   - Offset of 2 Celsius removed
//
// Version 0.1
//    - initial version

#ifndef Sensor_DS18B20_h
  #define Sensor_DS18B20_h 1.0

  // PIN for OneWire, if necessary must be defined before library Sensor_Receiver
  // Any pin 2 to 12 (not 13) and A0 to A5
  #ifndef OneWire_Pin
    #define OneWire_Pin 2
  #endif

  #include "Sensor_Base.h"
  #include <OneWire.h>

// Setup a oneWire instance to communicate with any OneWire devices
OneWire _OneWire(OneWire_Pin);
// Pass our oneWire reference to Dallas Temperature.
// DallasTemperature sensors ( &oneWire ) ;

const int _OneWire_Nr_Bytes = 8;
const int _OneWire_Max_Devices = 4;

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_DS18B20 : public _Sensor_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_DS18B20(String Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_DS18B20() {
    this->_OneWire_Resolution = 10;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    Version_Name = "V" + String(Sensor_DS18B20_h) + "  ======  Sensor_DS18B20.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    _OneWire.begin(_OneWire_Pin);
    Search_Devices();
    Set_Resolution_Alarm(_OneWire_Resolution);
    Serial.println(">>>>>>>>> Dallas:   Pin = " + String(_OneWire_Pin) +
                   "    Resolution = " + String(_OneWire_Resolution));

    for (byte Index = 0; Index < _OneWire_Nr_Devices; Index++) {
      _JSON_Short_Header += "DT" + _OneWire_Names[Index] + "\t";
    }
    this->_JSON_Long_Header = this->_JSON_Short_Header;
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    this->_OneWire_Pin = Settings.Get_Set_Default_Int("Dallas OneWire", 2, Force);
    this->_OneWire_Resolution = Settings.Get_Set_Default_Int("Dallas Resolution", 10, Force);
    _OneWire.begin(_OneWire_Pin);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;
    int Pin = Settings.Read_Int("Dallas OneWire");
    int Resolution = Settings.Read_Int("Dallas Resolution");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "Dallas OneWire") {
        if (New_Value_Int != Pin) {
          _My_Settings_Buffer["Dallas OneWire"] = New_Value_Int;
          _OneWire_Pin = New_Value_Int;
          _OneWire.begin(_OneWire_Pin);
        }
      } else if (My_Webserver.argName(i) == "Dallas Resolution") {
        if (New_Value_Int != Resolution) {
          _My_Settings_Buffer["Dallas Resolution"] = New_Value_Int;
          _OneWire_Resolution = New_Value_Int;
          this->Set_Resolution_Alarm(_OneWire_Resolution);
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.print("OneWire Pin = ");
    Serial.println(_OneWire_Pin);
    Serial.print("Resolution  = ");
    Serial.println(this->_OneWire_Resolution);
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Hardware_Test(int Test_Nr = 1) {
    _Hardware_Test += 1;
    if (_Hardware_Test > 50000) {
      delay(2000); // this will trigger the external hardware watchdog
    }
    return true;
  }
  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    Get_Temperatures(_OneWire_Temperature_Array);
    for (byte Index = 0; Index < _OneWire_Nr_Devices; Index++) {
      JSON_Data += " \"DT";
      JSON_Data += _OneWire_Names[Index];
      JSON_Data += "\":";
      JSON_Data += _OneWire_Temperature_Array[Index] / 1000.0;
      JSON_Data += ",";

      JSON_Short_Data += String(_OneWire_Temperature_Array[Index] / 1000.0, 1);
      JSON_Short_Data += "\t";

      External_Watchdog_Toggle();
    }
  }

  String Get_JSON_LuftData() { return _JSON_Sample; }

  // ***********************************************************************
private:
  // ***********************************************************************
  int _OneWire_Pin = OneWire_Pin;
  int _OneWire_Resolution;

  byte _OneWire_Device_Matrix[_OneWire_Max_Devices][_OneWire_Nr_Bytes];
  int _OneWire_Nr_Devices = 0;
  int _OneWire_Temperature_Array[_OneWire_Max_Devices];
  String _OneWire_Names[_OneWire_Max_Devices];
  int _Hardware_Test = 0;

  // *************************************************************************
  // *************************************************************************
  void Search_Devices(void) {
    // ********************************************
    // remove previous found devices from json tree
    // ********************************************
    JsonObject DocumentRoot = _My_Settings_Buffer.as<JsonObject>();
    for (JsonPair KeyValue : DocumentRoot) {
      String Key = String(KeyValue.key().c_str());
      if (Key.startsWith("DS18B20 Device")) DocumentRoot.remove(Key);
    }

    byte addr[8];

    _OneWire_Nr_Devices = 0;

    Serial.print("Looking for 1-Wire devices...\n");
    Serial.print(">>>>>>>>> Dallas OneWire Pin = ");
    Serial.println(OneWire_Pin);

    _OneWire.reset();
    _OneWire.reset_search();
    while (_OneWire.search(addr)) {
      Serial.print("  ");
      Serial.print(_OneWire_Nr_Devices + 1);
      Serial.print(" :  ");
      String Name = "";
      for (byte i = 0; i < 8; i++) {
        Serial.print("0x");
        if (addr[i] < 16) {
          Serial.print('0');
          Name += '0';
        }
        Serial.print(addr[i], HEX);
        Name += String(addr[i], HEX);
        if (i < 7) {
          Serial.print(", ");
        }
      }
      if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!");
      } else {
        Device_Matrix_Put(addr, _OneWire_Nr_Devices);
        _OneWire_Names[_OneWire_Nr_Devices] = Name;

        _My_Settings_Buffer["DS18B20 Device " + String(_OneWire_Nr_Devices + 1)] = "\t" + Name;

        Serial.println(Name);
        _OneWire_Nr_Devices += 1;
      }
    }
    Settings.Set_Unstored_Changes();
  }

  // *************************************************************************
  // Resolution is a trade off between accuracy and speed.
  //     Resolution     Delta       Time    Code
  //       [bits]     [Celcius]     [ms]
  //         9         0.5          100      1F
  //         10        0.25         200      3F
  //         11        0.125        400      5F
  //         12        0.0625       800      7F
  // *************************************************************************
  void Set_Resolution_Alarm(byte Resolution = 10, byte Low_Alarm = 0xFF, byte High_Alarm = 0x7F) {
    byte addr[8];

    for (byte Index = 0; Index < _OneWire_Nr_Devices; Index++) {
      Device_Matrix_Get(addr, Index);

      _OneWire.reset();
      _OneWire.select(addr);
      _OneWire.write(0x4E); // write scratch
      _OneWire.write(High_Alarm);
      _OneWire.write(Low_Alarm);
      byte Res = (Resolution - 9) << 5;
      _OneWire.write(Res);

      _OneWire.reset();
      _OneWire.select(addr);
      _OneWire.write(0x48); // copy scratch to EEPROM

      delay(20); // <--- added 20ms delay to allow 10ms long EEPROM write operation (as specified by datasheet)
    }
  }

  // *************************************************************************
  // Sends a Conversion command without an address
  // Then samples all the available/known channels
  // *************************************************************************/
  void Get_Temperatures(int *Data) {
    byte addr[8];
    byte data[12];

    _OneWire.reset();
    _OneWire.skip();
    _OneWire.write(0x44); // start conversion, without parasite power

    // wait till (all) device has completed their conversion
    while (not _OneWire.read_bit()) {
      delay(1);
    }

    for (byte Index = 0; Index < _OneWire_Nr_Devices; Index++) {
      Device_Matrix_Get(addr, Index);

      _OneWire.reset();
      _OneWire.select(addr);
      _OneWire.write(0xBE); // Read Scratchpad

      for (byte i = 0; i < 9; i++) { // we need 9 bytes
        data[i] = _OneWire.read();
      }

      short Temp_i = (data[1] << 8) | data[0];
      float Temp_f = 1000 * (float)(Temp_i) / 16;
      Data[Index] = (int)Temp_f;
    }
  }

  // *************************************************************************
  // *************************************************************************
  void Device_Matrix_Put(byte *Data, int Index = 0) {
    for (int j = 0; j < _OneWire_Nr_Bytes; j++) {
      _OneWire_Device_Matrix[Index][j] = Data[j];
    }
  }

  // *************************************************************************
  // *************************************************************************
  void Device_Matrix_Get(byte *Data, int Index = 0) {
    for (int j = 0; j < _OneWire_Nr_Bytes; j++) {
      Data[j] = _OneWire_Device_Matrix[Index][j];
    }
  }
};

#endif

/*
bad, which gives negative values at room temperature
RS: { "TimeStamp":269621, "DT28c6cf5500000027":-32.50 }
FC  /  FD  /  7F  /  FF  /  3F  /  FF  /  4  /  10  /  B7  /
RS: { "TimeStamp":279621, "DT28c6cf5500000027":-32.25 }
FC  /  FD  /  7F  /  FF  /  3F  /  FF  /  4  /  10  /  B7  /
RS: { "TimeStamp":289621, "DT28c6cf5500000027":-32.25 }
F8  /  FD  /  7F  /  FF  /  3F  /  FF  /  8  /  10  /  EF  /
*/