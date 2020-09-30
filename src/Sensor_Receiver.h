// ****************************************************************************
/*
//#define Set_RTC_YMD_HMS_DoW  "2020,2,21,12,57,0,FRI"

// *************************************************************************
// build paramters to reduce RAM size
//   the parameters starting with YES_ are only used in special programs
//   the parameters starting witn NOT_ are often used in most programs (used by myself)
// *************************************************************************
//#define YES_INCLUDE_FTPSERVER
//#define YES_INCLUDE_BLAUWE_ENGEL
//#define YES_INCLUDE_FIJNSTOF_CONDITIONERING
//#define YES_INCLUDE_SENSOR_BMP280
//#define YES_INCLUDE_SENSOR_MLX90614
//#define YES_INCLUDE_SENSOR_MLX90640
//#define YES_INCLUDE_SENSOR_MQTTBroker
//#define YES_INCLUDE_SENSOR_NTP
//#define YES_INCLUDE_SENSOR_OKE4
#define YES_INCLUDE_SENSOR_RFLINK
//#define YES_INCLUDE_SENSOR_RTC

//#define YES_INCLUDE_RECEIVER_LUFTDATEN
#define YES_INCLUDE_RECEIVER_OTA
#define YES_INCLUDE_RECEIVER_TELNET
#define YES_INCLUDE_RECEIVER_WEBSERVER

#define NOT_INCLUDE_SENSOR_ADS1115
#define NOT_INCLUDE_SENSOR_ADS1115_DIFF
#define NOT_INCLUDE_SENSOR_BME280
#define NOT_INCLUDE_SENSOR_DHT22
#define NOT_INCLUDE_SENSOR_DS18B20
#define NOT_INCLUDE_SENSOR_MHZ14
#define NOT_INCLUDE_SENSOR_MPU9250
#define NOT_INCLUDE_SENSOR_PIR
#define NOT_INCLUDE_SENSOR_SDS011

//#define NOT_INCLUDE_RECEIVER_MQTT
#define NOT_INCLUDE_RECEIVER_SCRATCHPAD
#define NOT_INCLUDE_RECEIVER_SDFAT
#define NOT_INCLUDE_RECEIVER_SSD1306
#define NOT_INCLUDE_RECEIVER_TM1638

*/
// ****************************************************************************
// Version 3.0  16-04-2020, SM
//    - effect of conditional build flags on web settings page improved
//    - more build flags added
//
// Version 2.9  10-04-2020, SM
//    - Topic appended to: MQTT_Callback (Topic, Payload, jsonBuffer);
//    - if from a Sensor or Receiver, the subscribed topic ends with an "/",
// then a "#" is added (universal subscription)
//    - _Debug_MQTT_Client renamed to My_MQTT_Client
//    - handling of serial commands in Sensor / Receiver loop corrected
//    - Serial_2_Telnet flag removed, because simultaneous Serial and Telnet is possible
//    - _DEBUG_Global_String added, to collects debug info while serial not yet available
// will be printed at the end of Receivers.Setup
//
// Version 2.8 23-02-2020, SM
//    - some cleaning up
//    - #define _Settings_Factory_BufSize 100 (reduced from 512)
//
// Version 2.7 05-02-2020, SM
//    - SDfat added, disabled other filesu systems via defines
//    - command loop, now guaranteed to run through both Sensors and Receivers
//
// Version 2.6 02-02-2020, SM
//    - MLX90640 only for ESP32 (gives compile errors on ESP8266)
//
// Version 2.5 12/20/2019, SM
//
// Version 2.4 06-12-2019, SM
//    - Sensor_MLX90614 added
//    - Sensor_MLX90640 added
//    - Sensor_MPU9250 added
//    - Sensor.Add returns a pointer to the Sensor
//    - Receiver.Add returns a pointer to the Receiver
//    - MQTT_ID derived from MQTT_Topic in Debug function
//
// Version 2.3 2019-09-26, SM
//    - My_Prefs = Preferences ("My_Prefs", ...) added
//
// Version 2.2 06-08-2019, SM
//    - delay (2) added to loops of Sensors and Receivers
//
// Version 2.1 2019-07-30, SM
//    - Added FTP flag, which can be controlled by MQTT
//
// Version 2.0 17-07-2019, SM
//    - Sensor_ESP32_ADC added
//    - Sensor_Blauwe_Angel added
//    - CSV header is printed at the beginning
//    - Help info from sensors printed after setup
//
// Version 1.9 12-06-2019, SM
//
// Version 1.8 May 23, 2019, SM
//    - JSON_Short_Data added (specially for SPIFFS storage
//
// Version 1.7 May 8, 2019, SM
//    - MQTT_Broker_IP, MQTT_Topic, MQTT_ID are defined here and name slightly changed
//    - OTA_HostName defined here
//
// Version 1.6 May 4, 2019, SM
//    - split (no need to use), in order to get MQTT broker and message to main program
//
// Version 1.5 March 9, 2019, SM
//    - print Main_Version in setup Sensors
//
// Version 1.3 March 5, 2019, SM
//    - Debug only sends over MQTT if there is already a connection
//    - import Luftdaten removed
//    - Value2Json added
//
// Version 1.2 Feb 22, 2019, SM
//    - added wifi mode and power
//
// Version 1.1  25-jan-2019
//    - Sensors.Pushed added
//
// Version 1.0  1-jan-2019
//    - External Watchdog Added
//
// Version 0.9 in wording 31 october 2018
//    - If no responses JSON_Data made empty
//    - in the above case, the data is also not sent
//
// Version 0.8
//    - Debug method added
//    - Sensor_OKE4 added
//
// Version 0.7
//    - Get_JSON_Data based on global append of all sensors
//
// Version 0.6
//    - Send_Data returns true if all receivers returned true
//                       false otherwise
//    - includes changed something
//
// Version 0.5
// ****************************************************************************

// ****************************************************************************
// ****************************************************************************
String _DEBUG_Global_String = "";
// ****************************************************************************
// ****************************************************************************

#ifndef Sensor_Receiver_h
#define Sensor_Receiver_h 3.0

#define BI "/B_Info.txt"

#define FileSystem_SPIFFS

#include "Utilities/My_StringSplitter.h"

#ifdef ESP32
  #include "Utilities/My_ESP32_Support.h"
#endif

#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiAP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
#endif

#ifdef ESP32
  #include <WebServer.h>
WebServer My_Webserver(80);
#else
  #include <ESP8266WebServer.h>
// ESP8266WebServer.h - Dead simple web-server.
// Supports only one simultaneous client, knows how to handle GET and POST.
// error: expected constructor, destructor, or type conversion before 'My_Webserver'
ESP8266WebServer My_Webserver(80);
#endif

// ****************************************************************************
// It's essential that these statements are at the top of the program
// ****************************************************************************
#ifndef Redirect_Serial_h
// Needed as no Redirect Serial, to compile
// ACTUALLY MUST JUST CHANGE TO SERIAL
HardwareSerial Serial_Device(0); // UART_NUM_0 ) ;
#endif

#ifdef YES_INCLUDE_RECEIVER_TELNET
  #include "Receivers/Receiver_Telnet.h"
#endif
// ****************************************************************************

#ifdef ESP32
  #include <Preferences.h>
Preferences My_Prefs;
#endif

// ****************************************************************************
// GENERAL GLOBALS
// ****************************************************************************
struct _Struct_Global_DateTime {
  int Year = 2000;
  int Month = 1;
  int Day = 1;
  int Hour = 0;
  int Minute = 0;
  int Second = 0;
};

