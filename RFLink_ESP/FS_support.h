// Version 1.1,  14-04-2020, SM
//    - nu ook voor ESP32 filelist ordering removed (not tesed!!)
//
// Version 1.0,  27-02-2020, SM
//    - Ordering van filelist removed, vraagt teveel RAM (tzt oplossen in HTML)
//    - SPIFFS wordt reeds geopend in de class constructor
//    - method Read_File toegeveogd (allen geschilt voor kleine bestanden )
//
// Version 0.9,  18-02-2020, SM
//    - bij restart wordt aantal seconden in de file opgezocht, zodat dit een monotoon stijgende reeks is
//
// Version 0.8,  20-12-2019, SM
//    - Get_File_Nr added
//
// Version 0.7,  29-08-2019, SM
//    - poging tot toevoeging SDMMC
//
// Version 0.6,  02-08-2019, SM
//    - Dir_CheckList added
//
// Version 0.5,  28-07-2019, SM
//    - Exists added
//
// Version 0.4
//    - Get_Time_In_File hanged if file ended incorrect
//    - _FS_class.Begin ESP32 support added
//
// Version 0.3
//   - starting new file on size or time
//     remove oldest file
//
// Version 0.2
//   - automatic formatting ESP32 if no filesystem yet
//
// Version 0.1
//
// https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html

#ifndef FS_support_h
#define FS_support_h   1.1


#define FS_NO_GLOBALS
#include "FS.h"
#include "My_File_Support.h"

#ifdef ESP32
  #include "SPIFFS.h"
#endif

