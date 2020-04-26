// ****************************************************************************
//   ESP8266: CPU Frequency 160 MHz, FS 1 MB
//   ESP32  : CPU Frequency 240 MHz, FS 1 MB 
// ****************************************************************************
// Version 3.2, 2020-04-23, SM
//   - added Email on (re-)start (at the moment this only works for ESP32)
//        by a call to Restart_Email also a restart email for the ESP8266 can be send
//   - added warning MQTT message on (re-)start
//   - added username/password to MQTTconnection (may be left empty)
//
// Version 3.1, 2020-04-18, SM
//   - program can be completely build and controlled by WebSettings
//   - build flags extended, so the program can be used with a minimal set of libraries
//
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
#define _Main_Version  3.2

// *************************************************************************
// build parameters to reduce RAM size
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
//#define NOT_INCLUDE_RECEIVER_EMAIL
#define NOT_INCLUDE_RECEIVER_SCRATCHPAD
#define NOT_INCLUDE_RECEIVER_SDFAT
#define NOT_INCLUDE_RECEIVER_SSD1306
#define NOT_INCLUDE_RECEIVER_TM1638

// ****************************************************************************
// Include all libraries & Global Variables
// ****************************************************************************
#include "Sensor_Receiver_2.h"


// ****************************************************************************
// Here we can choose if the program is build by WebSettings or 
//   by the code below this block 
// If the program is build by WebSettings, this block is the end of the program,
//    in fact any program !!, and all programs are identical (except for build flags above)
// ****************************************************************************
#define Use_Web_Settings_Version
#ifdef Use_Web_Settings_Version
  void setup() { Settings_Setup () ; }
  void loop () { Settings_Loop  () ; }
#else
// ****************************************************************************
// ****************************************************************************


// ***********************************************************************************
// Hardware PINs
// ***********************************************************************************
#ifdef ESP32
  #define TRANSMIT_PIN   14  // 5    // Data to the 433Mhz transmitter on this pin
  #define RECEIVE_PIN    12  // 19    // On this input, the 433Mhz-RF signal is received. LOW when no signal.
  #define WATCHDOG_PIN   23    // Pin that toggles to feed an external watchdog
  // reset pin = EN  // 10k to 3V3, 1uF to GND, LOW=reset
#else
  #define TRANSMIT_PIN    5    // Data to the 433Mhz transmitter on this pin
  #define RECEIVE_PIN    12    // On this input, the 433Mhz-RF signal is received. LOW when no signal.
  #define WATCHDOG_PIN   23    // Pin that toggles to feed an external watchdog
  //#define RESET_PIN       9    // 10k to 3V3, 1uF to GND, LOW=reset
#endif

// ***********************************************************************************
// ***********************************************************************************
void setup() {
  MQTT_ID         = _Main_Name             ; 
  MQTT_Topic      = "hb/from_HA/"+ MQTT_ID ; // Topic for sending and receiving general information

  // *******************************************************
  // For the ESP8266 this is the only moment to send an email
  // Values are defined in Wifi_settings.h
  // *******************************************************
  Restart_Email ( __SECRET_SMTP_MailTo, "RFLink Restarted", "Body" ) ;

  // *******************************************************
  // Create all Sensors and Receivers
  // *******************************************************
  Sensors.Add ( new _Sensor_Watchdog ( WATCHDOG_PIN ) );    // An external watchdog might be connected
  Sensors.Add ( new _Sensor_Wifi     () );
  Sensors.Add ( new _Sensor_RFLink   ( RECEIVE_PIN, TRANSMIT_PIN ) );
  Sensors.Add ( new _Sensor_System   () );

  Receivers.Add ( new _Receiver_MQTT      ( MQTT_Topic ) ) ; 
  Receivers.Add ( new _Receiver_Serial    () ) ;
  Receivers.Add ( new _Receiver_SPIFFS    ( "/RFLink.csv",24*60*60, 4 ) ) ;
  Receivers.Add ( new _Receiver_Telnet    () ) ;
  Receivers.Add ( new _Receiver_Webserver () ) ;
  Receivers.Add ( new _Receiver_OTA       () ) ;
  Receivers.Add ( new _Receiver_Email     () ) ;
    
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
#endif

// ***********************************************************************************
// upload binaries:
// ***********************************************************************************
/*
python /home/stef/.arduino15/packages/esp32/tools/esptool_py/2.6.1/esptool.py 
--chip esp32 
--port /dev/ttyUSB0 
--baud 921600 
--before default_reset 
--after hard_reset write_flash -z 
--flash_mode dio 
--flash_freq 80m 
--flash_size detect 
0xe000 
/home/stef/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/partitions/boot_app0.bin 
0x1000 
/home/stef/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/bin/bootloader_qio_80m.bin 
0x10000 
/tmp/arduino_build_841764/RFLink_ESPb.ino.bin 
0x8000 
/tmp/arduino_build_841764/RFLink_ESPb.ino.partitions.bin 

====> ESP8266
/home/stef/.arduino15/packages/esp8266/tools/python3/3.7.2-post1/python3 
/home/stef/.arduino15/packages/esp8266/hardware/esp8266/2.6.3/tools/upload.py 
--chip esp8266 
--port /dev/ttyUSB1 
--baud 921600 
--before default_reset 
--after hard_reset 
write_flash 0x0 /tmp/arduino_build_130546/RFLink_ESP_02.ino.bin 

/*/
