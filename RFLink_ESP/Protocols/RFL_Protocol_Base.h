
#ifndef RFL_Protocol_Base_h
#define RFL_Protocol_Base_h


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
    sprintf ( pbuffer, New.c_str() ) ;
  }  

  if ( pos  < 0 )  {
    if ( Learning_Mode == 0 ) return true ;
    else {
      Serial.print   ( "Unknown Device: 12;" ) ;
      Serial.println ( pbuffer ) ;
      Unknown_Device_ID = pbuffer ;
    }
  }
  return false ;
}

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_BaseClass {
  public:
    String Name = "Unknown" ;
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
    virtual bool Decode () {
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
