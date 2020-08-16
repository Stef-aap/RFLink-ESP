// ****************************************************************************
// Minimal program to get a FTP server working for uploading files
// ****************************************************************************
// Version 0.1, 14-08-2019, SM, checked by ..
//    - initial version
// ****************************************************************************
#define _Main_Name "Settings Minimal"
#define _Main_Version 0.1

// *************************************************************************
// build parameters to reduce RAM size
// *************************************************************************
#define YES_INCLUDE_FTPSERVER
#define NOT_INCLUDE_RECEIVER_MQTT
#define NOT_INCLUDE_RECEIVER_SDFAT
#define NOT_INCLUDE_RECEIVER_SSD1306
#define NOT_INCLUDE_RECEIVER_TM1638

#include "Sensor_Receiver_2.h"

// ****************************************************************************
void setup() {
  Serial_Setup(115200);

  // ////////////////////////////
  // SPIFFS.format();
  // ////////////////////////////

  Sensors.Add(new _Sensor_Wifi("Dummy"));
  Receivers.Add(new _Receiver_SPIFFS());

  Sensors.setup();
  Receivers.setup();
}
// ****************************************************************************
void loop() {
  Sensors.loop();
  Receivers.loop();
}
