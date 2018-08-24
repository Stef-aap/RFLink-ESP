
// Version 0.1

#ifndef RFL_Protocols_h
#define RFL_Protocols_h   0.1

#include <vector>


// ***********************************************************************************
// ***********************************************************************************
unsigned long HexString_2_Long ( String HexString ) {
  return ( strtoul ( HexString.c_str(), NULL, HEX ) ) ; 
//  unsigned long Value = 0 ;
//  int           Nibble ;
//  for ( int i = 0; i < HexString.length(); i++ ) {
//    Nibble = int ( HexString.charAt(i) ) ;
//    if ( Nibble >= 48 && Nibble <= 57  ) Nibble = map ( Nibble, 48, 57,  0,  9  ) ;
//    if ( Nibble >= 65 && Nibble <= 70  ) Nibble = map ( Nibble, 65, 70,  10, 15 ) ;
//    if ( Nibble >= 97 && Nibble <= 102 ) Nibble = map ( Nibble, 97, 102, 10, 15 ) ;
//    Nibble = constrain ( Nibble, 0, 15 );
//    Value = ( Value * 16 ) + Nibble ;
//  }
//  return Value;
} 

// ****************************************************************************
// Here all available Protocols are included
//   and the base class, from which all Protocols should be derived.
// ****************************************************************************
#include "Protocols/RFL_Protocol_Base.h"
#include "Protocols/RFL_Protocol_Start.h"

#include "Protocols/RFL_Protocol_EV1527.h"
#include "Protocols/RFL_Protocol_KAKU.h"
#include "Protocols/RFL_Protocol_Paget_Door_Chime.h"


// *************************************************************************
// *************************************************************************
class _RFL_Protocols {
  public :  

    // ***********************************************************************
    // ***********************************************************************
    _RFL_Protocols (){
      // *************************************** 
      // Add the first (Pre-Processing) protocol
      // *************************************** 
      Add ( new _RFL_Protocol_Start () ) ;  
   }

    // ***********************************************************************
    // ***********************************************************************
    void Add ( _RFL_Protocol_BaseClass* RFL_Protocol ) {
      // *************************************** 
      // append the protocol to the protocol list
      // *************************************** 
      _RFL_Protocol_List.push_back ( RFL_Protocol ) ;

      // *************************************** 
      // Set an unique ID for each protocol
      // *************************************** 
      RFL_Protocol->ID = _RFL_Protocol_List.size() ;
    }

    // ***********************************************************************
    // ***********************************************************************
    void setup () {
      // *************************************** 
      // Add the last ( Cleaning Up)  Protocol
      // *************************************** 
  	  //Add ( new _RFL_Protocol_Finish () ) ;  
		
      // *************************************** 
      // Run setup for each protocol
      // *************************************** 
      Serial.println () ;
      for ( auto RFL_Protocol:_RFL_Protocol_List ){
        RFL_Protocol->setup () ;      
      }
    }

    // ***********************************************************************
    // ***********************************************************************
    void loop () {
      //for ( auto RFL_Protocol:_RFL_Protocol_List ){
      //  RFL_Protocol->loop () ;   
      //}
    }

    // ***********************************************************************
    // ***********************************************************************
    void Print () {
      Serial.println ( "=======================  My_Numbers  =======================" ) ;
      for ( auto RFL_Protocol:_RFL_Protocol_List ){
        Serial.println ( "Something Else" );
      }
    }

