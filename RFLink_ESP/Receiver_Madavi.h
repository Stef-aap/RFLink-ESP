
// Version 0.1

#ifndef Receiver_Madavi_h
#define Receiver_Madavi_h 0.1

#include "LuftDaten.h"
#include "Receiver_Base.h"

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_Madavi : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_Madavi() { Serial.println("V" + String(Receiver_Madavi_h) + "   Receiver_Madavi_h"); }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) {
    bool Result = _Send_MadaviData();
    return Result;
  }
};

#endif
