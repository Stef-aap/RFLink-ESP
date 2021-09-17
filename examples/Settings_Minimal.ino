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
#define INCLUDE_FTPSERVER

#include "Sensor_Receiver.h"

// ****************************************************************************
void setup() {
  Serial_Setup(115200);

  Sensors.Add(new _Sensor_Wifi());
  Receivers.Add(new _Receiver_SPIFFS());

  Sensors.setup();
  Receivers.setup();
}
// ****************************************************************************
void loop() {
  Sensors.loop();
  Receivers.loop();
}
