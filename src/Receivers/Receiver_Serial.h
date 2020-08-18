// Version 0.6   10-04-2020, SM
//    - Serial_2_Telnet flag removed, because simultanuous Seria; and Telnet is possible
//
// Version 0.5   09-03-2019, SM
//    - print _Main_Version removed
//
// Version 0.4   02-01-2019, SM
//    - External_Watchdog_Disarm added
//
// Version 0.3   01-12-2018, SM
//    - Swap added to Serial_Setup
//
// Version 0.2   10-11-2018, SM
//    - Wordt niet geactiveerd als    "#define Serial_Leave_Off"
//
// Version 0.1

#ifndef Receiver_Serial_h
#define Receiver_Serial_h 0.6

#include "Receiver_Base.h"

bool Serial_Swapped = false;

// werkt niet hier
//#define Serial_Device Serial

// ***********************************************************************************
// ***********************************************************************************
int Serial_Setup(int Baudrate, bool Swap = false) {

#ifndef Serial_Leave_Off
  // Open serial communications and wait for port to open:
  if (Baudrate <= 0) Baudrate = 115200;
  //#ifndef Serial_2_Telnet
  Serial.begin(Baudrate);
  // while ( !Serial ) {
  //  ; // wait for serial port to connect. Needed for Leonardo only
  //}
  //#endif
  //    if ( Swap ) {
  //      Serial.swap();
  //      Serial_Swapped = true ;
  //    }
  Serial.println("\nCREATE    V" + String(Receiver_Serial_h) + "   Receiver_Serial_h");
#endif
  return Baudrate;
}

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_Serial : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_Serial() {
    Version_Name = "V" + String(Receiver_Serial_h) + "  ======  Receiver_Serial.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
#ifndef Serial_Leave_Off
    Serial.println("\nReceiver_Serial (... )");
#endif
  }

  // **********************************************************************************************
  // **********************************************************************************************
  virtual void Default_Settings(bool Force = false) {}

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Send_Data(String JSON_Message) {
#ifndef Serial_Leave_Off
    // Serial.println ( "=========  _RECEIVER_SERIAL:  ===========" ) ;
    Serial.println("RS: " + JSON_Message);
    // Serial.println ( "-----------------------------------------" ) ;
#endif
    return true;
  }
};

#endif
