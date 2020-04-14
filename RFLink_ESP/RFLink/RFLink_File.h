// Version 0.1

#ifndef RFLink_File_h
#define RFLink_File_h   0.1

//#include "FS.h"
//#include "SPIFFS.h"
#include "FS_support.h"


// ***********************************************************************************
// This class is explictly NOT derived from _FS_Class,
// because an instance of _FS_Class will always exist already and therfor can be used.
// ***********************************************************************************
//class _RFLink_File : public _FS_class {
class _RFLink_File {

  public:
    String Known_Devices = "" ;

    // **************************************************  
    // **************************************************    
    _RFLink_File ( String Filename = "/RFLink.txt" ) {
//_DEBUG_Global_String += "\r\n+ _RFLink_File.creator" ;
      _Filename = Filename ;
    }
  
    // **************************************************    
    // **************************************************    
    bool Begin () {
//_DEBUG_Global_String += "\r\n+ _RFLink_File.Begin" ;
      // for logging use max 5 files of length 100000
      File_System.Begin ( this->_Log_Filename, -100000, 5 ) ;
      //File_System.Begin ( this->_Log_Filename, -5000, 5 ) ;  // for test use smaller file
      _Read_Device_File () ;

    }
    
    // **************************************************    
    // **************************************************    
    void Log_Line ( String Line ) {
      File_System.Create_CSV_File_Nr ( this->_Log_Filename,"" ) ;
      File_System.Append_File ( this->_Log_Filename, String ( millis()/1000) + "\t" + Line + "\r\n"  ) ;
Serial.println ( "LOG: " + Line ) ;      
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
      String Line = "=====  Known Devices =====";
      this->Log_Line ( Line ) ;
      int x0 = 0 ;
      while ( x0 < Known_Devices.length() )  {
        int x1 = Known_Devices.indexOf ( ";", x0+1 ) ; 
        int x2 = Known_Devices.indexOf ( ";", x1+1 ) ; 
        if (( x2 < 0 ) || ( x2 < 0 )) return ;
        Line = "    " + Known_Devices.substring ( x0, x2+1 ) ;
        this->Log_Line ( Line ) ;
        x0 = x2+1 ;
      }  
    }

  // **************************************************    
  // **************************************************    
  private:
    String _Filename ;  
    String _Log_Filename = "/RFLog.txt" ;

    // **************************************************    
    // **************************************************    
    bool _Write_Device_File ( String Line ) {
//      File file = SPIFFS.open ( _Filename, FILE_WRITE ) ;
      fs::File file = SPIFFS.open ( _Filename, "w" ) ;
      file.print ( Line ) ;
      file.close () ;
      return true;
    }

    // **************************************************    
    // **************************************************    
    void _Read_Device_File () {
//      File file = SPIFFS.open ( _Filename, FILE_READ ) ;
      fs::File file = SPIFFS.open ( _Filename, "r" ) ;
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
