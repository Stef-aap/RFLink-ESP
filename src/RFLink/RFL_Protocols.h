// Version 0.2
//    - incoming Commands are translated to uppercase

// Version 0.1

#ifndef RFL_Protocols_h
#define RFL_Protocols_h 0.2

#include <vector>

struct RawSignalStruct { // Raw signal variabelen places in a struct
  int Number;            // Number of pulses, times two as every pulse has a mark and a space.
  int Min;
  int Max;
  long Mean;
  unsigned long Time;              // Timestamp indicating when the signal was received (millis())
  int Pulses[RAW_BUFFER_SIZE + 2]; // Table with the measured pulses in microseconds divided by RawSignal.Multiply.
                                   // (halves RAM usage) First pulse is located in element 1. Element 0 is used for
                                   // special purposes, like signalling the use of a specific plugin
} RawSignal = {0, 0, 0, 0, 0L};

unsigned long Last_BitStream = 0L; // holds the bitstream value for some plugins to identify RF repeats
unsigned long Last_Detection_Time = 0L;

// ***********************************************************************************
// ***********************************************************************************
unsigned long HexString_2_Long(String HexString) { return (strtoul(HexString.c_str(), NULL, HEX)); }

// ****************************************************************************
// Here all available Protocols are included
//   and the base class, from which all Protocols should be derived.
// ****************************************************************************
#include "RFL_Protocol_Base.h"
#include "RFL_Protocol_Start.h"

#include "RFL_Protocol_EV1527.h"
#include "RFL_Protocol_KAKU.h"
#include "RFL_Protocol_Paget_Door_Chime.h"

// *************************************************************************
// *************************************************************************
class _RFL_Protocols {
public:
  // ***********************************************************************
  // ***********************************************************************
  _RFL_Protocols() {
    // ***************************************
    // Add the first (Pre-Processing) protocol
    // ***************************************
    Add(new _RFL_Protocol_Start());
  }

  // ***********************************************************************
  // ***********************************************************************
  void Add(_RFL_Protocol_BaseClass *RFL_Protocol) {
    // ***************************************
    // append the protocol to the protocol list
    // ***************************************
    _RFL_Protocol_List.push_back(RFL_Protocol);

    // ***************************************
    // Set an unique ID for each protocol
    // ***************************************
    RFL_Protocol->ID = _RFL_Protocol_List.size();
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    // ***************************************
    // Run setup for each protocol
    // ***************************************
    Serial.println();
    for (auto RFL_Protocol : _RFL_Protocol_List) {
      RFL_Protocol->setup();
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {}

  // ***********************************************************************
  // ***********************************************************************
  void Print() {
    Serial.println("============  My_Numbers  ============");
    for (auto RFL_Protocol : _RFL_Protocol_List) {
      Serial.println("Something Else");
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Home_Command(String CommandLine) {
    int x1;
    int x2;
    int OnOff;
    CommandLine.toUpperCase();

    x1 = CommandLine.indexOf(";") + 1;
    x2 = CommandLine.indexOf(";", x1);
    String Device = CommandLine.substring(x1, x2);

    x1 = x2 + 1;
    x2 = CommandLine.indexOf(";", x1);
    String ID_String = CommandLine.substring(x1, x2);
    unsigned long ID = HexString_2_Long(ID_String);

    x1 = x2 + 1;
    x2 = CommandLine.indexOf(";", x1);
    String Switch_String = CommandLine.substring(x1, x2);
    int Switch = (int)HexString_2_Long(Switch_String);

    // *********************************************
    // no problem if the ; in the end is missing
    // *********************************************
    x1 = x2 + 1;
    x2 = CommandLine.indexOf(";", x1);
    String On_Off = CommandLine.substring(x1, x2);

    for (auto RFL_Protocol : _RFL_Protocol_List) {
      if (RFL_Protocol->Home_Command(Device, ID, Switch, On_Off)) {
        return true;
      }
    }
    return false;
  }

  // ***********************************************************************
  // ***********************************************************************
  boolean Decode() {
    sprintf(PreFix, "20;%02X;", PKSequenceNumber);

    // *****************************************************************
    // *****************************************************************
    if (Learning_Mode == 0) {
      for (auto RFL_Protocol : _RFL_Protocol_List) {
        if (RFL_Protocol->RF_Decode()) {
          // ****************************************************
          // do some housekeeping
          // ****************************************************
          RawSignal.Number = 0;
          Last_Detection_Time = millis();
          return true;
        }
      }
      return false;
    }
    // *****************************************************************
    // *****************************************************************
    else if (Learning_Mode == 1) {
      for (auto RFL_Protocol : _RFL_Protocol_List) {
        if (RFL_Protocol->RF_Decode()) {
          // ****************************************************
          // do some housekeeping
          // ****************************************************
          RawSignal.Number = 0;
          Last_Detection_Time = millis();
          return true;
        }
      }
      return false;
    }
    // *****************************************************************
    // *****************************************************************
    else if ((Learning_Mode == 2) || (Learning_Mode == 3)) {
      int Found = 0;
      int S_Len = RawSignal.Number - 3;
      for (auto RFL_Protocol : _RFL_Protocol_List) {
        if (RFL_Protocol->RF_Decode()) {
          if (RFL_Protocol->Name != "Start") {
          }
          if ((RFL_Protocol->Name != "Start") && (RFL_Protocol->Name != "Finish")) {
            Line_2_File = "LM-";
            Line_2_File += String(Learning_Mode);
            Line_2_File += " :   Protocol=";
            Line_2_File += RFL_Protocol->Name;
            Line_2_File += "    Len=";
            Line_2_File += String(S_Len);

            Line_2_File += "    Min=";
            Line_2_File += String(RawSignal.Min);
            Line_2_File += "   Max=";
            Line_2_File += String(RawSignal.Max);
            Line_2_File += "   Mean=";
            Line_2_File += String(RawSignal.Mean);
            RFLink_File.Log_Line(Line_2_File);

            Found += 1;
          }
          if (Learning_Mode == 2) {
            return true;
          }
          RawSignal.Number = S_Len; // is cleared after a valid detection
        }
      }
      if (Found > 0) {
        Serial.print("------------ FOUND = ");
        Serial.println(Found);
        return true;
      }
      return false;
    }

    // *****************************************************************
    // *****************************************************************
    else {

      for (auto RFL_Protocol : _RFL_Protocol_List) {
        if (RFL_Protocol->RF_Decode()) {
          RawSignal.Number = 0;
          return true;
        }
      }
      return false;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
private:
  std::vector<_RFL_Protocol_BaseClass *> _RFL_Protocol_List;
};

// ********************************************************************************
// Create Sensor and Protocol list
// ********************************************************************************
_RFL_Protocols RFL_Protocols;

#endif