_Struct_Global_DateTime Global_DateTime;
bool Settings_By_WebInterface = false;

// ****************************************************************************
// PROTOTYPING
// ****************************************************************************
void _MQTT_Callback_Wrapper(char *topic, byte *payload, unsigned int length);
void Store_Modified_Settings_From_Webserver(bool Restart_Needed);
void Set_Signal_LED(int N, int On, int Off);
void Update_All_Headers();
void Send_Email(String Mail_To, String Subject, String Body, bool HTML_Format);
// ****************************************************************************

// ****************************************************************************
// ****************************************************************************
void Print_Heap(int ID = -1) {
  if (ID >= 0) Serial.print(ID);
  Serial.print("            Free Heap = ");
  Serial.print(ESP.getFreeHeap());
#ifdef ESP8266
  Serial.print("            Fragmentation = ");
  Serial.println(" %");
#else
  Serial.println();
#endif
}

// ****************************************************************************
// Important: the settings file is read here
// ****************************************************************************
#include "Clients/My_Settings.h"
// ****************************************************************************

// ****************************************************************************
// ****************************************************************************
#ifndef NOT_INCLUDE_RECEIVER_MQTT
  #define MAX_MQTT_TOPICS 5
String MQTT_Topics[] = {"", "", "", "", ""};
void MQTT_Topics_Append(String Topic) {

  // *********************************
  // test if Topic already in list
  // *********************************
  for (int i = 0; i < MAX_MQTT_TOPICS; i++) {
    if (MQTT_Topics[i] == Topic) {
      return;
    }
  }

  // *********************************
  // if not in list, add the new Topic
  // *********************************
  for (int i = 0; i < MAX_MQTT_TOPICS; i++) {
    if (MQTT_Topics[i].length() == 0) {
      MQTT_Topics[i] = Topic;
      break;
    }
  }
}

void MQTT_Topics_Print(String Title) {
  Serial.println("\n    ==========  MQTT Subscribes from " + Title);
  for (int i = 0; i < MAX_MQTT_TOPICS; i++) {
    if (MQTT_Topics[i].length() > 0) {
      Serial.println(MQTT_Topics[i]);
    } else
      break;
  }
  Serial.println();
}
#endif

int Loop_Priority = 0;
bool Recording = true;

#ifndef NOT_INCLUDE_RECEIVER_TM1638
String _TM1638_Regel[8] = {"noSIGNAL", "        ", "        ", "        ",
                           "        ", "        ", "        ", "        "};
