// ****************************************************************************
//   ESP8266: CPU Frequency 160 MHz, FS 1 MB
//   ESP32  : CPU Frequency 240 MHz, FS 1 MB
// ****************************************************************************
// Version 3.2, 2020-04-23, SM  (RFLink_ESP_03)
//   - added Email on (re-)start (at the moment this only works for ESP32)
//        by a call to Restart_Email also a restart email for the ESP8266 can be send
//   - added warning MQTT message on (re-)start
//   - added username/password to MQTT connection (may be left empty)
//
// Version 3.1, 2020-04-18, SM
//   - program can be completely build and controlled by WebSettings
//   - build flags extended, so the program can be used with a minimal set of libraries
//
// Version 3.0, 2020-04-06, SM
//   - complete rebuild on the base of Sensors/Receivers-concept (more features with less effort)
//   - simultaneous support of serial and telnet debug and control
//   - OTA suport
//   - Webserver for settings and download of files
//   - received and transmitted messages are stored in a file
//   - flag Home_Automation removed
//
//   - command added:  19;PRINT;    //lists the known devices
//   - Learning_Mode=9 is made asynchronous, so you can do other things including stopping LM-9
//   - command added:  13; and 14;   // shows/sets 2 commands for Learning_Mode 8/9
//

// ****************************************************************************
#define _Main_Name "RFLink_ESP"
#define _Main_Version 3.2
#include <Arduino.h>

// *************************************************************************
// Common build parameters
// *************************************************************************

//#define INCLUDE_SENSOR_ADS1115
//#define INCLUDE_SENSOR_ADS1115_DIFF
//#define INCLUDE_SENSOR_BME280
//#define INCLUDE_SENSOR_DHT22
//#define INCLUDE_SENSOR_DS18B20
//#define INCLUDE_SENSOR_MHZ14
//#define INCLUDE_SENSOR_MPU9250
//#define INCLUDE_SENSOR_PIR
//#define INCLUDE_SENSOR_SDS011
#define INCLUDE_RECEIVER_MQTT
#define INCLUDE_RECEIVER_EMAIL
//#define INCLUDE_RECEIVER_SCRATCHPAD
//#define INCLUDE_RECEIVER_SDFAT
//#define INCLUDE_RECEIVER_SSD1306
//#define INCLUDE_RECEIVER_TM1638

// *************************************************************************
// Special case build parameters
// *************************************************************************

//#define INCLUDE_FTPSERVER
//#define INCLUDE_BLAUWE_ENGEL
//#define INCLUDE_FIJNSTOF_CONDITIONERING
#define INCLUDE_SENSOR_RFLINK
//#define INCLUDE_SENSOR_BMP280
//#define INCLUDE_SENSOR_MLX90614
//#define INCLUDE_SENSOR_MLX90640
//#define INCLUDE_SENSOR_MQTTBroker
//#define INCLUDE_SENSOR_NTP
//#define INCLUDE_SENSOR_OKE4
//#define INCLUDE_SENSOR_RTC
#define INCLUDE_RECEIVER_OTA
#define INCLUDE_RECEIVER_TELNET
#define INCLUDE_RECEIVER_WEBSERVER
//#define INCLUDE_RECEIVER_LUFTDATEN

// ****************************************************************************
// Include secrets
// ****************************************************************************
#include "../secrets.h"

// ****************************************************************************
// Include all libraries & Global Variables
// ****************************************************************************
#include "Sensor_Receiver.h"

// ****************************************************************************
// Here we can choose if the program is build by WebSettings or
//   by the code below this block
// If the program is build by WebSettings, this block is the end of the program,
//    in fact any program !!, and all programs are identical (except for build flags above)
// ****************************************************************************
//#define Use_Web_Settings_Version
#ifdef Use_Web_Settings_Version
void setup() { Settings_Setup(); }
void loop() { Settings_Loop(); }
#else
  // ****************************************************************************
  // ****************************************************************************

  // ***********************************************************************************
  // Hardware PINs
  // ***********************************************************************************
  #ifdef ESP32
    #define TRANSMIT_PIN 14 // 5    // Data to the 433Mhz transmitter on this pin
    #define RECEIVE_PIN 12  // 19    // On this input, the 433Mhz-RF signal is received. LOW when no signal.
    #define WATCHDOG_PIN 23 // Pin that toggles to feed an external watchdog
  // reset pin = EN  // 10k to 3V3, 1uF to GND, LOW=reset
  #else
    #define TRANSMIT_PIN 5  // Data to the 433Mhz transmitter on this pin
    #define RECEIVE_PIN 12  // On this input, the 433Mhz-RF signal is received. LOW when no signal.
    #define WATCHDOG_PIN 23 // Pin that toggles to feed an external watchdog
  //#define RESET_PIN       9    // 10k to 3V3, 1uF to GND, LOW=reset
  #endif

// ***********************************************************************************
// ***********************************************************************************
void setup() {
  MQTT_ID = _Main_Name;
  MQTT_Topic = "hb/from_HA/" + MQTT_ID; // Topic for sending and receiving general information

  // *******************************************************
  // For the ESP8266 this is the only moment to send an email
  // Values are defined in secrets.h
  // *******************************************************
  Restart_Email(__SECRET_SMTP_MailTo, "RFLink Restarted", "Body");

  // *******************************************************
  // Create all Sensors and Receivers
  // *******************************************************
  _Sensor_Wifi *wifiSensor = new _Sensor_Wifi();

  /*
  wifiSensor.AddAccessPoint(__SECRET_Wifi_Name, __SECRET_Wifi_PWD);
  */

  Sensors.Add(wifiSensor);
  Sensors.Add(new _Sensor_Watchdog(WATCHDOG_PIN)); // An external watchdog might be connected
  Sensors.Add(new _Sensor_RFLink(RECEIVE_PIN, TRANSMIT_PIN));
  Sensors.Add(new _Sensor_System());

  Receivers.Add(new _Receiver_MQTT(MQTT_Topic, __SECRET_Broker_User, __SECRET_Broker_Pass));
  Receivers.Add(new _Receiver_Serial());
  Receivers.Add(new _Receiver_SPIFFS("/RFLink.csv", 24 * 60 * 60, 4));
  Receivers.Add(new _Receiver_Telnet());
  Receivers.Add(new _Receiver_Webserver());
  Receivers.Add(new _Receiver_OTA());
  Receivers.Add(new _Receiver_Email());

  // *******************************************************
  // Initialize all Sensors and Receivers
  // *******************************************************
  Sensors.setup();
  Receivers.setup();
}

// ***********************************************************************************
// ***********************************************************************************
void loop() {
  Sensors.loop();
  Receivers.loop();
}
#endif
