
// Version 0.1

#ifndef FS_support_h
#define FS_support_h   0.1

#include "FS.h"

#ifdef ESP32
  #include  "SPIFFS.h"
#endif


// ***********************************************************************************
// typical usage:
//
//_FS_class   FileSystem () ;
//
//void setup(){
//  Serial.begin(115200);
//  delay ( 500 ) ;
//
//  FileSystem.Begin () ;
// ***********************************************************************************
class _FS_class {
  public:

    // **************************************************  
    // **************************************************    
    _FS_class () { 
    }
  
    // **************************************************    
    // **************************************************    
    bool Begin () {
      return SPIFFS.begin() ;
// MISCHEIN MOETEN WE VOOR ESP32 nog een format inbouwen ????
//      if ( !SPIFFS.begin() ) {
//        // ********************************
//        // if begin failed, format the disk
//        // ********************************
//        SPIFFS.format () ;
//      }
    }
    
    // **************************************************    
    // **************************************************    
    void DirList ( String Path = "/" ) {
      Serial.println ( "Files in SPIFFS" ) ;

      // **************************************************    
      // ESP32
      // **************************************************    
      #ifdef ESP32
        File dir = SPIFFS.open ( "/" ) ;
        File file = dir.openNextFile () ;
        while ( file ) {
          Serial.print   ( "  FILE: "  ) ;
          Serial.print   ( file.name() ) ;
          Serial.print   ( "\tSIZE: "  );
          Serial.println ( file.size() ) ;
          file = dir.openNextFile () ;
        }

        Serial.printf ( "Total=%i  Used=%i  Free=%i  MaxOpen=%i\n", 
                        SPIFFS.totalBytes(), SPIFFS.usedBytes(), SPIFFS.totalBytes() - SPIFFS.usedBytes(),
                        10 ) ;
      // **************************************************    
      // ESP8266
      // **************************************************    
      #else
        Dir dir = SPIFFS.openDir ( "/" ) ;
        while ( dir.next() ) {
          Serial.print ( dir.fileName() + "    " );
          File file = dir.openFile ( "r" ) ;
          Serial.println ( file.size() ) ;
        }

        FSInfo fs_info ;
        SPIFFS.info ( fs_info ) ;
        Serial.printf ( "Total=%i  Used=%i  Free=%i  MaxOpen=%i  MaxPathLength=%i  BlockSize=%i  PageSize=%i\n", 
                        fs_info.totalBytes, fs_info.usedBytes, fs_info.totalBytes - fs_info.usedBytes,  
                        fs_info.maxOpenFiles, fs_info.maxPathLength, 
                        fs_info.blockSize, fs_info.pageSize ) ;
      #endif
      

    }
    
    // **************************************************    
    // **************************************************    
    void Dump ( String Filename ) {
      Serial.printf ( "Contents of %s\n", Filename.c_str() ) ;
      File file = SPIFFS.open ( Filename, "r" ) ;
      if ( file ) {
        while ( file.available() ){
          Serial.print ( file.readString() ) ;
        }
        file.close () ;
        Serial.println () ;
      }
    }

    // **************************************************    
    // **************************************************    
    bool Delete ( String Filename ) {
      return SPIFFS.remove ( Filename ) ;
    }
    
    // **************************************************    
    // **************************************************    
    bool Creat_TestFile ( String Filename, String Line ) {
      File file = SPIFFS.open ( Filename, "a" ) ;
      file.print ( Line ) ;
      file.close () ;
      return true;
    }
};

#endif
