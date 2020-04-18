#ifndef Wifi_Settings_h
#define Wifi_Settings_h

// ***********************************************************************
//  PRIVATE GLOBALS
// ***********************************************************************
#define Local_Wifi_Name  "Home Network SSID"
#define Local_Wifi_PWD   "Password"   
#define Local_Broker_IP  "192.168.22.23" 

// ***********************************************************************
//  Multi_WiFi support, here you can add more network SSID
// ***********************************************************************
#ifndef ESP32
  #include <ESP8266WiFiMulti.h>
  void _AddAccessPoint ( ESP8266WiFiMulti *wifiMulti , char* SSID, char* PWD ) {
    Serial.println ( "Wifi Access : " + String ( SSID ) ) ;
    wifiMulti->addAP ( SSID, PWD ) ;
  }
  void AddAccessPoints ( ESP8266WiFiMulti *wifiMulti ) {
      _AddAccessPoint ( wifiMulti, Local_Wifi_Name  , Local_Wifi_PWD   );
      _AddAccessPoint ( wifiMulti, "A_Second SSID"  , "The Password"   );
  }
#endif

#endif
