// Version 0.1
#ifndef Receiver_Luftdaten_h
#define Receiver_Luftdaten_h    0.1

#include "Receiver_Base.h"
#include "LuftDaten.h"


// ***********************************************************************************
// ***********************************************************************************
class _Receiver_Luftdaten : public _Receiver_BaseClass {

  public:

    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _Receiver_Luftdaten ( String ID = "" ){
      Luftdaten_Setup ( ID ) ;
      Serial.println ( "V" + String ( Receiver_Luftdaten_h ) + "   Receiver_Luftdaten_h" ) ;
    }

    // ***********************************************************************
    // ***********************************************************************
    bool Send_Data ( String JSON_Message ) {
      bool Result = _Send_LuftData () ;
      return Result ;
    }
};

#endif