    // ***********************************************************************
    // ***********************************************************************
    bool Home_Command ( String CommandLine ) {
      int x1 ;
      int x2 ;
      int OnOff ;

      x1 = CommandLine.indexOf ( ";" ) + 1 ;
      x2 = CommandLine.indexOf ( ";", x1 ) ;
      String Device = CommandLine.substring ( x1, x2 ) ;

      x1 = x2 +1 ;
      x2  = CommandLine.indexOf ( ";", x1 ) ;
      String ID_String = CommandLine.substring ( x1, x2 ) ;
      unsigned long ID = HexString_2_Long ( ID_String ) ;

      x1 = x2 +1 ;
      x2  = CommandLine.indexOf ( ";", x1 ) ;
      String Switch_String = CommandLine.substring ( x1, x2 ) ;
      int Switch = Switch_String.toInt () ;
      Switch = (int) HexString_2_Long ( Switch_String ) ;

      // *********************************************
      // geen probleem als de ; op het einde ontbreekt
      // *********************************************
      x1 = x2 +1 ;
      x2  = CommandLine.indexOf ( ";", x1 ) ;
      String On_Off = CommandLine.substring ( x1, x2 ) ;

      /*
      Serial.println ( "CommandLine = " + CommandLine ) ;
      Serial.println ( "Device      = " + Device ) ;
      Serial.print   ( "ID          = 0x" ) ;
      Serial.println ( ID, HEX ) ;
      Serial.print   ( "Switch      = " ) ;
      Serial.println ( Switch ) ;
      Serial.println ( "Action=" + On_Off ) ;
      //*/      
      
      for ( auto RFL_Protocol:_RFL_Protocol_List ){
        if ( RFL_Protocol->Home_Command ( Device, ID, Switch, On_Off ) ) {
          return true ;
        }
      }  
      return false ;
    }
    

    // ***********************************************************************
    // ***********************************************************************
    boolean Decode () {
      sprintf ( PreFix, "20;%02X;", PKSequenceNumber ) ; 

      // *****************************************************************
      // *****************************************************************
      if ( Learning_Mode == 0 ) {
        for ( auto RFL_Protocol:_RFL_Protocol_List ){
          if ( RFL_Protocol->Decode () ) {
            // ****************************************************
            // do some housekeeping
            // ****************************************************
            RawSignal.Number  = 0 ;
            Last_Detection_Time = millis () ;
            return true ;
          }
        }  
        return false ;
      } 
      // *****************************************************************
      // *****************************************************************
      else if ( Learning_Mode == 1 ) {
        for ( auto RFL_Protocol:_RFL_Protocol_List ){
          if ( RFL_Protocol->Decode () ) {
            // ****************************************************
            // do some housekeeping
            // ****************************************************
            RawSignal.Number  = 0 ;
            Last_Detection_Time = millis () ;
            return true ;
          }
        }  
        return false ;
      } 
      // *****************************************************************
      // *****************************************************************
      else if ( ( Learning_Mode == 2 ) || ( Learning_Mode == 3 ) ){
        int Found = 0 ;
        int S_Len = RawSignal.Number - 3 ;
        for ( auto RFL_Protocol:_RFL_Protocol_List ){
          if ( RFL_Protocol->Decode () ) {
            if ( RFL_Protocol->Name != "Start" ) {
            }
            if ( ( RFL_Protocol->Name != "Start" ) && ( RFL_Protocol->Name != "Finish" ) ) {
              Serial.print   ( "LM="              ) ;
              Serial.print   ( Learning_Mode      ) ;      
              Serial.print   ( "    Protocol="    ) ;
              Serial.print   ( RFL_Protocol->Name ) ;      
              Serial.print   ( "    Len="         ) ;
              Serial.print   ( S_Len              ) ;
              
              Serial.print   ( "    Min="         ) ;
              Serial.print   ( RawSignal.Min      ) ;
              Serial.print   ( "   Max="          ) ;
              Serial.print   ( RawSignal.Max      ) ;
              Serial.print   ( "   Mean="         ) ;
              Serial.println ( RawSignal.Mean     ) ;

              Found += 1 ;
            }
            if ( Learning_Mode == 2 ) {
              return true;
            }
            RawSignal.Number = S_Len ;  // is cleared after a valid detection
          }
        }  
        if ( Found > 0 ) {
          Serial.print   ( "------------ FOUND = ") ;
          Serial.println ( Found ) ;
          return true ;
        }
        return false ;
      } 
      
      // *****************************************************************
      // *****************************************************************
      else {
       
        for ( auto RFL_Protocol:_RFL_Protocol_List ){
          if ( RFL_Protocol->Decode () ) {
            RawSignal.Number  = 0 ;
            return true ;
          }
        }  
        return false ;
      }  

    }

    // ***********************************************************************
    // ***********************************************************************
    private :
      std::vector <_RFL_Protocol_BaseClass*> _RFL_Protocol_List ;
};

// ********************************************************************************
// Create Sensor and Protocol list
// ********************************************************************************
_RFL_Protocols RFL_Protocols ;

#endif

