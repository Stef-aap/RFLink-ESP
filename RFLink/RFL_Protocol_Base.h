
#ifndef RFL_Protocol_Base_h
#define RFL_Protocol_Base_h


char      _RFLink_pbuffer  [ 60 ] ;           // Buffer for printing data
char      _RFLink_pbuffer2 [ 30 ] ;

String    _RFLink_MQTT_Topic_Send ;

// ***********************************************************************************
// ***********************************************************************************
String Randomize_Device_ID ( String Device_ID ) {
  if ( Device_ID.length() == 0 ) return "" ;
  int x1 = Device_ID.indexOf ( ";" ) ;
  String New = Device_ID.substring ( x1+4 ) ;
  New = New.substring ( 0, New.length()-1 ) ;

  unsigned long x = HexString_2_Long ( New ) ;
  x = x * 48271 % 2147483647 ;
  // ********************************************************************
  // Pasted from <http://www.cplusplus.com/reference/random/minstd_rand/>
  // ********************************************************************

  String Result = Device_ID.substring ( 0, x1+1 ) + "ID=" + String ( x, HEX ) + ";" ;
  Result.toUpperCase () ;
  return Result ;            
}

// ***********************************************************************************
// ***********************************************************************************
bool Unknown_Device ( String Device ) {
  int pos = RFLink_File.Known_Devices.indexOf ( Device ) ;
  // *******************************************************************
  // If there's a mius sign before the Device name,
  // the received RF ID should be changed by the pseudo random generator
  // *******************************************************************
  if ( ( pos > 0 ) && ( RFLink_File.Known_Devices.charAt(pos-1) == '-' ) ) {
    String New = Randomize_Device_ID ( Device ) ;
    sprintf ( _RFLink_pbuffer, New.c_str() ) ;
  }  

  if ( pos  < 0 )  {
    if ( Learning_Mode == 0 ) return true ;
    else {
      if ( strcasecmp ( _RFLink_pbuffer, Unknown_Device_ID.c_str() ) != 0 ) {
/*
        Serial.print   ( "Unknown Device: 12;" ) ;
        Serial.print   ( _RFLink_pbuffer ) ;
        Serial.print   ( "   ") ;
        Serial.println ( millis() );
        Unknown_Device_ID = String ( _RFLink_pbuffer ) ;
*/        
Unknown_Device_ID = String ( _RFLink_pbuffer ) ;
Line_2_File = "Unknown Device: 12;" + Unknown_Device_ID  + "   " ;
Line_2_File += String ( millis() );
RFLink_File.Log_Line ( Line_2_File ) ;
      }
      return true ;
    }
  }
  return false ;
}


// ***********************************************************************************
/*
20;16;EV1527;ID=005DF;SWITCH=01;CMD=ON;
20;03;EV1527;ID=005DF;SWITCH=2;CMD=ON;    << switch is iets anders geworden, maar lijkt me geen probleem

20;17;NewKaku;ID=2508A7C;SWITCH=B;CMD=OFF;

20;18;PT2262;ID=755D5;SWITCH=01;CMD=ON;
20;0D;PT2262;ID=755D5;SWITCH=1;CMD=ON;    << switch is iets anders geworden, maar lijkt me geen probleem

20;19;Door_Chime;ID=AAAA;SWITCH=1;CMD=ON;CHIME=02;
20;02;Door_Chime;ID=AAAA;SWITCH=1;CMD=ON;CHIME=02; 

Name      ID      Switch   CMD     extra
*/
// ***********************************************************************************
bool Send_Message ( String Name, unsigned long Id, unsigned long Switch, String On_Off, String Extra="" ) {
  sprintf ( _RFLink_pbuffer, "%s;ID=%0X;", Name.c_str(), Id ) ; 
  if ( Unknown_Device ( _RFLink_pbuffer ) ) return false ;

  // **********************************************************  
  // Send an MQTT Message
  //      ha/from_HA/ev1527_005df     S02
  // **********************************************************  
//  if ( Home_Automation == "MQTT" ) {
    //String Topic = "ha/from_RFLink/" + Name + "_" ;
//String Topic = MQTT_Topic_Send + Name + "_" ;
String Topic = _RFLink_MQTT_Topic_Send + Name + "_"  ;
    sprintf ( _RFLink_pbuffer, "%0X", Id ) ;
    Topic += String ( _RFLink_pbuffer ) ;

    String Payload = "S" ;
    sprintf ( _RFLink_pbuffer, "%0X_%s", Switch, On_Off.c_str() ) ;
    Payload += String ( _RFLink_pbuffer ) ;

    if ( Extra.length() > 0 ) {
      Payload += "_" + Extra ;
    }
    
    //Serial.println ( "MQTT Send     Topic: " + Topic + "   Payload: " + Payload ) ;
Serial.println ( "111" + Topic ) ;    
    My_MQTT_Client->Publish ( Topic, Payload ) ;

Line_2_File = "MQTT-Send  Topic=" + Topic + "   Payload=" +  Payload  ;
RFLink_File.Log_Line ( Line_2_File ) ;

/*
  }
  // **********************************************************  
  // Send Serial Message
  // **********************************************************  
  else {
    //sprintf ( _RFLink_pbuffer, "%s;ID=%05X;", Name.c_str(), Id ) ; 
    
    Serial.print   ( PreFix ) ;
    Serial.print   ( _RFLink_pbuffer ) ;
    sprintf ( _RFLink_pbuffer2, "SWITCH=%0X;CMD=%s;", Switch, On_Off.c_str() ) ; 
    Serial.print ( _RFLink_pbuffer2 ) ;
    if ( Extra.length() > 0 ) {
      Serial.print ( Extra + ";" ) ;
    }
    Serial.println () ;    

//Line = "MQTT-Receive  Topic=" + Topic + " Payload=" +  Payload + " - Converted: " + Line ;
Line_2_File = "MQTT-Send    " + String ( PreFix ) + String ( _RFLink_pbuffer ) + String ( _RFLink_pbuffer2 )  ;
if ( Extra.length() > 0 )  Line_2_File += Extra + ';' ;
RFLink_File.Log_Line ( Line_2_File ) ;


    PKSequenceNumber += 1 ;
  }
  /*/
  return true ;
}

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_BaseClass {
  public:
    String Name = "Unknown" ;
    String NAME ;
    int    ID   = 0         ;

    // **********************************************************************************************
    // By defining the method virtual,
    // The method in the derived class (if available) will take over the functionality
    // If no method with the same name is in the derived class, this virtual method will be effective
    //
    // if derived class overrides this method, it should call theis base:
    //   _RFL_Protocol_BaseClass::setup() ;
    // **********************************************************************************************
    virtual void setup () {
      if ( Learning_Mode > 0 ) {
        Serial.print   ( "Active Protocol " + Name ) ;
        Serial.print   ( "    ID = " ) ;
        Serial.println ( ID ) ;
      }
    }

    virtual void loop () {
      // ************************************************************
      // Not allowed to do here a print statement
      // because it's possible that the sensor doesn't need a loop !!
      // ************************************************************
    }
    
    // ***********************************************************************
    // ***********************************************************************
    virtual bool RF_Decode () {
      Serial.print   ( "DECODE of _RFL_Protocol_Class is Missing " + Name ) ;
	    return false ;
    }

    // ***********************************************************************
    // ***********************************************************************
    virtual bool Home_Command ( String Device, unsigned long ID, int Switch, String On ) {
      return false ;
    } 

    // ***********************************************************************
    // ***********************************************************************
    virtual void Print_Help () {
      Serial.println ( "Help information is missing for this library " + Name ) ;
    }

};
#endif
