// Version 0.6   04-10-2019, SM
//    - storage OTA password corrected
//
// Version 0.5   04-05-2019, SM
//    - on setup, OTA-Hostname naar Serial
//
// Version 0.4   02-01-2019, SM
//    - External_Watchdog_Disarm added
//
// Version 0.3
//    - Password added
//    - WERKT NIET:  if not OTA_HostName defined, try to use MQTT_ID as a host name
//
// Version 0.2
//    - OTA hostname toegevoegd:  #define OTA_HostName  ....
//
// Version 0.1

#ifndef Receiver_OTA_h
#define Receiver_OTA_h 0.6

#include "Receiver_Base.h"
#include <ArduinoOTA.h>

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_OTA : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_OTA() {
    Version_Name = "V" + String(Receiver_OTA_h) + "  ──────  Receiver_OTA.h";
    Serial.println("\n──────  CREATE  ──────  " + Version_Name);
    Help_Text = "BEWARE, this Receiver may significantly degrade FTP server and Web server operation !!!!!";
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    // *********************************************
    // Hostname defaults to esp8266-[ChipID]
    // *********************************************
    this->_Get_Parameters();

    if (_OTA_HostName.length() > 0) {
      ArduinoOTA.setHostname(_OTA_HostName.c_str());
      ArduinoOTA.setPassword(_OTA_PassWord.c_str());
    }
    Serial.println("OTA hostname = " + ArduinoOTA.getHostname());

    ArduinoOTA.onStart([]() { Serial.println("Start"); });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
    });
    ArduinoOTA.begin();
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    this->_Get_Parameters();
    _My_Settings_Buffer["OTA Hostname"] = _OTA_HostName;
    _My_Settings_Buffer["$OTA Password"] = _OTA_PassWord;
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;

    String New_Value;
    String HostName = Settings.Read_String("OTA Hostname");
    String Password = Settings.Read_String("OTA Hostname");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value = My_Webserver.arg(i);

      if (My_Webserver.argName(i) == "OTA Hostname") {
        if ((New_Value != HostName) && (New_Value.length() > 0)) {
          _My_Settings_Buffer["OTA Hostname"] = New_Value;
          ArduinoOTA.setHostname(New_Value.c_str());
        }
      } else if (My_Webserver.argName(i) == "$OTA Password") {
        if ((New_Value != "********") && (New_Value != Password) && (New_Value.length() >= 8)) {
          _My_Settings_Buffer["$OTA Password"] = New_Value;
          ArduinoOTA.setPassword(New_Value.c_str());
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() { ArduinoOTA.handle(); }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) { return true; }

  // ****************************************************************************************
private:
  // ****************************************************************************************
  String _OTA_HostName = "";
  String _OTA_PassWord = "";
  String _Mac;

  // **********************************************************************************************
  // **********************************************************************************************
  void _Get_Parameters() {
    String Value = Settings.Read_String("OTA Hostname");
    if (Value.length() == 0) {
#ifdef _Main_Name
      Value = _Main_Name;
#else
      _Mac = String(WiFi.macAddress());
  #ifdef ESP32
      Value = "ESP32-" + _Mac.substring(9, 11) + _Mac.substring(12, 14) + _Mac.substring(15);
  #else
      Value = "esp8266-" + _Mac.substring(9, 11) + _Mac.substring(12, 14) + _Mac.substring(15);
  #endif
#endif
      _My_Settings_Buffer["OTA Hostname"] = Value;
    }
    _OTA_HostName = Value;

    Value = Settings.Read_String("$OTA Password");
    if (Value.length() == 0) {
      Value = _OTA_HostName;
      _My_Settings_Buffer["$OTA Password"] = Value;
    }
    _OTA_PassWord = Value;
  }
};

#endif
