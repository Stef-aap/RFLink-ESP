
// Version 0.3, 12-12-2019, SM
//     - OLED display support added
// Version 0.2, 29-07-2019, SM
//     - Get_JSON_Data removed from loop
// Version 0.1

#ifndef Sensor_Dummy_h
#define Sensor_Dummy_h    0.3

#include "Sensor_Base.h"

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_Dummy : public _Sensor_BaseClass {
  
  public:
    String   Device_Firmware = "deviceID Mijn eerste sensor" ;
   
    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _Sensor_Dummy ( int Display_X = -1, int Display_Y = -1 ) {
      this -> _Display_X = Display_X ;
      this -> _Display_Y = Display_Y ;
      Version_Name = "V" + String ( Sensor_Dummy_h ) + "   Sensor_Dummy.h" ;
      Serial.println ( "CREATE    " + Version_Name ) ;
      _JSON_Short_Header = "Dummy Sawtooth\tDummy_Noise\t" ;
      _JSON_Long_Header  = _JSON_Short_Header ;
    }

    // ***********************************************************************
    // ***********************************************************************
    void setup () {
      //Serial.print   ( "SETUP of _Sensor_Dummy,  ID = ") ;
      _Saw_Tooth = 0 ;   //random ( 10000 ) ;
    }


    // ***********************************************************************
    // Get all the sampled data as a JSON string
    // ***********************************************************************
    void Get_JSON_Data () {
      _Saw_Tooth += 1 ;
      _Saw_Tooth %= 99 ; 
      int Noise = random ( 0, 100 ) ;
      
      JSON_Data += " \"Dummy_Sawtooth\":" ;
      JSON_Data += String ( _Saw_Tooth ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Dummy_Noise\":" ;
      JSON_Data += String ( Noise ) ;
      JSON_Data += "," ;


      JSON_Short_Data += String ( _Saw_Tooth ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( Noise ) ;
      JSON_Short_Data += "\t" ;
      
      /*
      if ( ( this->_Display_X >= 0 ) && ( this-> _Display_Y>=0 ) ) {
        float TMuur   = 19.12 ;
        float TRuimte = 19.68 ;
        float Watt    = 34 ;
        float TDelta  = TRuimte - TMuur ;
        //_Character_Display->Show ( String ( Watt   , 1 ), 0, 0 ) ;
        //_Character_Display->Show ( String ( TDelta , 2 ), 0, 1 ) ;
        //_Character_Display->Show ( String ( TMuur  , 1 ), 0, 2 ) ;
        //_Character_Display->Show ( String ( TRuimte, 1 ), 0, 3 ) ;
        
        //_Character_Display->Show ( "2.8 " , 6, 0 ) ;
        //_Character_Display->Show ( "R"    , 9, 0 ) ;
        //_Character_Display->Show ( "IN  " , 6, 1 ) ;   //  In  / UIT
        //_Character_Display->Show ( "0.13", 6, 2 ) ;   // 0.04 / 0.10 / 0.13
        //_Character_Display->Show ( "Vloe", 6, 3 ) ;   // Vloe / Wand / Top
      }
      */
      
   }

    
    
  // ***********************************************************************
  private:
  // ***********************************************************************
    unsigned int  _Saw_Tooth        = 0 ;
    //unsigned long Sample_Time_Last  ;
    int           _Display_X ;
    int           _Display_Y ;

};

#endif
