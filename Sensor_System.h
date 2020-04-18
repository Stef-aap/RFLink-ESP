// Version 0.2, 23-02-2020, SM
//   - Free Heap added
// Version 0.1, 16-06-2019, SM
//   - initial version

#ifndef Sensor_System_h
#define Sensor_System_h    0.2

//#include "FS.h"

//#ifdef ESP32
//  #include  "SPIFFS.h"
//#endif

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_System : public _Sensor_BaseClass {

  public:
   
    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _Sensor_System () {
      Version_Name = "V" + String ( Sensor_System_h ) + "  ======  Sensor_System.h" ;
      Serial.println ( "CREATE    " + Version_Name ) ;
      
      _JSON_Long_Header  = F("RSSI\tFree_Disk\tFree_Heap\t") ;
      _JSON_Short_Header = _JSON_Long_Header ;
    }

    // **********************************************************************************************
    // **********************************************************************************************
    void Default_Settings ( bool Force = false ) {
    }

    // ***********************************************************************
    // Get all the sampled data as a JSON string
    // ***********************************************************************
    void Get_JSON_Data () {
#ifdef FileSystem_SPIFFS
      int Disk_Free ;
      #ifdef ESP32
        Disk_Free = SPIFFS.totalBytes() - SPIFFS.usedBytes() ;
      #else
        fs::FSInfo fs_info ;
        SPIFFS.info ( fs_info ) ;
        Disk_Free = fs_info.totalBytes - fs_info.usedBytes ;
      #endif

      int RSSI = WiFi.RSSI() ;
      if ( RSSI < 0 ) _RSSI = RSSI  ;
      else             RSSI = _RSSI ;
      
      int Heap_Free = ESP.getFreeHeap () ;

      JSON_Data += " \"RSSI\":" ;
      JSON_Data += String ( RSSI ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Free_Disk\":" ;
      JSON_Data +=  String ( Disk_Free ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Free_Heap\":" ;
      JSON_Data +=  String ( Heap_Free ) ;
      JSON_Data += "," ;
      
      JSON_Short_Data += String ( RSSI ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( Disk_Free ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( Heap_Free ) ;
      JSON_Short_Data += "\t" ;
#endif
      } 
    
  // ***********************************************************************
  private:
  // ***********************************************************************
    int _RSSI = -80 ;
};

#endif


