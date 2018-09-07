
// Version 0.1

#ifndef RFLink_File_h
#define RFLink_File_h   0.1

//#include "FS.h"
//#include "SPIFFS.h"
#include "FS_support.h"


// ***********************************************************************************
// typical usage:
//
//String Filename = "/RFLink.txt" ;
//_RFLink_File  RFLink_File ;
//
//void setup(){
//  Serial.begin ( 57600 );
//  delay ( 500 ) ;
//
//  RFLink_File.Begin () ;
//  //RFLink_File.Clear () ;
//  Serial.println ( "1:" + RFLink_File.Known_Devices ) ;
//  RFLink_File.Add ( "Device;ID=55AAP;" ) ;
//  Serial.println ( "2:" + RFLink_File.Known_Devices ) ;
// ***********************************************************************************
class _RFLink_File : public _FS_class {

  public:
    String Known_Devices = "" ;

    // **************************************************  
    // **************************************************    
    _RFLink_File ( String Filename = "/RFLink.txt" ) {
      _Filename = Filename ;
    }
  
    // **************************************************    
    // **************************************************    
    bool Begin () {
      _FS_class::Begin () ;
      _Read_Device_File () ;
    }
    
    // **************************************************    
    // **************************************************    
    bool Add_Device ( String Line ) {
      if ( Known_Devices.indexOf ( Line ) < 0 ) {
        Known_Devices += Line ;
        _Write_Device_File ( Known_Devices ) ;
      }
    }

    // **************************************************    
    // **************************************************    
    bool Remove_Device ( String Line ) {
      int pos = Known_Devices.indexOf ( Line ) ;
      if ( pos >= 0 ) {
        Known_Devices = Known_Devices.substring ( 0, pos ) + Known_Devices.substring ( pos + Line.length() ) ;
        _Write_Device_File ( Known_Devices ) ;
      }
    }

    // **************************************************    
    // **************************************************    
    bool Clear_Devices () {
      SPIFFS.remove ( _Filename );
      Known_Devices = "" ;
    }

    // **************************************************    
    // **************************************************    
    void Print_Devices () {
      Serial.println ( "=====  Known Devices =====" ) ;
      int x0 = 0 ;
      while ( x0 < Known_Devices.length() )  {
        int x1 = Known_Devices.indexOf ( ";", x0+1 ) ; 
        int x2 = Known_Devices.indexOf ( ";", x1+1 ) ; 
        if (( x2 < 0 ) || ( x2 < 0 )) return ;
        Serial.println ( "    " + Known_Devices.substring ( x0, x2+1 ) ); 
        x0 = x2+1 ;
      }  
    }

  // **************************************************    
  // **************************************************    
  private:
    String _Filename ;  

    // **************************************************    
    // **************************************************    
    bool _Write_Device_File ( String Line ) {
//      File file = SPIFFS.open ( _Filename, FILE_WRITE ) ;
      File file = SPIFFS.open ( _Filename, "w" ) ;
      file.print ( Line ) ;
      file.close () ;
      return true;
    }

    // **************************************************    
    // **************************************************    
    void _Read_Device_File () {
//      File file = SPIFFS.open ( _Filename, FILE_READ ) ;
      File file = SPIFFS.open ( _Filename, "r" ) ;
      Known_Devices = "" ;
      if ( file ) {
        while ( file.available() ){
          Known_Devices += file.readString() ;
        }
        file.close () ;
      }
    }

};

#endif
