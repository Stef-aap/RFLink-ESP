// ****************************************************************************
//   ESP8266: CPU Frequency 160 MHz, FS 1 MB
//   ESP32  : CPU Frequency 240 MHz, FS 1 MB 
// ****************************************************************************
// Version 3.0, 2020-04-06, SM
//   - complete rebuild on the base of Sensors/Receivers-concept (more features with less effort)
//   - simultanuous support of serial and telnet debug and control
//   - OTA suport
//   - Webserver for settings and download of files
//   - received and transmitted messages are stored in a file
//   - flag Home_Automation removed
//
//   - command added:  19;PRINT;    //lists the known devices
//   - Learning_Mode=9 is made asynchronuous, so you can do other things including stopping LM-9
//   - command added:  13; and 14;   // shows/sets 2 commands for Learning_Mode 8/9
//

// ****************************************************************************
#define _Main_Name     "RFLink_ESP"
#define _Main_Version  3.0

// ***********************************************************************************
// Hardware PINs
// ***********************************************************************************
#ifdef ESP32
  #define TRANSMIT_PIN   14  // 5    // Data to the 433Mhz transmitter on this pin
  #define RECEIVE_PIN    12  // 19    // On this input, the 433Mhz-RF signal is received. LOW when no signal.
  #define WATCHDOG_PIN   23    // Pin that toggles to feed an external watchdog
  #define RESET_PIN       9    // 10k to 3V3, 1uF to GND, LOW=reset
#else
  #define TRANSMIT_PIN    5    // Data to the 433Mhz transmitter on this pin
  #define RECEIVE_PIN    12    // On this input, the 433Mhz-RF signal is received. LOW when no signal.
  #define WATCHDOG_PIN   23    // Pin that toggles to feed an external watchdog
  #define RESET_PIN       9    // 10k to 3V3, 1uF to GND, LOW=reset
#endif

// *************************************************************************
// build paramters to reduce RAM size
// *************************************************************************
//#define YES_INCLUDE_FTPSERVER
#define YES_INCLUDE_SENSOR_RFLINK
//#define YES_INCLUDE_SENSOR_MQTTBroker
#define YES_INCLUDE_RECEIVER_TELNET
#define YES_INCLUDE_RECEIVER_OTA
#define YES_INCLUDE_RECEIVER_WEBSERVER

#define NOT_INCLUDE_SENSOR_ADS1115
#define NOT_INCLUDE_SENSOR_MHZ14
#define NOT_INCLUDE_SENSOR_SDS011
//#define NOT_INCLUDE_RECEIVER_MQTT
#define NOT_INCLUDE_RECEIVER_SCRATCHPAD
#define NOT_INCLUDE_RECEIVER_SDFAT
#define NOT_INCLUDE_RECEIVER_SSD1306
#define NOT_INCLUDE_RECEIVER_TM1638

// ****************************************************************************
// Include all libraries & Global Variables
// ****************************************************************************
#include "Sensor_Receiver_2.h"

// ***********************************************************************************
// ***********************************************************************************
void setup() {
  MQTT_ID         = _Main_Name             ; 
  MQTT_Topic      = "hb/from_HA/"+ MQTT_ID ; // Topic for sending and receiving general information
  MQTT_Topic_Send = "hb/from_RFLink/"      ; // Topic to return switch status
  MQTT_Topic_Rec  = "hb/from_HA/"          ; // Topic to receive switch commands

  // *******************************************************
  // Create all Sensors and Receivers
  // *******************************************************
  Sensors.Add ( new _Sensor_Watchdog ( WATCHDOG_PIN ) );    // An external watchdog might be connected
  Sensors.Add ( new _Sensor_Wifi     () );
  Sensors.Add ( new _Sensor_RFLink   ( RECEIVE_PIN, TRANSMIT_PIN ) );
  Sensors.Add ( new _Sensor_System   () );

  Receivers.Add ( new _Receiver_MQTT      ( MQTT_Topic ) ) ; 
  Receivers.Add ( new _Receiver_Serial    () ) ;
  Receivers.Add ( new _Receiver_Telnet    () ) ;
  Receivers.Add ( new _Receiver_Webserver () ) ;
  Receivers.Add ( new _Receiver_OTA       () ) ;
    
  // *******************************************************
  // Initialize all Sensors and Receivers
  // *******************************************************
  Sensors  .setup () ;
  Receivers.setup () ;
}

// ***********************************************************************************
// ***********************************************************************************
void loop () {
  Sensors  .loop () ;
  Receivers.loop () ;
}