#include "My_StringSplitter.h"




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
    int           _Offset_Seconds = 0 ;

    // **************************************************  
    // **************************************************    
    _FS_class () { 
      // **********************************************************************
      // Bewust naar constructor verplaatst, zodat filesysteem direct bruikbaar
      // **********************************************************************
//_DEBUG_Global_String += "\r\n+ _FS_Class.creator" ;
      #ifdef ESP32
//fff        this->_Opened = SPIFFS.begin ( true ) ;  // format if no filesystem yet
//SPIFFS.format () ;
//delay(1000);
      #else
        if ( ! SPIFFS.begin ( ) ) {
          SPIFFS.format () ;
        }
        this->_Opened = SPIFFS.begin ( ) ;
      #endif
//SPIFFS.format () ;
      if ( this->_Opened ) Serial.println ( "SPIFFS succesfull opened" ) ;
      else          Serial.println ( "ERROR: SPIFFS could not be opened !!!! " ) ;
    }
  
    // **************************************************    
    // **************************************************    
    bool Begin ( String Filename="", long Max_File_Time = 24*60*60, int Max_NFile = 10  ) {
//_DEBUG_Global_String += "\r\n+ _FS_Class.Begin" ;
      #ifdef ESP32
        if ( !this->_Opened ) {
          this->_Opened = SPIFFS.begin ( true ) ;  // format if no filesystem yet
        }
Serial.println ( "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ" ) ;
Serial.println ( this->_Opened ) ;
        if ( this->_Opened ) {
          if ( _Parse_Filename ( Filename ) ) {
            String Last_File_Pre  = _Temp_Last_File_Pre  ;
            String Last_File_Post = _Temp_Last_File_Post ;
            int    Last_File_Nr   = 0                    ;
            String FileFound      ;
            fs::File dir = SPIFFS.open ( "/" ) ; 
            fs::File file = dir.openNextFile () ;
            while ( file ) {
              FileFound = file.name() ;
              if ( _Parse_Filename ( FileFound ) ) {
                if ( _Temp_Last_File_Nr > Last_File_Nr ) {
                  Last_File_Nr = _Temp_Last_File_Nr ;
                }
              }
              file = dir.openNextFile () ;
            }
            _Last_Filename  = Last_File_Pre + String ( Last_File_Nr ) + Last_File_Post ;
            _Last_File_Pre  = Last_File_Pre ;
            _Last_File_Post = Last_File_Post ;
            _Last_File_Nr   = Last_File_Nr  ;
            Serial.println ( "Last File (SPIFFS) = " + _Last_Filename ) ;
          }
        }
      #else
        if ( this->_Opened ) {
          if ( _Parse_Filename ( Filename ) ) {
            String Last_File_Pre  = _Temp_Last_File_Pre  ;
            String Last_File_Post = _Temp_Last_File_Post ;
            int    Last_File_Nr   = 0                    ;
            String FileFound      ;

            fs::Dir dir = SPIFFS.openDir ( Last_File_Pre ) ;
            while ( dir.next() ) {
              FileFound = dir.fileName() ;
              if ( _Parse_Filename ( FileFound ) ) {
                if ( _Temp_Last_File_Nr > Last_File_Nr ) {
                  Last_File_Nr = _Temp_Last_File_Nr ;
                }
              }
            }
            _Last_Filename  = Last_File_Pre + String ( Last_File_Nr ) + Last_File_Post ;
            _Last_File_Pre  = Last_File_Pre ;
            _Last_File_Post = Last_File_Post ;
            _Last_File_Nr   = Last_File_Nr  ;
            Serial.println ( "Last File (SPIFFS) = " + _Last_Filename ) ;
          }
        }
      #endif
      
      
      //   >0  aantal seconden (eerste kolom) dat een file maximaal mag bevatten
      //   <0  maximaal filesize (in bytes)
      _Max_FileSize = 0 ;
      _Max_FileTime = 0 ;
      if ( Max_File_Time > 0 ) {
        _Max_FileTime = Max_File_Time ;
      }
      else {
        _Max_FileSize = -Max_File_Time ;
      }
      _Max_NFile = Max_NFile - 2 ;
      if ( _Max_NFile < 0 ) {
        _Max_NFile = 0 ;
      }
      return this->_Opened ;
    }
    
    // **************************************************    
    // **************************************************    
    String DirList ( String Path = "/" ) {
      String Result = "" ;
      // **************************************************    
      // ESP32
      // **************************************************    
      #ifdef ESP32
        fs::File dir = SPIFFS.open ( "/" ) ;
        fs::File file = dir.openNextFile () ;
        Serial.println ( ">>>>>");
        while ( file ) {
          Serial.println ( file.name());
          Result += String ( file.name() ).substring(1) + "\n" ;
          file = dir.openNextFile () ;
        }

      // **************************************************    
      // ESP8266
      // **************************************************    
      #else
        fs::Dir dir = SPIFFS.openDir ( "/" ) ;
        Serial.println ( ">>>>>");
        while ( dir.next() ) {
          Serial.println ( dir.fileName());
          Result += dir.fileName().substring(1) + "\n" ;
        }
      #endif
      return Result ;
    }
    
    void DirList_Print ( String Path = "/" ) {
//    void DirList_Print ( fs::FS &Drive, String Path = "/" ) {
      //if ( Drive == SD_MMC ) { 
//Serial.println ( "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  FS_SUPPOYU" ) ; 

      Serial.println ( "=====  Files in SPIFFS  =====  " + Path ) ;
      int Count = 0 ;
      
      // **************************************************    
      // ESP32
      // **************************************************    
      #ifdef ESP32
        fs::File dir = SPIFFS.open ( "/" ) ;
        fs::File file = dir.openNextFile () ;
        while ( file ) {
          Serial.print   ( "    "  ) ;
          Serial.print   ( file.name() ) ;
          Serial.print   ( "\t["  );
          Serial.print   ( file.size() ) ;
          Serial.println ( "]" ) ;
          file = dir.openNextFile () ;
          Count += 1 ;
        }

        Serial.printf ( "Files=%i  Total=%i  Used=%i  Free=%i  MaxOpen=%i\n", 
                        Count, SPIFFS.totalBytes(), SPIFFS.usedBytes(), SPIFFS.totalBytes() - SPIFFS.usedBytes(), 10 ) ;
      // **************************************************    
      // ESP8266
      // **************************************************    
      #else
        fs::Dir dir = SPIFFS.openDir ( Path ) ;
        while ( dir.next() ) {
          Serial.print ( "    " + dir.fileName() + "\t[" );
          fs::File file = dir.openFile ( "r" ) ;
          Serial.print   ( file.size() ) ;
          Serial.println ( "]" ) ;
          Count += 1 ;
        }

        fs::FSInfo fs_info ;
        SPIFFS.info ( fs_info ) ;
        Serial.printf ( "Files=%i  Total=%i  Used=%i  Free=%i  MaxOpen=%i  MaxPathLength=%i  BlockSize=%i  PageSize=%i\n", 
                        Count, fs_info.totalBytes, fs_info.usedBytes, fs_info.totalBytes - fs_info.usedBytes,  
                        fs_info.maxOpenFiles, fs_info.maxPathLength, 
                        fs_info.blockSize, fs_info.pageSize ) ;
Print_Heap () ;      
      #endif
    }

    // **************************************************    
    // **************************************************  
    void HTML_File_CheckList ( String Path = "/", String Ends = "" ) {
      String Filename ;
      String FileExt  ;
      String Line     ;
      String Result   ;

      My_Webserver.sendContent ( "<h4>Data Files (unordered) on ESP-chip</h4>\n" ) ;
      My_Webserver.sendContent ( "<form action=\"/Files_Selected.php\" method=\"get\">\n" ) ;
      My_Webserver.sendContent ( "<table>\n" ) ;
      My_Webserver.sendContent ( "<thead><tr><td>Graph</td><td>Filename</td><td>Size</td><td>Download</td></tr></thead><tbody>\n" ) ;

      // **************************************************    
      // ESP32
      // **************************************************    
      #ifdef ESP32
        /* Moet worden herschreven naar zelfde stijl als ESP8266 (zonder Filelist)
        fs::File dir = SPIFFS.open ( Path ) ;
        fs::File file = dir.openNextFile () ;
        while ( file && ( i < _FileList_Max_N ) ) {
          String Filename = String( file.name() ).substring(1) ;
          if ( Ends.length() == 0 || Filename.endsWith ( Ends ) ) {
            _FileList[i] = Filename + "&emsp;[" + String ( file.size() ) + "]" ;
            i++ ;
          }
          file = dir.openNextFile () ;
        }
        */
        fs::File  dir = SPIFFS.open ( Path ) ;
        fs::File file = dir.openNextFile () ;
        while ( file ) {
          String Filename = String ( file.name() ).substring(1) ;
          if ( Ends.length() == 0 || Filename.endsWith ( Ends ) ) {
            Result = "" ;
            int x1 = Filename.indexOf ( '.' ) ;
            FileExt = Filename.substring ( x1+1 );
            FileExt.toLowerCase() ;
            Result = "<tr><td>" ;
            //if ( FileExt.endsWith ( "csv" ) ) {
            if ( FileExt == "csv" ) {
              Result += F("<label><input type=\"checkbox\" name=\"") ;
              Result += Filename ;
              Result += "\"></label>" ;
            } 
            Result += "</td><td>" + Filename + "</td><td>" + String ( file.size() ) ;
            
            Result += "</td><td><a href=\"" ;
            Result += Filename ;
            Result += F("\" download>Download</a><td></tr>\n") ;
           
            My_Webserver.sendContent ( Result.c_str() ) ;
            delay(40);        
            yield() ;        
          }
          file = dir.openNextFile () ;
        }

      // **************************************************    
      // ESP8266
      // **************************************************    
      #else
        fs::Dir dir = SPIFFS.openDir ( Path ) ;
        while ( dir.next() ) {
          String Filename = String(dir.fileName()).substring(1) ;
          if ( Ends.length() == 0 || Filename.endsWith ( Ends ) ) {
            Result = "" ;
            fs::File file = dir.openFile ( "r" ) ;
            Filename = String(dir.fileName()).substring(1) ;
            //Line     = Filename + "&emsp;[" + String ( file.size() ) + "]" ;
            //Line     = Filename + "</td><td>" + String ( file.size() ) + "</td><td>" ;

            int x1 = Filename.indexOf ( '.' ) ;
            FileExt = Filename.substring ( x1+1 );
            FileExt.toLowerCase() ;
//Serial.println ( x1 ) ;
//Serial.println ( FileExt  );
            Result = "<tr><td>" ;
            //if ( FileExt.endsWith ( "csv" ) ) {
            if ( FileExt == "csv" ) {
              Result += F("<label><input type=\"checkbox\" name=\"") ;
              Result += Filename ;
              Result += "\"></label>" ;
            } 
            Result += "</td><td>" + Filename + "</td><td>" + String ( file.size() ) ;
            
            Result += "</td><td><a href=\"" ;
            Result += Filename ;
            Result += F("\" download>Download</a><td></tr>\n") ;
           
            My_Webserver.sendContent ( Result.c_str() ) ;
    delay(40);        
    yield() ;        
            
          }
        }          
      #endif 
      My_Webserver.sendContent ( "</tbody></table><input type=\"submit\" value=\"Show Graph(s)\">\n</form>\n") ;
    }

    // **************************************************    
    // **************************************************    
    int Get_File_Nr ( ) {
      return _Last_File_Nr ;
    }

    // **************************************************    
    // **************************************************    
    String Get_Next_Filename () {
      if ( _Last_Filename.length() == 0 ) {
        _Get_Last_Filename () ;
      }

      _Last_File_Nr += 1 ;
      if ( _Last_File_Pre.length() == 0 ) {
        _Last_File_Pre = "/MLX_" ;
        _Last_File_Post= ".bin" ;
      }
      
      //if ( _Last_File_Nr < 10 ) _Last_Filename = _Last_File_Pre + "0" + String ( _Last_File_Nr ) + _Last_File_Post ;
      //else _Last_Filename = _Last_File_Pre + String ( _Last_File_Nr ) + _Last_File_Post ;
      _Last_Filename = _Last_File_Pre + String ( _Last_File_Nr ) + _Last_File_Post ;
      
      return _Last_Filename ;
    }




    // **************************************************    
    // ene variant om de laatste MLX_xxx.bin file te bepalen
    // **************************************************    
    int Get_Last_File_Number () {
      
      int Last_FileNr = 0 ;
      // **************************************************    
      // ESP32
      // **************************************************    
      #ifdef ESP32
        fs::File dir = SPIFFS.open ( "/" ) ;
        fs::File file = dir.openNextFile () ;
        while ( file ) {
          String Filename = file.name() ;
          int    FileNr   = Parse_FileNr ( Filename ) ;
          if ( FileNr > Last_FileNr ) Last_FileNr = FileNr ;
          file = dir.openNextFile () ;
       }
      // **************************************************    
      // ESP8266
      // **************************************************    
      #else
        fs::Dir dir = SPIFFS.openDir ( "/" ) ;
        while ( dir.next() ) {
          String Filename = dir.fileName() ;
          int    FileNr   = Parse_FileNr ( Filename ) ;
          if ( FileNr > Last_FileNr ) Last_FileNr = FileNr ;
        }
      #endif
      return Last_FileNr ;
    }

    // **************************************************    
    // **************************************************    
    String _Get_Last_Filename () {
      if ( _Last_Filename.length() > 0 ) {
        return _Last_Filename ;
      }
      
      // **************************************************    
      // ESP32
      // **************************************************    
      #ifdef ESP32
        fs::File dir = SPIFFS.open ( "/" ) ;
        fs::File file = dir.openNextFile () ;
        while ( file ) {
          _Last_Filename = file.name() ;
           file = dir.openNextFile () ;
       }
      // **************************************************    
      // ESP8266
      // **************************************************    
      #else
        fs::Dir dir = SPIFFS.openDir ( "/" ) ;
        while ( dir.next() ) {
          //_Last_Filename = dir.Filename() ;
          _Last_Filename = dir.fileName() ;
        }
      #endif
      
      // **************************************************    
      // Parse the filename and store the parts
      // **************************************************    
      int x1 = _Last_Filename.indexOf ( '_' ) ;  
      int x2 = _Last_Filename.indexOf ( '.' ) ;       
      if ( x1 > 0 ) {
        _Last_File_Pre  =   _Last_Filename.substring ( 0   , x1+1 ) ;
        _Last_File_Nr   = ( _Last_Filename.substring ( x1+1, x2   ) ).toInt () ;
        _Last_File_Post =   _Last_Filename.substring ( x2         ) ;
      }
      else {
        _Last_File_Pre  = _Last_Filename.substring ( 0   , x2 ) + "_" ;
        _Last_File_Nr   = 0 ;
        _Last_File_Post = _Last_Filename.substring ( x2       ) ;
        
      }
      return _Last_Filename ;
    }

    
    // **************************************************    
    // **************************************************    
    void Dump ( String Filename ) {
      Serial.println ( "===== Dump " + Filename ) ;
      fs::File file = SPIFFS.open ( Filename, "r" ) ;
      if ( file ) {
        while ( file.available() ){
          Serial.print ( file.readString() ) ;
        }
        file.close () ;
        Serial.println ( "\n===============" ) ;
      }
      else {
        Serial.println ( "ERROR: file not found" ) ;
      }
    }
    // **************************************************    
    // **************************************************    
    String Read_File ( String Filename ) {
      String Line ;
      fs::File file = SPIFFS.open ( Filename, "r" ) ;
      if ( file ) {
        while ( file.available() ){
          Line += file.readString() ;
        }
        file.close () ;
      }
      return Line ;
    }

    // **************************************************    
    // **************************************************    
    int Get_Time_In_File ( String Filename ) {
      int Delta_T = 0 ;
      fs::File file = SPIFFS.open ( Filename, "r" ) ;
      String Line_0 ;
      String Line_1 ;
      String Line_2 ;
      String Line_x ;
      if ( !file ) {
        return 0 ;
      }
      if ( ! file.available() ) {
        return 0 ;
      }

      Line_0 = file.readStringUntil ( '\n' ) ;
      My_StringSplitter *Splitter = new My_StringSplitter ( Line_0, '\t' ) ;
      int ItemCount = Splitter -> getItemCount () ;
      int DT_i ;
      for ( DT_i = 0; DT_i < ItemCount; DT_i++ ) {
        String Item = Splitter -> getItemAtIndex ( DT_i ) ;
        if ( Item == "Seconds" ) {
          break ;
        }
      }
      if ( DT_i >= ItemCount ) {
        return 0 ;
      }
      if ( !file.available() ) {
        return 0 ;
      }

      Line_1 = file.readStringUntil ( '\n' ) ;
      Splitter -> newString ( Line_1, '\t' ) ;
      String Date1 = Splitter -> getItemAtIndex ( DT_i ) ;
      int DT1 = Date1.toInt () ;
      if ( !file.available() ) {
        return 0 ;
      }

      Line_2 = file.readStringUntil ( '\n' ) ;
      Splitter -> newString ( Line_2, '\t' ) ;
      String Date2 = Splitter -> getItemAtIndex ( DT_i ) ;
      int DT2 = Date2.toInt () ;
      if ( !file.available() ) {
        return 0 ;
      }

      Line_x = "###" ;
      while ( file.available() && ( Line_x.length() > 1 ) ){
        Line_x = file.readStringUntil ( '\n' ) ;
//Serial.println ( Line_x ) ;        
        Splitter -> newString ( Line_x, '\t' ) ;
        String Datex = Splitter -> getItemAtIndex ( DT_i ) ;
        int DTx = Datex.toInt () ;
        
        
        if ( DTx > DT2 ) {
          Delta_T += DTx - DT2 ;
        }        
        DT2 = DTx ;
      }
      file.close () ;
      return Delta_T ;
    }

    // **************************************************    
    // **************************************************    
