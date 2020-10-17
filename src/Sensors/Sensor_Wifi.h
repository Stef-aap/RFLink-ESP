// Version 0.3, 23-02-2020, SM, checked by ..
//    - Removed MDNS, uses a lot of RAM (almost 3000)
//    - default WIFI_OPTIONS_MUTIPLE
//
// Version 0.2, 06-08-2019, SM, checked by ..
//    - multiple / single / autoconfig choices added
//
// Version 0.1, 05-08-2019, SM, checked by ..
//    - initial version
// ***********************************************************************************

// setting softap parameters:
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html

// possible alternative to esp8266WiFi
// https://github.com/ekstrand/ESP8266wifi
// ***********************************************************************************

#ifndef Sensor_Wifi_h
#define Sensor_Wifi_h 0.3

#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiAP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

#define WIFI_OPTIONS_MUTIPLE 0
#define WIFI_OPTIONS_SINGLE  1

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_Wifi : public _Sensor_BaseClass {
public:
  // ***********************************************************************
  // ***********************************************************************
  _Sensor_Wifi(String Dummy) {
    this->Default_Settings();
    this->Constructor_Finish(WIFI_OPTIONS_SINGLE);
  }
  _Sensor_Wifi(int Options = WIFI_OPTIONS_MUTIPLE) {
    _Wifi_Name = __SECRET_Wifi_Name;
    _Wifi_PWD = __SECRET_Wifi_PWD;
    this->Constructor_Finish(Options);
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish(int Options) {
    Version_Name = "V" + String(Sensor_Wifi_h) + "  ------  Sensor_Wifi.h";
    Serial.println("\n------  CREATE  ------  " + Version_Name);
    this->_Wifi_Options = Options;

#ifdef __SECRET_Broker_IP
    if (MQTT_Broker_IP.length() == 0) {
      MQTT_Broker_IP = __SECRET_Broker_IP;
    }
#endif
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {

    _Mac = String(WiFi.macAddress());
#ifdef ESP32
    _Chip_ID = "ESP32-" + _Mac.substring(9, 11) + _Mac.substring(12, 14) + _Mac.substring(15);
#else
    _Chip_ID = "esp8266-" + _Mac.substring(9, 11) + _Mac.substring(12, 14) + _Mac.substring(15);
#endif

#ifdef _Main_Name
    _AP_ssid = String(_Main_Name);
#else
    _AP_ssid = "ESP_" + _Mac.substring(9, 11) + _Mac.substring(12, 14) + _Mac.substring(15);
#endif

    Serial.print("Chip-ID = " + _Chip_ID);
    Serial.print("   MAC = " + _Mac);
    Serial.println("   AP-SSID = " + _AP_ssid);

    if (this->_Wifi_Options == WIFI_OPTIONS_MUTIPLE) this->_Setup_Multi_Wifi();
    else
      this->_Setup_Single_Wifi();
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
#ifdef __SECRET_Wifi_Name
    _Wifi_Name = Settings.Get_Set_Default_String("Wifi-Netwerk", __SECRET_Wifi_Name, Force);
#else
    _Wifi_Name = Settings.Get_Set_Default_String("Wifi-Netwerk", "lokaal Netwerk", Force);
#endif

#ifdef __SECRET_Wifi_PWD
    _Wifi_PWD = Settings.Get_Set_Default_String("$Wifi-Password", __SECRET_Wifi_PWD, Force);
#else
    _Wifi_PWD = Settings.Get_Set_Default_String("$Wifi-Password", "password", Force);
#endif
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    String New_Value;
    String Wifi_Name = Settings.Read_String("Wifi-Netwerk");
    String Wifi_PWD = Settings.Read_String("$Wifi-Password");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value = My_Webserver.arg(i);

      if (My_Webserver.argName(i) == "Wifi-Netwerk") {
        if (New_Value != Wifi_Name) {
          _My_Settings_Buffer["Wifi-Netwerk"] = New_Value;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "$Wifi-Password") {
        if ((New_Value != "********") && (New_Value != Wifi_PWD) && (New_Value.length() >= 8)) {
          _My_Settings_Buffer["$Wifi-Password"] = New_Value;
          Restart = true;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void _Setup_User_Config() {}

  // ***********************************************************************
  // ***********************************************************************
  void _Setup_Single_Wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(_Wifi_Name.c_str(), _Wifi_PWD.c_str());

    Serial.println("..... Trying to connect to " + String(_Wifi_Name));
    int Max_Count = 20;
    bool LED = LOW;
    while ((WiFi.status() != WL_CONNECTED) && (Max_Count > 0)) {
      delay(500);
      Serial.print(".");
      Max_Count -= 1;

      // is not yet in setup loop, so manually
      if (Signal_LED > 0) {
        LED = !LED;
        pinMode(Signal_LED, OUTPUT);
        digitalWrite(Signal_LED, LED);
      }
    }
    Serial.print(F("\nIP = "));
    Serial.println(WiFi.localIP());

    if (Signal_LED > 0) {
      digitalWrite(Signal_LED, LOW);
    }

// ******************************************************
// Also set up your own AccessPoint, if not connected to the set network
// ******************************************************
#ifdef ESP32
    WiFi.setHostname(_AP_ssid.c_str());
#else
    WiFi.hostname(_AP_ssid);
#endif
    if (Max_Count <= 0) {
      this->_Create_SoftAP();
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void _Setup_Multi_Wifi() {
    int Wait_Count = 0;
// *****************************************************************
// Sometimes communication is better with higher TX power and G-Mode
// *****************************************************************
#ifndef ESP32
  #ifndef WIFI_TX_POWER
    #define WIFI_TX_POWER 82
  #endif
  #ifdef WIFI_TX_POWER
    system_phy_set_max_tpw(WIFI_TX_POWER); // set TX power [ 0..82 ], 0.25 dBm per step
    Serial.print(F("WIFI-Power = "));
    Serial.println(WIFI_TX_POWER);
  #endif

  #ifdef WIFI_MODE_BGN
    wifi_set_phy_mode(WIFI_MODE_BGN); // B(werkt hier niet), G, N
    Serial.print(F("WIFI-Mode  = "));
    Serial.println(WIFI_MODE_BGN);
  #else
    #define WIFI_MODE_BGN PHY_MODE_11N
    wifi_set_phy_mode(WIFI_MODE_BGN); // B(werkt hier niet), G, N
    Serial.print(F("WIFI-Mode  = "));
    Serial.println(WIFI_MODE_BGN);
  #endif
#endif

    // *****************************
    // Absolutely necessary !!!!!!
    // to prevent:   failed rc=-2
    // *****************************
    WiFi.mode(WIFI_STA);
    // *****************************

#ifdef ESP32
    WiFi.begin(_Wifi_Name.c_str(), _Wifi_PWD.c_str());
    while (WiFi.status() != WL_CONNECTED) {
#else
    while (wifiMulti.run() != WL_CONNECTED) {
#endif
      Serial.print(".");
      delay(500);
      External_Watchdog_Toggle();

      Wait_Count += 1;
      if (Wait_Count > 10) {
        if (Allow_Wifi_Not_Found) {
          break;
        }
        ESP.restart();
      }
    }

    Serial.print("IP = ");
    Serial.println(WiFi.localIP());
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (this->_Wifi_Options == WIFI_OPTIONS_MUTIPLE) this->_Loop_Multi_Wifi();
    else
      this->_Loop_Single_Wifi();
  }
  // ***********************************************************************
  // ***********************************************************************
  void _Loop_Single_Wifi() {}
  // ***********************************************************************
  // ***********************************************************************
  void _Loop_Multi_Wifi() {

#ifdef ESP32
#else
    if (wifiMulti.run() == WL_CONNECTED) {
      _MultiWifi_Retries = 0;
    } else {
      if (Allow_Wifi_Not_Found) {
        return;
      }

      if (_MultiWifi_Retries == 0) {
        _MultiWifi_Start = millis();
      }
      _MultiWifi_Retries += 1;
      if ((millis() - _MultiWifi_Start) > 5000) {
        Serial.println();
        Serial.println(F("ESP will be RESET by MultiWifi_Loop"));
        delay(2000); // will cause a hardware timer reset
        ESP.restart();
      }
    }
#endif
  }

// ***********************************************************************
// ***********************************************************************
#ifndef ESP32
  void AddAccessPoint(char *SSID, char *PWD) {
    Serial.println("Wifi Access : " + String(SSID));
    wifiMulti.addAP(SSID, PWD);
  }
#endif

  // ***********************************************************************************
  // ***********************************************************************************
private:
  int _MultiWifi_Retries = 0;
  unsigned long _MultiWifi_Start;
  int _Wifi_Options;
  String _Mac;
  String _Chip_ID;
  String _AP_ssid;
  String _AP_pwd = "_ESP_pwd";
  String _Wifi_Name;
  String _Wifi_PWD;

  // ***********************************************************************
  // ***********************************************************************
  void _Create_SoftAP() {
    // ********************
    // Set softIP addresses
    // ********************
    Serial.println(F("..... Setting soft-AP configuration ..... "));
    IPAddress local_IP(192, 168, 6, 84);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_AP_ssid.c_str());

    // *****************************************************
    // in fact you should wait for for SYSTEM_EVENT_AP_START
    // *****************************************************
    delay(100);
    WiFi.softAPConfig(local_IP, local_IP, subnet);
    Serial.println("AP = " + _AP_ssid + "   Password = " + _AP_pwd.substring(1) + "   IP = " + local_IP.toString());
    Serial.println(WiFi.softAPIP().toString());
    Serial.println();
  }
};
#endif