word _TM1638_Dots[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

// ********************************************
// ********************************************
#define WIFI_TX_POWER 82

bool Allow_Wifi_Not_Found = false;

// was at the very top
#ifndef ESP32
  #include "user_interface.h" //system_get_sdk_version()
#endif

#include <Wire.h>
#include <vector>

#include "Receivers/Receiver_Base.h"

// ****************************************************************************
// ****************************************************************************
bool FTP_Server_SD_MMC = false;
bool FTP_Server_SPIFFS = false;

// ****************************************************************************
// ****************************************************************************
String Build_Info;
String Build_Info_Help;
void Read_Build_Info() {
  Build_Info = File_System.Read_File(BI);
  if (Build_Info.length() == 0) {
    Build_Info = "00000000";
    File_System.Store_File(BI, Build_Info);
  }
  Serial.println("Build Info : " + Build_Info);
}

String JSON_Data;
String JSON_Short_Data;
String JSON_Short_Header;
String JSON_Long_Header;
int Signal_LED = -1;
bool Signal_LED_Invert = false;

// ****************************************************************************
// ****************************************************************************
int _External_Watchdog_Pin = -1;
bool _External_Watchdog_Pin_State = LOW;

// abcdefghjiklmnopqrstuvwxyz
// 0123456789012345678901234

// ****************************************************************************
// ****************************************************************************
void External_Watchdog_Toggle() {
  if (_External_Watchdog_Pin >= 0) {
    _External_Watchdog_Pin_State = !_External_Watchdog_Pin_State;
    digitalWrite(_External_Watchdog_Pin, _External_Watchdog_Pin_State);
  }
}

// ****************************************************************************
// ****************************************************************************
void External_Watchdog_Activate(String Actor = "") {
  _External_Watchdog_Pin = -1;
  Serial.println("++++++++++++++++++ EXTERNAL WATCHDOG ACTIVATED by " + Actor);
}

// ****************************************************************************
// ****************************************************************************
String MQTT_Broker_IP = "";
#ifndef NOT_INCLUDE_RECEIVER_MQTT
String MQTT_Broker_IP2 = "";
#endif
String MQTT_Topic;

// ****************************************************************************
// ****************************************************************************
bool Sensor_OKE4_Available = false;

// ****************************************************************************
// ****************************************************************************
bool _Debug_MQTT_Available = false;
bool _Debug_Over_MQTT = false;
extern bool _Debug_Over_Serial = true;
bool _Debug_Over_Bluetooth = false;
_Receiver_BaseClass *My_MQTT_Client;
bool _MQTT_Client_Available = false;
_Receiver_BaseClass *_Character_Display;
_Receiver_BaseClass *_p_Receiver_SDfat = NULL;
_Receiver_BaseClass *_p_Receiver_Email = NULL;

String MQTT_ID;
void Debug(String Line) {
  if (_Debug_Over_Serial) {
    Serial.println("DEBUG: " + Line);
  }
#ifndef NOT_INCLUDE_RECEIVER_MQTT
  if (_Debug_Over_MQTT) {
    if (My_MQTT_Client->Connected()) {
      // ********************************************************
      // If MQTT_ID still empty, take the last part of MQTT_Topic
      // ********************************************************
      if (MQTT_ID == "") {
        My_StringSplitter *Splitter = new My_StringSplitter(MQTT_Topic, '/');
        MQTT_ID = Splitter->getItemAtIndex(Splitter->getItemCount() - 1);
      }
      My_MQTT_Client->Publish("arduino/debug/" + MQTT_ID, Line);
    }
  }
#endif
  if (_Debug_Over_Bluetooth) {
  }
}
void Debugf(char *fmt, ...) {
  char buf[500];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end(args);
  Debug(String(buf));
}

// ***********************************************************************
// _Serial_Command = 0   wait for a command
// _Serial_Command = 1   command found
// _Serial_Command = 2   command run through first loop (Sensors or Receivers) and false
// _Serial_Command = 3   command run through Sensors and receivers or true in either loop
// ***********************************************************************
#define _SERIAL_MAX_SIZE 40
int _Serial_Command = 0;
int _SerialInByteCounter = 0;                   // number of bytes counter
byte _SerialInByte;                             // incoming character value
char _InputBuffer_Serial[_SERIAL_MAX_SIZE + 1]; // Buffer for Serial data
String _InputBuffer_Serial_String;
bool _Get_Serial_Commands() {
  if ((_Serial_Command == 1) || (_Serial_Command == 2)) return true;
  if (_Serial_Command > 2) {
    _SerialInByteCounter = 0;
  }
  _Serial_Command = 0;

  // *********************************************
  // Collect serial bytes until "\n" is detected
  // *********************************************
  while (Serial.available()) {
    _SerialInByte = Serial.read();

    // *********************************************
    // If "\n", line is complete
    // *********************************************
    if (_SerialInByte == '\n') { // new line character
      _InputBuffer_Serial[_SerialInByteCounter] = 0;
      _InputBuffer_Serial_String = String(_InputBuffer_Serial);
      _Serial_Command = 1;
      return true;
    } else if (isprint(_SerialInByte)) {
      if (_SerialInByteCounter < (_SERIAL_MAX_SIZE - 1)) {
        _InputBuffer_Serial[_SerialInByteCounter++] = _SerialInByte;
      }
      // *********************************************
      // otherwise clear the buffer and start all over again
      // *********************************************
      else {
        _SerialInByteCounter = 0;
      }
    }
  }
  return false;
}

// ***********************************************************************
// ***********************************************************************
void Create_HTML_Values_Page() {
  // *******************************************************************************
  // *******************************************************************************
  // *******************************************************************************
  // Now create webpage for current values
  // *******************************************************************************
#ifdef FileSystem_SPIFFS
  fs::File HTML_File = SPIFFS.open(F("/Values.html"), "w");
  HTML_File.print(FPSTR(HTML_Values_Pagina_Begin));
  HTML_File.println(F("<table style=\"width:40%\">"));
  HTML_File.println(F("<tr> <td><b>Parameter</b></td> <td><b>Value</b></td> </tr>"));

  My_StringSplitter *Splitter = new My_StringSplitter(JSON_Long_Header, '\t');
  String Table;
  for (int i = 0; i < Splitter->getItemCount(); i++) {
    String Key = Splitter->getItemAtIndex(i);
    Table = F("<tr> <td>");
    Table += Key;
    Table += F("</td> <td> <span id=\"");
    Table += Key;
    Table += F("\"> Value </span> </td> </tr>");
    HTML_File.println(Table);
  }
  HTML_File.println(F("</table>"));
  HTML_File.print(FPSTR(HTML_Values_Pagina_End));
  HTML_File.close();
#endif
}

unsigned long LuftDaten_Sample_Count = 0;
#ifdef YES_INCLUDE_RECEIVER_LUFTDATEN
  #include "Clients/LuftDaten.h"
#endif

// ****************************************************************************
// Here all available sensors are included
//   and the base class, from which all sensors should be derived.
// ****************************************************************************
#include "Sensors/Sensor_Base.h"

#ifndef NOT_INCLUDE_SENSOR_ADS1115
  #include "Sensors/Sensor_ADS1115.h"
#endif
#ifndef NOT_INCLUDE_SENSOR_ADS1115_DIFF
  #include "Sensors/Sensor_ADS1115_Diff3.h"
#endif

#ifdef YES_INCLUDE_SENSOR_BLAUWE_ENGEL
  #include "Sensors/Sensor_Blauwe_Engel.h"
#endif

#ifndef NOT_INCLUDE_SENSOR_BME280
  #include "Sensors/Sensor_BME280_AF.h"
#endif

#ifdef YES_INCLUDE_SENSOR_BMP280
  #include "Sensors/Sensor_BMP280.h"
#endif

#ifndef NOT_INCLUDE_SENSOR_DHT22
  #include "Sensors/Sensor_DHT22.h"
#endif

#ifndef NOT_INCLUDE_SENSOR_DS18B20
  #include "Sensors/Sensor_DS18B20.h"
#endif

#include "Sensors/Sensor_Dummy.h"

#ifndef ESP32
  #include "Sensors/Sensor_EnergyMeter.h"
#endif

#ifdef ESP32
  #include "Sensors/Sensor_ESP32_ADC.h"
#endif
#include "Sensors/Sensor_I2C_Scan.h"

#ifdef INCLUDE_IR_SAMSUNG_TV
  #include "Sensors/Sensor_IR_Samsung_TV.h"
#endif

#ifndef NOT_INCLUDE_SENSOR_MHZ14
  #include "Sensors/Sensor_MHZ14.h"
#endif

#ifdef YES_INCLUDE_SENSOR_MLX90614
  #include "Sensors/Sensor_MLX90614.h"
#endif

#ifdef ESP32
  #ifdef YES_INCLUDE_SENSOR_MLX90640
    #include "Sensors/Sensor_MLX90640.h"
  #endif
#endif

#ifndef NOT_INCLUDE_SENSOR_MPU9250
  #include "Sensors/Sensor_MPU9250.h"
#endif

#ifndef ESP32
  #ifdef YES_INCLUDE_SENSOR_MQTTBroker
    #include "Sensors/Sensor_MQTTBroker.h"
  #endif
#endif

#include "Sensors/Sensor_Noise_100.h"

#ifdef YES_INCLUDE_SENSOR_NTP
  #include "Sensors/Sensor_NTP_Client.h"
#endif

#ifndef ESP32
  #ifdef YES_INCLUDE_SENSOR_OKE4
    #include "Sensors/Sensor_OKE4.h"
  #endif
#endif

#ifndef NOT_INCLUDE_SENSOR_PIR
  #include "Sensors/Sensor_PIR.h"
#endif

#ifdef YES_INCLUDE_SENSOR_RFLINK
  #include "Sensors/Sensor_RFLink.h"
#endif

#ifdef YES_INCLUDE_SENSOR_RTC
  #include "Sensors/Sensor_RTC.h"
#endif

#ifndef NOT_INCLUDE_SENSOR_SDS011
  #include "Sensors/Sensor_SDS011.h"
#endif

#ifdef INCLUDE_SLIMME_METER
  #include "Sensors/Sensor_SlimmeMeter.h"
#endif

#ifdef INCLUDE_SONOFF
  #include "Sensors/Sensor_SonOff.h"
#endif

#include "Sensors/Sensor_System.h"
#include "Sensors/Sensor_Timestamp.h"
#include "Sensors/Sensor_Watchdog.h"
#include "Sensors/Sensor_Wifi.h"

#ifdef INCLUDE_SENSOR_WITTY
  #include "Sensors/Sensor_Witty_LDR.h"
#endif

// ****************************************************************************
// ****************************************************************************
_Sensor_BaseClass *Sensor_OKE4;

// *************************************************************************
class _Sensors {
public:
  std::vector<_Sensor_BaseClass *> _Sensor_List;

  // Sensors ***************************************************************
  // ***********************************************************************
  _Sensors() {}

  // Sensors ***************************************************************
  // ***********************************************************************
  _Sensor_BaseClass *Add(_Sensor_BaseClass *Sensor) {
    _Sensor_List.push_back(Sensor);

    if (Sensor_OKE4_Available) {
      Serial.print(F("OKE4 sensor found"));
      Sensor_OKE4 = Sensor;
      Sensor_OKE4_Available = false;
    }
    External_Watchdog_Toggle();
    return Sensor;
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void setup() {

    if (Signal_LED > 0) {
      pinMode(Signal_LED, OUTPUT);
      if (Signal_LED_Invert) digitalWrite(Signal_LED, HIGH);
      else
        digitalWrite(Signal_LED, LOW);
    }

#ifdef _Main_Name
  #ifndef _Main_Version
    #define _Main_Version "????"
  #endif
    Serial.println("  MAIN    V" + String(_Main_Version) + "   " + String(_Main_Name));
#else
    Serial.println("V?    UNKNOWN PROGRAM");
#endif

    Serial.println("CREATE    V" + String(Sensor_Receiver_h) + "  ======  Sensor_Receiver_h");
    JSON_Data.reserve(1000);
    JSON_Short_Data.reserve(200);
    JSON_Short_Header.reserve(200);
    JSON_Long_Header.reserve(300);
    JSON_Short_Header = F("Seconds\t");
    JSON_Long_Header = "";
    int Debug_Counter = 0;
    for (auto Sensor : _Sensor_List) {
      Serial.print(F("\n======  SETUP  ======  "));
      Serial.println(Sensor->Version_Name);
      Sensor->Print_Help();

      Sensor->setup();
      Debug_Counter += 1;

      JSON_Short_Header += Sensor->_JSON_Short_Header;
      JSON_Long_Header += Sensor->_JSON_Long_Header;

#ifndef NOT_INCLUDE_RECEIVER_MQTT
      if (Sensor->MQTT_Callback_Topic.length() > 0) {
        if (Sensor->MQTT_Callback_Topic.endsWith("/")) MQTT_Topics_Append(Sensor->MQTT_Callback_Topic + "#");
        else
          MQTT_Topics_Append(Sensor->MQTT_Callback_Topic);
        Serial.println("SENSOR   MQTT CALLBACK  = " + Sensor->MQTT_Callback_Topic);
      }
#endif
      External_Watchdog_Toggle();
    }

    JSON_Short_Header.remove(JSON_Short_Header.length() - 1);
    JSON_Long_Header.remove(JSON_Long_Header.length() - 1);
    Serial.println("JSON_Short_Header: " + JSON_Short_Header);
    Serial.println("JSON_Long_Header : " + JSON_Long_Header);

#ifndef NOT_INCLUDE_RECEIVER_MQTT
    MQTT_Topics_Print(F("Sensors"));
#endif

    Serial.println("=======================  SETUP  SENSORS  DONE  ==================================");
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  bool Check_Modified_Settings() {
    int Debug_Counter = 0;
    bool Restart_Needed = false;
    for (auto Sensor : _Sensor_List) {
      String Line = "SETTINGS Sensor [" + String(Debug_Counter) + "] = ";
      Line += Sensor->Version_Name;
      Serial.println(Line);

      bool Result = Sensor->Check_Modified_Settings();
      if (Result) Restart_Needed = true;
      Serial.println("Restart Needed " + String(Result));
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
    return Restart_Needed;
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void Default_Settings(bool Force = true) {
    int Debug_Counter = 0;
    for (auto Sensor : _Sensor_List) {
      String Line = "DEFAULT SETTINGS Sensor [" + String(Debug_Counter) + "] = ";
      Line += Sensor->Version_Name;
      Serial.println(Line);

      Sensor->Default_Settings(Force);
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void _Do_Signal_LED() {
    if (Signal_LED < 0) {
      return;
    }

    switch (_Signal_LED_State) {
      case 0:
        if (_Signal_LED_N > 0) {
          _Signal_LED_State += 1;
          pinMode(Signal_LED, OUTPUT);
        }
        break;

      case 1:
        _Signal_LED_start = millis();
        _Signal_LED_State += 1;
        if (Signal_LED_Invert) digitalWrite(Signal_LED, LOW);
        else
          digitalWrite(Signal_LED, HIGH);
        break;

      case 2:
        if ((millis() - _Signal_LED_start) > _Signal_LED_ON) {
          _Signal_LED_start = millis();
          _Signal_LED_State += 1;
          if (Signal_LED_Invert) digitalWrite(Signal_LED, HIGH);
          else
            digitalWrite(Signal_LED, LOW);
        }
        break;

      case 3:
        if ((millis() - _Signal_LED_start) > _Signal_LED_OFF) {
          _Signal_LED_N -= 1;
          if (_Signal_LED_N > 0) _Signal_LED_State = 1;
          else
            _Signal_LED_State = 0;
        }
        break;

      default:
        _Signal_LED_State = 0;
        _Signal_LED_N = 0;
        break;
    }
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void Set_Signal_LED(int N, int On, int Off) {
    _Signal_LED_N = N;
    _Signal_LED_ON = On;
    _Signal_LED_OFF = Off;
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void loop() {
    this->_Do_Signal_LED();
    if (Loop_Priority > 0) return;

    for (auto Sensor : _Sensor_List) {
      Sensor->loop();
      yield();
      External_Watchdog_Toggle();
    }

    if (Loop_Priority == 0) {
      if (_Get_Serial_Commands()) {
        bool Done = false;
        for (auto Sensor : _Sensor_List) {
          if (Sensor->Handle_Serial_Command(_InputBuffer_Serial_String)) {
            Done = true;
            break;
          }
          yield();
          External_Watchdog_Toggle();
        }
        if (Done) _Serial_Command = 3;
        else
          _Serial_Command += 1;
      }
    }
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  bool Pushed() {
    bool Push = false;
    for (auto Sensor : _Sensor_List) {
      if (Sensor->Pushed()) {
        Push = true;
      }
    }
    return Push;
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void Send_IR(unsigned long Data) {
    int Debug_Counter = 0;
    for (auto Sensor : _Sensor_List) {
      Sensor->Send_IR(Data);
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void Print() {
    Serial.println(F("=======================  My_Numbers  ======================="));
    int Debug_Counter = 0;
    for (auto Sensor : _Sensor_List) {
      Serial.println("Something");
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data = "{";
    JSON_Short_Data = "";

    int Debug_Counter = 0;
    for (auto Sensor : _Sensor_List) {
      Sensor->Get_JSON_Data();
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
    if (JSON_Data.length() > 3) {
      JSON_Data.remove(JSON_Data.length() - 1);
      JSON_Data += "}";
    } else
      JSON_Data = "";

    if (JSON_Short_Data.length() > 1) {
      JSON_Short_Data.remove(JSON_Short_Data.length() - 1);
    } else
      JSON_Short_Data = "";
  }

#ifdef YES_INCLUDE_RECEIVER_LUFTDATEN
  // Sensors ***************************************************************
  // ***********************************************************************
  void Get_JSON_LuftData() {
    JSON_LuftData = LuftDaten_First_Part;
    int Debug_Counter = 0;
    for (auto Sensor : _Sensor_List) {
      JSON_LuftData += Sensor->Get_JSON_LuftData();
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }

    Complete_JSON_String(LuftDaten_Sample_Count);
    LuftDaten_Sample_Count += 1;
    return;
  }
#endif

  // Sensors ***************************************************************
  // FOUT  FOUT  FOUT
  // ***********************************************************************
  int Get_JSON_Length(int Sensor_Nr) {
    int N_Samples;
    for (auto Sensor : _Sensor_List) {
      N_Samples = Sensor->Get_JSON_Length();
    }
    return N_Samples;
  }

  // Sensors ***************************************************************
  // ***********************************************************************
  void Hardware_Test(int Test_Nr = 1) {
    int Debug_Counter = 0;
    for (auto Sensor : _Sensor_List) {
      Sensor->Hardware_Test(Test_Nr);
      Debug_Counter += 1;
      External_Watchdog_Toggle();
      break;
    }
  }

  // Sensors ***************************************************************
  // ***********************************************************************
private:
  int _Signal_LED_State = 0;
  int _Signal_LED_N = 0;
  int _Signal_LED_ON = 0;
  int _Signal_LED_OFF = 0;
  unsigned long _Signal_LED_start;
};

// ****************************************************************************
// Define all Receivers
// ****************************************************************************
#include "Receivers/Receiver_Base.h"
#ifdef YES_INCLUDE_FIJNSTOF_CONDITIONERING
  #include "Receivers/Receiver_Fijnstof_Conditionering.h"
#endif
#ifdef YES_INCLUDE_RECEIVER_LUFTDATEN
  #include "Receivers/Receiver_Luftdaten.h"
  #include "Receivers/Receiver_Madavi.h"
#endif

#ifndef NOT_INCLUDE_RECEIVER_MQTT
  #include "Receivers/Receiver_MQTT.h"
#endif

#ifdef YES_INCLUDE_RECEIVER_OTA
  #include "Receivers/Receiver_OTA.h"
#endif

#ifndef NOT_INCLUDE_RECEIVER_SCRATCHPAD
  #include "Receivers/Receiver_ScratchPad.h"
#endif

// **************************
// File Systems
// **************************
#ifdef FileSystem_SPIFFS
  #include "Receivers/Receiver_SPIFFS.h"
#endif

// ***********************************************************************************
// ***********************************************************************************
void Settings_Factory() {
  fs::File Source = SPIFFS.open(F("/Factory.h"), "r");
  fs::File Dest = SPIFFS.open(F("/Settings.h"), "w");
#define _Settings_Factory_BufSize 100
  uint8_t buf[_Settings_Factory_BufSize];
  while (Source.read(buf, _Settings_Factory_BufSize)) {
    Dest.write(buf, _Settings_Factory_BufSize);
  }
  Dest.close();
  Source.close();

  ESP.restart();
}

#ifndef NOT_INCLUDE_RECEIVER_SDFAT
  #include "Receivers/Receiver_SDfat.h"
#endif

#ifdef FileSystem_SDMMC
  #ifdef ESP32
    #include "Receivers/Receiver_SDMMC.h"
  #endif
#endif

#ifdef FileSystem_SDcard
  #ifdef ESP32
    #include "Receivers/Receiver_SDcard.h" // THIS IS REALLY REGULAR PROBLEMS !!!!
  #endif
#endif
// **************************
// **************************

#include "Receivers/Receiver_Serial.h"

#ifndef NOT_INCLUDE_RECEIVER_EMAIL
  #include "Receivers/Receiver_Email.h"
#endif

#ifndef NOT_INCLUDE_RECEIVER_SSD1306
  #include "Receivers/Receiver_SSD1306.h"
#endif

#ifndef NOT_INCLUDE_RECEIVER_TM1638
  #include "Receivers/Receiver_TM1638.h"
#endif

#ifdef YES_INCLUDE_RECEIVER_WEBSERVER
  #include "Receivers/Receiver_Webserver.h"
#endif

// *************************************************************************
// *************************************************************************
class _Receivers {
public:
  std::vector<_Receiver_BaseClass *> _Receiver_List;

  // Receivers *************************************************************
  // ***********************************************************************
  _Receivers() {
    // **************************************************
    // Dummy device,
    // so calls to _Character_Display can be made safely,
    // even if no real _Character_Display exists
    // **************************************************
    _Character_Display = new _Receiver_BaseClass();
  }

  // Receivers *************************************************************
  // ***********************************************************************
  _Receiver_BaseClass *Add(_Receiver_BaseClass *Receiver) {
    _Receiver_List.push_back(Receiver);

    if (_Debug_MQTT_Available) {
      Serial.println(F("MQTT client found, added for debugging"));
      My_MQTT_Client = Receiver;
      _Debug_Over_MQTT = true;
      _Debug_MQTT_Available = false;
      _MQTT_Client_Available = true;
    }

    if (Receiver->Character_Display) {
      Serial.println("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
      _Character_Display = Receiver;
    }

#ifndef NOT_INCLUDE_RECEIVER_SDFAT
    if (Receiver->_Is_Receiver_SDFat) {
      _p_Receiver_SDfat = Receiver;
    }
#endif

    if (Receiver->_Is_Receiver_Email) {
      _p_Receiver_Email = Receiver;
    }

    External_Watchdog_Toggle();
    return Receiver;
  }

  // Receivers *************************************************************
  // ***********************************************************************
  void setup() {
    if (Allow_Wifi_Not_Found) {
      Serial.println(F("WIFI_ALLOW_NOT_FOUND = true !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    }
    int Debug_Counter = 0;
    for (auto Receiver : _Receiver_List) {
      Receiver->setup();
      Debug_Counter += 1;
      Serial.print(F("\n======  SETUP  ======  "));
      Serial.println(Receiver->Version_Name);
      Receiver->Print_Help();

#ifndef NOT_INCLUDE_RECEIVER_MQTT
      if (Receiver->MQTT_Callback_Topic.length() > 0) {
        if (Receiver->MQTT_Callback_Topic.endsWith("/")) MQTT_Topics_Append(Receiver->MQTT_Callback_Topic + "#");
        else
          MQTT_Topics_Append(Receiver->MQTT_Callback_Topic);
        Serial.println("RECEIVER MQTT CALLBACK  = " + Receiver->MQTT_Callback_Topic);
      }
#endif
      External_Watchdog_Toggle();
    }

#ifndef NOT_INCLUDE_RECEIVER_MQTT
    MQTT_Topics_Print(F("Receivers"));
#endif

#ifdef ESP32

    Serial.println(F("===============  ESP 32 parameters ====================== "));
    Serial.println("       CPU FREQUENCY        = " + String(ESP.getCpuFreqMHz()) + " MHz");
    Serial.println("       SDK Version          = " + String(ESP.getSdkVersion()));
    // Serial.println ( "       SketchSize           = " + String ( ESP.getSketchSize() ) ) ;
    // Serial.println ( "       Free Sketch Space    = " + String ( ESP.getFreeSketchSpace() ) ) ;
    Serial.println("       Flash Chip Size      = " + String(ESP.getFlashChipSize()));
    Serial.println("       Flash Chip Frequency = " + String(ESP.getFlashChipSpeed()));
    Serial.print(F("       IP Address           = "));
    Serial.println(WiFi.localIP());
    Serial.println();
#else
    Serial.println(F("===============  ESP 8266 parameters ====================== "));
    Serial.println("       CPU FREQUENCY        = " + String(ESP.getCpuFreqMHz()) + " MHz");
    // Serial.println ( "       Heap Fragmentation   = " + String ( ESP.getHeapFragmentation() ) + " %" ) ;
    // Serial.println ( "       Max Allocatable RAM  = " + String ( ESP.getMaxFreeBlockSize() ) ) ;
    Serial.println("       Core Version         = " + String(ESP.getCoreVersion()));
    Serial.println("       SDK Version          = " + String(ESP.getSdkVersion()));
    Serial.println("       SketchSize           = " + String(ESP.getSketchSize()));
    Serial.println("       Free Sketch Space    = " + String(ESP.getFreeSketchSpace()));
    Serial.println("       Flash Chip Size      = " + String(ESP.getFlashChipSize()));
    Serial.println("       Real Flash Chip Size = " + String(ESP.getFlashChipRealSize()));
    Serial.println("       Flash Chip Frequency = " + String(ESP.getFlashChipSpeed()));
    Serial.print(F("       IP Address           = "));
    Serial.println(WiFi.localIP());
    Serial.println();
#endif

    Serial.println("=======================  SETUP  RECEIVERS  DONE  ==================================");

    Create_HTML_Values_Page();

    if (_DEBUG_Global_String.length() > 0) {
      Serial.println("======  _DEBUG_Global_String =====");
      Serial.println(_DEBUG_Global_String);
      Serial.println("======  END _DEBUG_Global_String =====");
    }
  }

  // Receivers *************************************************************
  // ***********************************************************************
  bool Check_Modified_Settings() {
    int Debug_Counter = 0;
    bool Restart_Needed = false;
    for (auto Receiver : _Receiver_List) {
      String Line = "SETTINGS Receiver [" + String(Debug_Counter) + "] = ";
      Line += Receiver->Version_Name;
      Serial.println(Line);

      bool Result = Receiver->Check_Modified_Settings();
      if (Result) Restart_Needed = true;
      Serial.println("Restart Needed " + String(Result));
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
    return Restart_Needed;
  }

  // Receivers *************************************************************
  // ***********************************************************************
  void Default_Settings(bool Force = true) {
    int Debug_Counter = 0;
    for (auto Receiver : _Receiver_List) {
      String Line = "DEFAULT SETTINGS Reveiver [" + String(Debug_Counter) + "] = ";
      Line += Receiver->Version_Name;
      Serial.println(Line);

      Receiver->Default_Settings(Force);
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
  }

  // Receivers *************************************************************
  // ***********************************************************************
  void loop() {
    int Debug_Counter = 0;
    for (auto Receiver : _Receiver_List) {
      if (Receiver->High_Priority_Loop >= Loop_Priority) {
        Receiver->loop();
      }
      yield();
      External_Watchdog_Toggle();
    }

    if (Loop_Priority == 0) {
      if (_Get_Serial_Commands()) {
        bool Done = false;
        for (auto Receiver : _Receiver_List) {
          if (Receiver->Handle_Serial_Command(_InputBuffer_Serial_String)) {
            Done = true;
            break;
          }
          Debug_Counter += 1;
          yield();
          External_Watchdog_Toggle();
        }
        if (Done) _Serial_Command = 3;
        else
          _Serial_Command += 1;
        _SerialInByteCounter = 0;
      }
    }
  }

  // Receivers *************************************************************
  // ***********************************************************************
  void Print() {
    Serial.println(F("=======================  My_Numbers  ======================="));
    int Debug_Counter = 0;
    for (auto Receiver : _Receiver_List) {
      Serial.println(F("Something Else"));
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
  }

  // Receivers *************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) {
    if (JSON_Message.length() < 3) return false;
    bool Result_OK = true;
    int Debug_Counter = 0;
    for (auto Receiver : _Receiver_List) {
      if (!Receiver->Send_Data(JSON_Message)) {
        Result_OK = false;
      }
      Debug_Counter += 1;
      External_Watchdog_Toggle();
    }
    return Result_OK;
  }

  // Receivers *************************************************************
  // ***********************************************************************
private:
};

// ********************************************************************************
// Create Sensor and Receiver list
// ********************************************************************************
_Sensors Sensors;
_Receivers Receivers;

// ***********************************************************************
// ***********************************************************************
void Update_All_Headers() {
  JSON_Short_Header = "";
  JSON_Long_Header = "";
  for (auto Sensor : Sensors._Sensor_List) {
    JSON_Short_Header += Sensor->_JSON_Short_Header;
    JSON_Long_Header += Sensor->_JSON_Long_Header;
  }
  JSON_Short_Header.remove(JSON_Short_Header.length() - 1);
  JSON_Long_Header.remove(JSON_Long_Header.length() - 1);
  Create_HTML_Values_Page();
}

// ***********************************************************************
// Witty:   Green=12,  Blue=13,  Red=15,  Builtin=2
// ***********************************************************************
void Set_Signal_LED(int N = 5, int On = 50, int Off = 400) {
  Serial.println("*0*0*0*0*  Signal LED *0*0*0*0*  " + String(N) + " / " + String(On) + " / " + String(Off));
  Sensors.Set_Signal_LED(N, On, Off);
}

// ********************************************************************************
// ********************************************************************************
void Store_Modified_Settings_From_Webserver(bool Restart_Needed) {

  // *******************************************************
  // Check general settings for changes
  // *******************************************************
  String Key;
  int New_Value_Int;
  int Baudrate = Settings.Read_Int("RS232 Baudrate");
  int Period = Settings.Read_Int("Sample Period[ms]");
  int Signal_LED = Settings.Read_Int("Signal LED");
  bool FTP_SD_MMC = Settings.Read_Bool("FTP on SD-card");
  int FTP_SPIFFS = Settings.Read_Int("FTP on SPIFFS");
  for (int i = 0; i < My_Webserver.args(); i++) {
    Key = My_Webserver.argName(i);
    New_Value_Int = My_Webserver.arg(i).toInt();

    if (Key == F("RS232 Baudrate")) {
      if (New_Value_Int != Baudrate) {
        _My_Settings_Buffer[F("RS232 Baudrate")] = New_Value_Int;
        Restart_Needed = true;
      }
    } else if (Key == F("Sample Period[ms]")) {
      if (New_Value_Int != Period) {
        _My_Settings_Buffer[F("Sample Period[ms]")] = New_Value_Int;
        Restart_Needed = true;
      }
    } else if (Key == F("Signal LED")) {
      if (New_Value_Int != Signal_LED) {
        _My_Settings_Buffer[F("Signal LED")] = New_Value_Int;
        Restart_Needed = true;
      }
    } else if (Key == F("FTP on SD-card")) {
      Serial.println("FTP SDCARD " + String(New_Value_Int) + " / " + String(FTP_SD_MMC));
      if (!FTP_SD_MMC) {
        _My_Settings_Buffer[F("FTP on SD-card")] = true;
        Restart_Needed = true;
      }
    } else if (Key == F("FTP on SPIFFS")) {
      Serial.println("FTP SPIFFS " + String(New_Value_Int) + " / " + String(FTP_SPIFFS));
      if (New_Value_Int != FTP_SPIFFS) {
        _My_Settings_Buffer[F("FTP on SPIFFS")] = New_Value_Int;
        Restart_Needed = true;
      }
    }
  }

  // apparently fast boolean evalustion, so watch order !!!
  Restart_Needed = Sensors.Check_Modified_Settings() || Restart_Needed;
  Restart_Needed = Receivers.Check_Modified_Settings() || Restart_Needed;

  Serial.println("Restart Needed total " + String(Restart_Needed));

  // *******************************************************
  // If checkbox DEFAULT SETTINGS checked
  // *******************************************************
  for (int i = 0; i < My_Webserver.args(); i++) {
    if (My_Webserver.argName(i) == "All_Default_Values") {
      Sensors.Default_Settings(true);
      Receivers.Default_Settings(true);
      Restart_Needed = true;
      break;
    }
  }

  Settings.Store_Settings();
  Settings.Create_WebPage();

  if (Restart_Needed) {
    Serial.println(F("*****  Restart Needed  *****  Restart Needed  *****  Restart Needed  *****  Restart Needed  "
                     "*****  Restart Needed  "));
    ESP.restart();
  }
}

// ***********************************************************************************
// Special function (only for ESP8266) to sed a mail on (re-)start
// Call this function before any sensor or receiver is added!!!!
// ***********************************************************************************
void Restart_Email(String MailTo, String Subject, String Body, bool HTML_Format = false) {
#ifndef ESP32
  _Email_Client_Class My_Mail_Client(__SECRET_SMTP_Server, __SECRET_SMTP_Port, __SECRET_SMTP_User, __SECRET_SMTP_PWD);

  bool Result;

  Result = My_Mail_Client.Send_Mail(MailTo, Subject, Body);
  if (Result) Serial.println("Email Succeeded");
  else
    Serial.println("Email Failed");
#endif
}

// ********************************************************************************
// ********************************************************************************
void Send_Email(String Mail_To, String Subject, String Body, bool HTML_Format = false) {
#ifndef NOT_INCLUDE_RECEIVER_EMAIL
  if (_p_Receiver_Email != NULL) {
    ((_Receiver_Email *)_p_Receiver_Email)->Send_Email(Mail_To, Subject, Body, HTML_Format);
  }
#endif
}

int _Loop_Send_Time = 10000;
unsigned long _Loop_Last_Time = 0;
// ***********************************************************************************
// ***********************************************************************************
void Settings_Setup() {
  Settings_By_WebInterface = true;

  Serial.begin(115200);
  Settings.Setup();

  // ************************************************************
  // ************************************************************

#ifndef NOT_INCLUDE_RECEIVER_EMAIL
  #ifndef ESP32
  Restart_Email(__SECRET_SMTP_MailTo, "RFLink Restarted", "Body");
  #endif
#endif

  int Baudrate = Settings.Get_Set_Default_Int("RS232 Baudrate", 115200);
  Serial_Setup(Baudrate);

#ifdef ESP32
  My_Prefs.begin("My_Prefs", false); // not readonly
  uint8_t Reboot_Options = My_Prefs.getChar("Reboot_Options", 0);
  Serial.println("                   Reboot Options : " + String(Reboot_Options));
#endif

  _Loop_Send_Time = Settings.Get_Set_Default_Int(F("Sample Period[ms]"), 10000);
  Signal_LED = Settings.Get_Set_Default_Int(F("Signal LED"), -1);

  // ************************************************************
  //  Sensors / Receivers that must always be present
  // ************************************************************
  _My_Settings_Buffer[F("Sensor_Wifi")] = true;
  Sensors.Add(new _Sensor_Wifi(F("FS")));
  // the idea here was to make sure that WiFi is fast, so that telnet can display everything

#ifdef YES_INCLUDE_RECEIVER_TELNET
  _My_Settings_Buffer[F("Receiver_Telnet")] = true;
  Receivers.Add(new _Receiver_Telnet());
#else
  _My_Settings_Buffer.remove(F("Receiver_Telnet"));
#endif

#ifdef FileSystem_SPIFFS
  _My_Settings_Buffer[F("Receiver_SPIFFS")] = true;
  Receivers.Add(new _Receiver_SPIFFS(684));
#else
  _My_Settings_Buffer.remove(F("Receiver_SPIFFS"));
#endif

#ifdef YES_INCLUDE_RECEIVER_WEBSERVER
  _My_Settings_Buffer[F("Receiver_Webserver")] = true;
  Receivers.Add(new _Receiver_Webserver());
#else
  _My_Settings_Buffer.remove(F("Receiver_Webserver"));
#endif

  FTP_Server_SD_MMC = false;
  FTP_Server_SPIFFS = false;
  if (Settings.Read_Bool(F("FTP on SD-card"))) {
    _My_Settings_Buffer[F("FTP on SPIFFS")] = false;
    FTP_Server_SD_MMC = true;
  } else if (Settings.Read_Bool(F("FTP on SPIFFS"))) {
    _My_Settings_Buffer[F("FTP on SD-card")] = false;
    FTP_Server_SPIFFS = true;
  }

  // ************************************************************
  // Create Sensors (and if not needed, make sure it's in the settings file)
  // ************************************************************
  if (Settings.Read_Bool(F("Sensor_Watchdog"))) Sensors.Add(new _Sensor_Watchdog());
  else
    _My_Settings_Buffer[F("Sensor_Watchdog")] = false;

#ifndef NOT_INCLUDE_SENSOR_ADS1115
  if (Settings.Read_Bool(F("Sensor_ADS1115"))) Sensors.Add(new _Sensor_ADS1115("FS"));
  else
    _My_Settings_Buffer[F("Sensor_ADS1115")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_ADS1115"));
#endif

#ifndef NOT_INCLUDE_SENSOR_ADS1115_DIFF
  if (Settings.Read_Bool(F("Sensor_ADS1115_Diff3"))) Sensors.Add(new _Sensor_ADS1115_Diff3("FS"));
  else
    _My_Settings_Buffer[F("Sensor_ADS1115_Diff3")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_ADS1115_Diff3"));
#endif

#ifndef NOT_INCLUDE_SENSOR_BME280
  if (Settings.Read_Bool(F("Sensor_BME280"))) Sensors.Add(new _Sensor_BME280_AF("FS"));
  else
    _My_Settings_Buffer[F("Sensor_BME280")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_BME280"));
#endif

#ifdef YES_INCLUDE_BLAUWE_ENGEL
  if (Settings.Read_Bool(F("Sensor_Blauwe_Engel"))) Sensors.Add(new _Sensor_Blauwe_Engel("FS"));
  else
    _My_Settings_Buffer[F("Sensor_Blauwe_Engel")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_Blauwe_Engel"));
#endif

#ifndef NOT_INCLUDE_SENSOR_DHT22
  if (Settings.Read_Bool(F("Sensor_DHT22"))) Sensors.Add(new _Sensor_DHT22());
  else
    _My_Settings_Buffer[F("Sensor_DHT22")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_DHT22"));
#endif

#ifndef NOT_INCLUDE_SENSOR_DS18B20
  if (Settings.Read_Bool(F("Sensor_DS18B20"))) Sensors.Add(new _Sensor_DS18B20("FS"));
  else
    _My_Settings_Buffer[F("Sensor_DS18B20")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_DS18B20"));
#endif

  if (Settings.Read_Bool(F("Sensor_Dummy"))) Sensors.Add(new _Sensor_Dummy());
  else
    _My_Settings_Buffer[F("Sensor_Dummy")] = false;

  if (Settings.Read_Bool(F("Sensor_I2C_Scan"))) Sensors.Add(new _Sensor_I2C_Scan("FS"));
  else
    _My_Settings_Buffer[F("Sensor_I2C_Scan")] = false;

#ifndef NOT_INCLUDE_SENSOR_MHZ14
  if (Settings.Read_Bool(F("Sensor_MHZ14"))) Sensors.Add(new _Sensor_MHZ14("FS"));
  else
    _My_Settings_Buffer[F("Sensor_MHZ14")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_MHZ14"));
#endif

#ifdef YES_INCLUDE_SENSOR_MLX90614
  if (Settings.Read_Bool(F("Sensor_MLX90614"))) Sensors.Add(new _Sensor_MLX90614());
  else
    _My_Settings_Buffer[F("Sensor_MLX90614")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_MLX90614"));
#endif

#ifdef ESP32
  #ifdef YES_INCLUDE_SENSOR_MLX90640
  if (Settings.Read_Bool(F("Sensor_MLX90640"))) Sensors.Add(new _Sensor_MLX90640());
  else
    _My_Settings_Buffer[F("Sensor_MLX90640")] = false;
  #else
  _My_Settings_Buffer.remove(F("Sensor_MLX90640"));
  #endif
#endif

#ifndef NOT_INCLUDE_SENSOR_MPU9250
  if (Settings.Read_Bool(F("Sensor_MPU9250"))) Sensors.Add(new _Sensor_MPU9250());
  else
    _My_Settings_Buffer[F("Sensor_MPU9250")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_MPU9250"));
#endif

#ifndef ESP32
  #ifdef YES_INCLUDE_SENSOR_MQTTBroker
  if (Settings.Read_Bool(F("Sensor_MQTTBroker"))) Sensors.Add(new _Sensor_MQTTBroker());
  else
    _My_Settings_Buffer[F("Sensor_MQTTBroker")] = false;
  #else
  _My_Settings_Buffer.remove(F("Sensor_MQTTBroker"));
  #endif
#endif

  if (Settings.Read_Bool(F("Sensor_Noise_100"))) Sensors.Add(new _Sensor_Noise_100());
  else
    _My_Settings_Buffer[F("Sensor_Noise_100")] = false;

#ifdef YES_INCLUDE_SENSOR_NTP
  if (Settings.Read_Bool(F("Sensor_NTP_Client"))) Sensors.Add(new _Sensor_NTP_Client());
  else
    _My_Settings_Buffer[F("Sensor_NTP_Client")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_NTP_Client"));
#endif

#ifdef YES_INCLUDE_SENSOR_RTC
  if (Settings.Read_Bool(F("Sensor_RTC"))) Sensors.Add(new _Sensor_RTC());
  else
    _My_Settings_Buffer[F("Sensor_RTC")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_RTC"));
#endif

#ifdef YES_INCLUDE_SENSOR_RFLINK
  if (Settings.Read_Bool(F("Sensor_RFLink"))) Sensors.Add(new _Sensor_RFLink("FS"));
  else
    _My_Settings_Buffer[F("Sensor_RFLink")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_RFLink"));
#endif

#ifndef NOT_INCLUDE_SENSOR_SDS011
  if (Settings.Read_Bool(F("Sensor_SDS011"))) Sensors.Add(new _Sensor_SDS011());
  else
    _My_Settings_Buffer[F("Sensor_SDS011")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_SDS011"));
#endif

  if (Settings.Read_Bool(F("Sensor_System"))) Sensors.Add(new _Sensor_System());
  else
    _My_Settings_Buffer[F("Sensor_System")] = false;

#ifdef INCLUDE_SENSOR_WITTY
  if (Settings.Read_Bool(F("Sensor_Witty_LDR"))) Sensors.Add(new _Sensor_Witty_LDR());
  else
    _My_Settings_Buffer[F("Sensor_Witty_LDR")] = false;
#else
  _My_Settings_Buffer.remove(F("Sensor_Witty_LDR"));
#endif

    // ************************************************************
    // create Receivers
    // ************************************************************
#ifdef YES_INCLUDE_FIJNSTOF_CONDITIONERING
  if (Settings.Read_Bool(F("Receiver_Fijnstof_Conditionering"))) Receivers.Add(new _Receiver_Fijnstof_Conditionering());
  else
    _My_Settings_Buffer[F("Receiver_Fijnstof_Conditionering")] = false;
#else
  _My_Settings_Buffer.remove(F("Receiver_Fijnstof_Conditionering"));
#endif

#ifndef NOT_INCLUDE_RECEIVER_EMAIL
  if (Settings.Read_Bool(F("Receiver_Email"))) Receivers.Add(new _Receiver_Email());
  else
    _My_Settings_Buffer[F("Receiver_Email")] = false;
#else
  _My_Settings_Buffer.remove(F("Receiver_Email"));
#endif

#ifndef NOT_INCLUDE_RECEIVER_MQTT
  if (Settings.Read_Bool(F("Receiver_MQTT"))) Receivers.Add(new _Receiver_MQTT(684));
  else
    _My_Settings_Buffer[F("Receiver_MQTT")] = false;
#else
  _My_Settings_Buffer.remove(F("Receiver_MQTT"));
#endif

#ifdef FileSystem_SDMMC
  #ifdef ESP32
  if (Settings.Read_Bool(F("Receiver_SD_MMC"))) Receivers.Add(new _Receiver_SDMMC(684));
  else
    _My_Settings_Buffer[F("Receiver_SD_MMC")] = false;
  #endif
#else
  _My_Settings_Buffer.remove(F("Receiver_SD_MMC"));
#endif

#ifndef NOT_INCLUDE_RECEIVER_SCRATCHPAD
  if (Settings.Read_Bool(F("Receiver_ScratchPad"))) Receivers.Add(new _Receiver_ScratchPad());
  else
    _My_Settings_Buffer[F("Receiver_ScratchPad")] = false;
#else
  _My_Settings_Buffer.remove(F("Receiver_ScratchPad"));
#endif

  if (Settings.Read_Bool(F("Receiver_Serial"))) Receivers.Add(new _Receiver_Serial());
  else
    _My_Settings_Buffer[F("Receiver_Serial")] = false;

#ifndef NOT_INCLUDE_RECEIVER_SSD1306
  if (Settings.Read_Bool(F("Receiver_SSD1306"))) Receivers.Add(new _Receiver_SSD1306());
  else
    _My_Settings_Buffer[F("Receiver_SSD1306")] = false;
#else
  _My_Settings_Buffer.remove(F("Receiver_SSD1306"));
#endif

#ifndef NOT_INCLUDE_RECEIVER_TM1638
  if (Settings.Read_Bool(F("Receiver_TM1638"))) Receivers.Add(new _Receiver_TM1638("FS"));
  else
    _My_Settings_Buffer[F("Receiver_TM1638")] = false;
#else
  _My_Settings_Buffer.remove(F("Receiver_TM1638"));
#endif

#ifdef YES_INCLUDE_RECEIVER_OTA
  if (Settings.Read_Bool(F("Receiver_OTA"))) Receivers.Add(new _Receiver_OTA());
  else
    _My_Settings_Buffer[F("Receiver_OTA")] = false;
#else
  _My_Settings_Buffer.remove(F("Receiver_OTA"));
#endif

  // ************************************************************
  // ************************************************************
  Sensors.setup();
  Receivers.setup();

  // ************************************************************
  // ************************************************************
  Settings.Store_Settings();
  Settings.Create_WebPage();
  File_System.Dump(_My_Settings_Filename);
}

// ***********************************************************************************
// ***********************************************************************************
bool Recording_Stopped = false;
void Settings_Loop() {

  // ***********************************************************
  // call all loop methods
  // ***********************************************************
  Sensors.loop();
  Receivers.loop();

  if ((!Recording) && (!Recording_Stopped)) {
    Recording_Stopped = true;
    Serial.println(F(">>>>>>>>>>>>>>>>>>>>>>>>>>> Recording Stopped"));
  }

  // ***********************************************************
  // for all sensors, get JSON-data and send to all active receivers
  // ***********************************************************
  if (millis() - _Loop_Last_Time >= _Loop_Send_Time) {
    _Loop_Last_Time = millis(); // no need to overtake !!
    if (Recording) {
      Sensors.Get_JSON_Data();
      Receivers.Send_Data(JSON_Data);
    } else
      Serial.println(F("Recording Stopped "));
  }
}

// ********************************************************************************
// Collecton of all MQTT Callback methods
// ********************************************************************************
#ifndef NOT_INCLUDE_RECEIVER_MQTT
void _MQTT_Callback_Wrapper(char *topic, byte *payload, unsigned int length) {
  String Topic = String(topic);
  String Payload = "";
  for (int i = 0; i < length; i++) {
    Payload += (char)payload[i];
  }

  DynamicJsonDocument jsonBuffer(600);  // ArduinoJson V6
  deserializeJson(jsonBuffer, Payload); // ArduinoJson V6

  for (auto Sensor : Sensors._Sensor_List) {
    if ((Topic == Sensor->MQTT_Callback_Topic) ||
        ((Sensor->MQTT_Callback_Topic.endsWith("/")) && (Topic.startsWith(Sensor->MQTT_Callback_Topic)))) {
      Sensor->MQTT_Callback(Topic, Payload, jsonBuffer);
    }
  }

  for (auto Receiver : Receivers._Receiver_List) {
    if ((Topic == Receiver->MQTT_Callback_Topic) ||
        ((Receiver->MQTT_Callback_Topic.endsWith("/")) && (Topic.startsWith(Receiver->MQTT_Callback_Topic)))) {
      Receiver->MQTT_Callback(Topic, Payload, jsonBuffer);
    }
  }
}
#endif

#endif