/*
    int Get_Time_In_File_HANGT_TEVEEL_AF_VAN_NTP_SERVER ( String Filename ) {
      File file = SPIFFS.open ( Filename, "r" ) ;
      String Line_0 ;
      String Line_1 ;
      String Line_x ;
      if ( file ) {
        if ( file.available() ) {
          Line_0 = file.readStringUntil ( '\n' ) ;
        }
        if ( file.available() ) {
          Line_1 = file.readStringUntil ( '\n' ) ;
        }
        while ( file.available() ){
          Line_x = file.readStringUntil ( '\n' ) ;
        }
        file.close () ;

        My_StringSplitter *Splitter = new My_StringSplitter ( Line_0, '\t' ) ;
        int ItemCount = Splitter -> getItemCount () ;
        int DT_i ;
        for ( DT_i = 0; DT_i < ItemCount; DT_i++ ) {
          String Item = Splitter -> getItemAtIndex ( DT_i ) ;
          if ( Item == "DateTime" ) {
            break ;
          }
        }

        if ( DT_i < ItemCount ) {
          Splitter -> newString ( Line_1, '\t' ) ;
          String Date1 = Splitter -> getItemAtIndex ( DT_i ) ;
          time_t DT1 = String_2_UnixTime ( Date1 ) ;

          Splitter -> newString ( Line_x, '\t' ) ;
          String Date2 = Splitter -> getItemAtIndex ( DT_i ) ;
          time_t DT2 = String_2_UnixTime ( Date2 ) ;

          Serial.print ( "Delta T in file = ") ;
          Serial.println ( DT2-DT1 ) ;
          return (int) (DT2-DT1) ;
        }
        else {
          return 0 ;
        }
      }
      else {
        return 0 ;
      }
    }
*/
    // **************************************************    
    // **************************************************    
    bool Exists ( String Filename ) {
      fs::File file = SPIFFS.open ( Filename, "r" ) ;
      if ( file ) {
        file.close () ;
        return true ;
      }
      return false ;
    }

    // **************************************************    
    // **************************************************    
    void DumpAll () {
      // **************************************************    
      // ESP32
      // **************************************************    
      #ifdef ESP32
        fs::File dir = SPIFFS.open ( "" ) ;
        fs::File file = dir.openNextFile () ;
        while ( file ) {
          String Filename = file.name () ;
          Serial.println ( Filename ) ;
          Serial.println ( file.size() ) ;
          if ( file ) {
            while ( file.available() ){
              Serial.print ( file.readString() ) ;
            }
            file.close () ;
            //if ( Delete_Files ) {
            //  Delete ( Filename ) ;
            //}  
          }
          file = dir.openNextFile () ;
        }

      // **************************************************    
      // ESP8266
      // **************************************************    
      #else
        //String Result ;
        fs::Dir dir = SPIFFS.openDir ( "" ) ;
        while ( dir.next() ) {
          //Result += dir.fileName() + "\n" ;
          fs::File file = dir.openFile ( "r" ) ;
          if ( file ) {
            Serial.print   ( "===== Contents of " ) ;
            Serial.print   ( dir.fileName() ) ;
            Serial.print   ( "    " ) ;
            Serial.println ( file.size() ) ;
            while ( file.available() ){
              Serial.print ( file.readString() ) ;
            }
            file.close () ;
            Serial.println ( "\n===============" ) ;
          }
        }
      #endif
      return ;
    }

    // **************************************************    
    // **************************************************    
    bool Delete ( String Filename ) {
      Serial.println ( "===== Delete " + Filename ) ;
      bool Result = SPIFFS.remove ( Filename ) ;
      if ( Result ) {
        Serial.println ( "\n===============" ) ;
      }
      else {
        Serial.println ( "ERROR: file not found" ) ;        
      }
    }

    // **************************************************    
    // **************************************************    
    void Delete_All () {
      Serial.println ( "===== Delete All Files" ) ;
      String Filename ;

      #ifdef ESP32
        Serial.println ( "!!!!!!!! ToDo _FS_class.Delete_All" ) ;
      #else
      fs::Dir dir = SPIFFS.openDir ( "" ) ;
      while ( dir.next() ) {
        Filename = dir.fileName() ;
        Serial.println ( "Delete : " + Filename ) ;
        SPIFFS.remove ( Filename ) ;
      }
      #endif
    }
    
    // **************************************************    
    // **************************************************    
    bool Store_File ( String Filename, String Line ) {
      fs::File file = SPIFFS.open ( Filename, "w" ) ;
      file.print ( Line ) ;
      file.close () ;
      return true;
    }
    
    // **************************************************    
    // **************************************************    
    bool Append_File ( String Filename, String Line ) {
      fs::File file = SPIFFS.open ( Filename, "a" ) ;
      file.print ( Line ) ;
      file.close () ;
      return true;
    }
    
    // **************************************************    
    // **************************************************    
    bool Create_CSV_File ( String Filename, String Header ) {
      if ( not SPIFFS.exists ( Filename ) ) {
        Append_File ( Filename, Header ) ;
	    }
    }
    
    // **************************************************    
    // **************************************************    
    bool Create_CSV_File_Nr ( String Filename, String Header ) {
      // ************************************************************
      // als ongeveer een dag opgenomen, ga over naar een nieuwe file
      // ************************************************************
      bool NewFile = false ;
      if ( _Max_FileSize > 0 ) {
        fs::File file = SPIFFS.open ( Filename, "r" ) ;
        if ( file.size() > _Max_FileSize ) {
          NewFile = true ;
        }
        file.close() ;
      }
      else if ( _Max_FileTime > 0 ) { 
        // ************************************************************
        // After a reset check how many seconds are already in the file
        // ************************************************************
        if ( !_Initialized ) {
//Serial.println ( "Before gettime" ) ;          
          _Offset_Seconds = millis()/1000 + 10 + Get_Time_In_File ( Filename );
//Serial.println ( "Agter gettime" ) ;          
          if ( _Offset_Seconds > _Max_FileTime ) {
            NewFile = true ;
          }
          _Initialized   = true ;
        }
/*
Serial.print   ( "millis / Delta / maxfileTIME / Offset = ");
Serial.print ( millis() ) ;
Serial.print ( " / " ); 
Serial.print ( _Max_FileTime - _Offset_Seconds ) ;
Serial.print ( " / " ) ; 
Serial.print ( _Max_FileTime ) ;
Serial.print ( " / " ); 
Serial.println ( _Offset_Seconds ) ;
//*/
        //if ( ( millis() - _File_Millis ) > ( _Max_FileTime - _Offset_Seconds ) ) {
        if ( ( millis()/1000 + _Offset_Seconds ) > _Max_FileTime ) {
          NewFile = true ;
        }
      }
      
      
      
      
      //if ( ( millis() - _File_Millis ) > _Max_FileTime ) {
      if ( NewFile ) {
        //_File_Millis = millis() ;
        if ( _Last_File_Nr > _Max_NFile ) {
          String File_To_Remove =  _Last_File_Pre + String ( _Last_File_Nr - _Max_NFile ) + _Last_File_Post ;
          SPIFFS.remove ( File_To_Remove ) ;
          Serial.println ( "rrrrrrrrrrrrrrrrrr " + File_To_Remove ) ;
        }
        SPIFFS.rename ( Filename, Get_Next_Filename() ) ;
        Serial.println ( "???????????????????" + _Get_Last_Filename() ) ;
      }
      
      //_Get_Last_Filename () ;
      if ( not SPIFFS.exists ( Filename ) ) {
        Append_File ( Filename, Header ) ;
        _Offset_Seconds = - ( millis()/1000 ) ;
	    }
    }
    
  // *************************************************  
  private:  
  // *************************************************  
    bool   _Opened         = false ;
    String _Last_Filename  = "" ;
    String _Last_File_Pre  = "" ;
    String _Last_File_Post = "" ;
    int    _Last_File_Nr   = 0  ;

    String _Temp_Last_File_Pre   = "" ;
    String _Temp_Last_File_Post  = "" ;
    int    _Temp_Last_File_Nr    = 0  ;

    //unsigned long _File_Millis   = 0 ;
    bool          _Initialized   = false ;
    //int           _Offset_Millis = 0 ;
    
    unsigned long _Max_FileTime      ;
    long          _Max_FileSize      ;
    int           _Max_NFile         ;
    

    // **************************************************    
    // Parse the filename and store the parts
    // **************************************************    
    bool _Parse_Filename ( String Filename ) {
      if ( Filename.length() == 0 ) {
        return false ;
      }
      int x1 = Filename.indexOf ( '_' ) ;  
      int x2 = Filename.indexOf ( '.' ) ;       
      if ( x1 > 0 ) {
        _Temp_Last_File_Pre  =   Filename.substring ( 0   , x1+1 ) ;
        _Temp_Last_File_Nr   = ( Filename.substring ( x1+1, x2   ) ).toInt () ;
        _Temp_Last_File_Post =   Filename.substring ( x2         ) ;
      }
      else {
        _Temp_Last_File_Pre  = Filename.substring ( 0   , x2 ) + "_" ;
        _Temp_Last_File_Nr   = 0 ;
        _Temp_Last_File_Post = Filename.substring ( x2       ) ;
      }
      return true ;
    }    
    
  
};


// ***********************************************************************************
// ***********************************************************************************
_FS_class   File_System ; 


#endif
