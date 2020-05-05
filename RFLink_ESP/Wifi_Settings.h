#ifndef Wifi_Settings_h
#define Wifi_Settings_h

// ***********************************************************************
//  PRIVATE GLOBALS
// ***********************************************************************
#define __SECRET_Wifi_Name  "Home Network SSID"
#define __SECRET_Wifi_PWD   "Password"   
#define __SECRET_Broker_IP  "192.168.22.23" 

#define __SECRET_SMTP_Server "smtp.gmail.com"
#ifdef ESP32
  #define __SECRET_SMTP_Port   587
#else
  #define __SECRET_SMTP_Port   465
#endif
#define __SECRET_SMTP_User   "someone@gmail.com"
#define __SECRET_SMTP_PWD    "someones password"
#define __SECRET_SMTP_MailTo "someone@gmail.com"

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
      _AddAccessPoint ( wifiMulti, __SECRET_Wifi_Name  , __SECRET_Wifi_PWD   );
      _AddAccessPoint ( wifiMulti, "A_Second SSID"  , "The Password"   );
  }
#endif

#endif
