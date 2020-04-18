
// Version 0.2   22-02-2020,  SM
//    - opschoning Send_Data
//
// Version 0.1   05-02-2020, SM
//    - initial release
//    - werkt behoorlijk goed
//    - homepage werkt alleen als niette ceel files op sdcard
//    - downloaden + graph+ ftp werken nog niet op SDcard
//
// ***********************************************************************************
// ***********************************************************************************
//  C:\Users\Mattijs\Documents\Arduino\libraries\SdFat\src\FatLib
//     ArduinoFiles.h
//     FatFileSystem.h
//       "File" vervangen door "Arduino_File"
//       conflicteert anders met FS.h
//
// Inmiddels overgestapt op de Adafruit forl op SDfat
//  en op de volgende manier het conflicht opgelost
/*
there is a define for this to be used before header call

#define FS_NO_GLOBALS
#include <FS.h>

then you must add namespace fs for SPIFFS objects like
fs::File currentfile = SPIFFS.open("/myfile.txt", "r");
fs::DIR 
fs:FSINFO
etc

but for SD no need :
File textfile = SD.open("/myfile.txt", FILE_READ);
*/

// ToDo: add timestamp
/*  // create a new file with default timestamps
  if (!file.open("stamp.txt", O_WRONLY | O_CREAT)) {
    error("open stamp.txt failed");
  }
  // set creation date time
  if (!file.timestamp(T_CREATE, 2014, 11, 10, 1, 2, 3)) {
    error("set create time failed");
  }
  // set write/modification date time
  if (!file.timestamp(T_WRITE, 2014, 11, 11, 4, 5, 6)) {
    error("set write time failed");
  }
  // set access date
  if (!file.timestamp(T_ACCESS, 2014, 11, 12, 7, 8, 9)) {
    error("set access time failed");
  }
  cout << F("\nTimes after timestamp() calls\n");
  printTimestamps(file);

  file.close();
*/
// ***********************************************************************************
// ***********************************************************************************


#ifndef Receiver_SDfat_h
#define Receiver_SDfat_h   0.2

#include "SdFat.h"
#include "sdios.h"

#include "My_File_Support.h"
#include "FS_support.h"         // is dat nodig ???


// MOET WORDEN VERWIJDERS VAN FILELIST BESTAAT NIET MEER
// ***********************************************************************************
// ***********************************************************************************
void MySort ( String String_List[], int Len, bool Reversed ) {
  String Temp ;
  for ( int Start=1; Start<Len; Start++ ) {
    for ( int i=Start; i<Len; i++ ) {
      if ( ( !Reversed && ( String_List [i] < String_List [Start-1] ) ) ||
           ( Reversed  && ( String_List [i] > String_List [Start-1] ) ) ) {
//      if ( String_List [i] < String_List [Start-1] ) {
        Temp                  = String_List [i] ;
        String_List [i]       = String_List [Start-1] ;
        String_List [Start-1] = Temp ;
      }
    }
  }
 
  /*
  for ( int i2=0; i2<Len; i2++ ) {
    Serial.println ( "Sorted_List [" + String (i2) + "] : " + String_List [i2] ) ;
  }
  Serial.println ( "Soretd-Soretd-Soretd-Soretd-Soretd-Soretd-");
  //*/
}



// Receiver_SDfat ********************************************************************
// ***********************************************************************************
class _Receiver_SDfat : public _Receiver_BaseClass {
  
  public:
SdFat  SDcard ;
SdFile SDpath ;
SdFile SDfile ;

  
    String Help_Text = "#define Receiver_SDfat  <Filename>\n\
#define Receiver_SDfat  <Filename>  <CS>\n\
    <CS> Chip Select pin, defaults to GPIO4\n\
    <Filename>            defaults to \"temp.txt\"" ;
   
    // Receiver_SDfat ********************************************************
    // Creator, 
    // ***********************************************************************
    _Receiver_SDfat (  int SD_Card_CS = 14, String Filename = "Data.csv", int Max_File_Seconds=600, int Max_NFile=10 ) {
      this -> _SD_Card_CS       = SD_Card_CS ;
      this -> _Data_Filename    = Filename ;
      this -> _Max_File_Seconds = Max_File_Seconds ;
      this -> _Max_NFile        = Max_NFile ;
      this -> Constructor_Finish () ;      
    }

    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void Constructor_Finish () {
      _Is_Receiver_SDFat = true ;
      Version_Name = "V" + String ( Receiver_SDfat_h ) + "   Receiver_SDfat.h" ;
      Serial.println ( "CREATE    " + Version_Name ) ;

      int x1 = this->_Data_Filename.indexOf ( '.' ) ;
      _Data_Filename_Prefix = this->_Data_Filename.substring ( 0, x1 ) + '_' ;
      _Data_Filename_Ext    = this->_Data_Filename.substring ( x1 ) ;
      
    }
    
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void setup () {
      Serial.print ( "\n#define Receiver_SDfat  " ) ;
      Serial.print ( _Data_Filename + "  " ) ;
	    Serial.print ( _SD_Card_CS ) ;
      Serial.print ( "\n    Initializing SD card ... " ) ;

      if  ( SDcard.begin ( _SD_Card_CS, SD_SCK_MHZ (50) ) ) {
        Serial.println ( "SD-Card initialized.");
        _Card_Present = true ;
      }
      else {
        Serial.println ( "SD-Card failed, or not present" ) ;
        //SDcard.initErrorHalt();
        _Card_Present = false ;
      }

 	    Serial.println ( Help_Text ) ;
   }

    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    bool Send_Data ( String JSON_Message ) {
      if ( !_Card_Present ) return false ;
      SdFile DataFile ; 
      
      // *****************************************************
      // create the file if it doesn't exists and filestamp it
      // *****************************************************
      if ( !_DataFile_Initialized ) {
        this->_Seconds_In_File = Get_LastTime_In_File ( _Data_Filename ) ;
        if ( this->_Seconds_In_File < 0 ) {
          DataFile.open ( _Data_Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT ) ;
          DataFile.println ( JSON_Short_Header ) ;
          // *****************************************************
          // Cancel callback so sync will not change access/modify timestamp
          // *****************************************************
          SdFile::dateTimeCallbackCancel();
          DataFile.timestamp ( T_CREATE, Global_DateTime.Year, Global_DateTime.Month , Global_DateTime.Day,
                                         Global_DateTime.Hour , Global_DateTime.Minute, Global_DateTime.Second ) ;
          DataFile.timestamp ( T_WRITE, Global_DateTime.Year, Global_DateTime.Month , Global_DateTime.Day,
                                         Global_DateTime.Hour , Global_DateTime.Minute, Global_DateTime.Second ) ;
          DataFile.timestamp ( T_ACCESS, Global_DateTime.Year, Global_DateTime.Month , Global_DateTime.Day,
                                         Global_DateTime.Hour , Global_DateTime.Minute, Global_DateTime.Second ) ;
          DataFile.close();
          this->_Seconds_In_File = - ( millis()/1000 ) ;
        }
        _DataFile_Initialized = true ;
      }
      
      //******************************************************
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      // FILE_WRITE = read / append
      //******************************************************
      DataFile.open ( _Data_Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT ) ;
      if ( !DataFile.isOpen () ) return false;
      
      int Seconds  = millis()/1000 + this->_Seconds_In_File ;
      int FileSize = DataFile.fileSize() ;
            
      bool New_File_Needed = false ;
      New_File_Needed = ( ( this->_Max_File_Seconds > 0 ) && ( Seconds  >   this->_Max_File_Seconds ) ) ||\
                        ( ( this->_Max_File_Seconds < 0 ) && ( FileSize > - this->_Max_File_Seconds ) );
      
      //******************************************************
      // create new file if needed,
      //    remove the oldest file, if too many
      //******************************************************
      if ( New_File_Needed ) {
        int Last_FileNr = Get_Last_File_Number () ;
        String New_Filename ;
        if ( Last_FileNr >= this->_Max_NFile ) {
          New_Filename = this->_Data_Filename_Prefix + String ( Last_FileNr - this->_Max_NFile + 2 ) + this->_Data_Filename_Ext ;
          this->Delete ( New_Filename ) ;
        }

        // *****************
        // create a new file
        // *****************
        New_Filename = this->_Data_Filename_Prefix + String ( Get_Last_File_Number() + 1 ) + this->_Data_Filename_Ext ;
        DataFile.rename ( New_Filename.c_str() ) ;
      }
      DataFile.close () ;
      DataFile.open ( _Data_Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT ) ;
      
      FileSize = DataFile.fileSize() ;
      if ( FileSize == 0 ) {
        DataFile.println ( JSON_Short_Header ) ;
        this->_Seconds_In_File = - ( millis()/1000) ;
        
        // *****************************************************
        // Cancel callback so sync will not change access/modify timestamp
        // *****************************************************
        SdFile::dateTimeCallbackCancel();
        DataFile.timestamp ( T_CREATE, Global_DateTime.Year, Global_DateTime.Month , Global_DateTime.Day,
                                       Global_DateTime.Hour , Global_DateTime.Minute, Global_DateTime.Second ) ;
        DataFile.timestamp ( T_WRITE , Global_DateTime.Year, Global_DateTime.Month , Global_DateTime.Day,
                                       Global_DateTime.Hour , Global_DateTime.Minute, Global_DateTime.Second ) ;
        DataFile.timestamp ( T_ACCESS, Global_DateTime.Year, Global_DateTime.Month , Global_DateTime.Day,
                                       Global_DateTime.Hour , Global_DateTime.Minute, Global_DateTime.Second ) ;
      }
      
      //******************************************************
      // if the file is available, write to it:
      // neemt zo'n 18 msec in beslag
      //******************************************************
      if ( DataFile.isOpen() ) {
        DataFile.println ( String ( millis()/1000 + this->_Seconds_In_File ) + "\t" + JSON_Short_Data ) ;
        DataFile.close();
        return true ;
      }

      //******************************************************
      // if the file isn't open, pop up and return false
      //******************************************************
      else {
        Serial.print   ( "ERROR opening : " ) ;
        Serial.println ( _Data_Filename ) ;
        return false ;
      }
    }

    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    bool Handle_Serial_Command ( String Serial_Command ) {
      if ( !_Card_Present ) return false ;
      
      String Filename ;
      String LowerCase = Serial_Command ;
	    LowerCase.toLowerCase () ;
	  
      //******************************************
	    if ( LowerCase.startsWith ( "help" ) ) {
      //******************************************
		    Serial.println ( _Serial_Commands_Text ) ;
		    return false ;
	    }
      
      //******************************************
	    else if ( LowerCase.startsWith ( "?" ) ) {
      //******************************************
		    Serial.println ( _Serial_Commands_Text ) ;
		    return false ;
	    }
      
      //******************************************
      else if ( LowerCase.startsWith ( "dirr" ) ) {
      //******************************************
        this -> Get_CWD () ;
        Filename = Serial_Command.substring ( 5 ) ;
        if ( Filename.length() == 0 ) Filename = "/" ;
        Serial.println ( "======  Recursed Files in :  \"" + Filename + "\"  ======" ) ;
        SdFile SDfile ( Filename.c_str(), O_RDONLY ) ;
        SDfile.ls (  &Serial, LS_R ) ;
        SDfile.close () ;   
        Serial.println ("=====================================") ;
        return false ;
      }
      
      //******************************************
      else if ( LowerCase.startsWith ( "dir" ) ) {
      //******************************************
//this -> Get_CWD () ;
        Filename = Serial_Command.substring ( 4 ) ;
        if ( Filename.length() == 0 ) Filename = "/" ;

        /*
        // IK WEET NIET HOE IK DE HUIDIGE CWD INHOUD OPVRAG !!
        String Path ;
        if ( Filename.length() == 0 ) {
          SDcard.vwd()->getName (  _SDcard_Line, sizeof(_SDcard_Line) ) ;
          Path = _SDcard_Line ;
          Serial.println ( "Path = " + Path ) ;
        } else Path = Filename ;
        */
        
        this -> Dir2 () ;
        return false ;
      }
      
      //******************************************
      else if ( LowerCase.startsWith ( "cd" ) ) {
      //******************************************
//this -> Get_CWD () ;
        String Path = Serial_Command.substring ( 3 ) ;
        Serial.println ( Path ) ;
        
        SDcard.chdir ( Path.c_str(), true ) ;
        SDcard.vwd()->getName (  _SDcard_Line, sizeof(_SDcard_Line) ) ;
        Serial.println ( _SDcard_Line ) ;
        
        Serial.print ("============================= CD --> " ) ;
        this -> Get_CWD () ;
        return true ;
      }
      
      //******************************************
      else if ( LowerCase.startsWith ( "md" ) ) {
      //******************************************
//this -> Get_CWD () ;
        String Path = Serial_Command.substring ( 3 ) ;
        this -> MakeDir ( Path ) ;
        Serial.print ("============================= MD --> " + Path ) ;
        return true ;
      }
      
      //******************************************
      else if ( LowerCase.startsWith ( "dump " ) ) {
      //******************************************
        Filename = Serial_Command.substring ( 5 ) ;
        this -> Dump ( Filename ) ;
        //SdFile SDfile ( Filename.c_str(), O_RDONLY ) ;
        //SDfile.dmpFile ( &Serial, 0, 100 ) ;         
      }
      
      //******************************************
      else if ( LowerCase.startsWith ( "dumpall" ) ) {
      //******************************************
        String Path = Serial_Command.substring ( 8 ) ;
        if ( Path.length() == 0 ) Path = "/" ;
        SDcard.chdir ( Path.c_str() ) ;
        // vwd= "working Directory"
        while ( SDfile.openNext ( SDcard.vwd(), O_READ )) {
          if ( SDfile.isFile () ) {
            SDfile.getName (  _SDcard_Line, sizeof(_SDcard_Line) ) ;
            String Filename = _SDcard_Line ;
            SDfile.close () ;
            this -> Dump ( Filename ) ;
            
            /*
            SDfile.printModifyDateTime ( &Serial ) ;
            Serial.print ( '\t' ) ;
            SDfile.printFileSize ( &Serial ) ;
            Serial.print ( "\t\t" ) ;
            SDfile.printName ( &Serial ) ;
            Serial.println () ;
            */
          }
          else SDfile.close();
        }
      }
      
      //******************************************
      else if ( LowerCase.startsWith ( "del " ) ) {
      //******************************************
        Filename = Serial_Command.substring ( 4 ) ;
        SdFile SDfile ( Filename.c_str(), O_RDWR ) ;
        if ( SDfile.isOpen() ) {
          bool Result = SDfile.remove () ;
          if ( Result ) {
            Serial.print ("============================= Deleted : " + Filename ) ;
          }
          else {
            Serial.println ( "ERROR: file not found." ) ;        
          }
        }
      }  

      //******************************************
      else if ( LowerCase == "delall yes" ) {
      //******************************************
        Serial.println ( "DELALL YES, not yet implemented" ) ;
      }

      //******************************************
      else if ( LowerCase.startsWith ( "create " ) ) {
      //******************************************
        Filename = Serial_Command.substring ( 7 ) ;
        SdFile SDfile ( Filename.c_str(), FILE_WRITE ) ;
        SDfile.write ( "This is the first line\n" ) ;
        SDfile.close () ;
      }

      //******************************************
      else {
      //******************************************
        return false ;
      }
	  }

    
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void Append_2_CSV ( String Filename, String Header, String Data ) {
      if ( !_Card_Present ) return ;
      
      SdFile SDfile ( Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT ) ;
      if ( SDfile.isOpen () ) {
        SDfile.println ( Data ) ;
      SDfile.close();
      }
    }
     
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void HTML_File_CheckList ( String Path = "/", String Ends = "" ) {
/* moet worden heschreven zonder sort !! zie FS_support      
      if ( !_Card_Present ) return ;
      
      String Filename ;
      String FileExt  ;
      String Line     ;
      String Result   ;
      
      this -> Get_Ordered_DirList ( Path, Ends, Reversed ) ;
      Serial.println ( _FileList_Len );

      My_Webserver.sendContent ( F("<h4>Data Files on SD-card</h4>\n") ) ;
      My_Webserver.sendContent ( F("<form action=\"/Files_Selected_SD.php\" method=\"get\">\n") ) ;
      My_StringSplitter *Splitter = new My_StringSplitter ( "", '\t' ) ;
      for ( int i=0; i<_FileList_Len; i++ ) {
        Line = _FileList [i] ;
        Splitter -> newString ( Line, '&' ) ;
        Filename = Splitter -> getItemAtIndex ( 0 ) ;
        Splitter -> newString ( Filename, '.' ) ;
        FileExt  = Splitter -> getItemAtIndex ( -1 ) ;   //eigenlijk wil ik hier index=-1 !! de laatset

        FileExt.toLowerCase() ;
        if ( FileExt == "csv" ) {
          Result = F("<label><input type=\"checkbox\" name=\"") ;
          Result += Line ;
          Result += "\">" ;
          Result += Line ;
          Result += "</label>" ;
        } 
        else {
          Result = "&emsp;" + Line ;
        }
 
        Result += "&emsp;&emsp;<a href=\"" ;
        Result += Filename ;
        Result += F("\" download>Download</a><br>\n") ;
        
        if ( ( i % 10 ) == 9 ) {
          Result += "<br>" ;
        }
        
        My_Webserver.sendContent ( Result.c_str() ) ;
      }
      My_Webserver.sendContent ( F("<br><input type=\"submit\" value=\"Show Graph(s)\">\n\
</form>\n") ) ;
//*/
    }
    
  // Receiver_SDfat ********************************************************
  private :
  // ***********************************************************************
    String _Data_Filename        ;
    String _Data_Filename_Prefix ;
    String _Data_Filename_Ext    ;
    int    _SD_Card_CS    ;
    bool   _Card_Present         = false ;
    bool   _DataFile_Initialized = false ;
    int    _Seconds_In_File ;
    int    _Max_File_Seconds ;
    int    _Max_NFile ;

    char _SDcard_Line [ 250 ] ;


    
    String _Serial_Commands_Text = "======  Receiver_SDfat  ======\n\
dir  [FILENAME] // Display Alle bestanden in deze (of huidige) directory\n\
dirr [FILENAME] // Display alle bestanden recursively\n\
cd    Filename  // change directory\n\
md    Filename  // make dierectory \n\
dump  FILENAME  // Print de inhoude van een bestand\n\
dumpALL         // Print de inhoud van alle bestanden\n\
create FILENAME // Create bestand als nog niet bestaat , append regel text\n\
del    FILENAME // Delete bestand\n\
delALL YES      // TODO Delete ALL bestanden";
  
  
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void Get_CWD () {
      if ( !_Card_Present ) return ;
      
      SDcard.vwd()->getName (  _SDcard_Line, sizeof(_SDcard_Line) ) ;
      Serial.println ( "CWD = " + String ( _SDcard_Line ) ) ;
    }
    
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void Dir ( String Path, String Filename="" ) {
      if ( !_Card_Present ) return ;
      
      if ( Filename.length() == 0 ) {
        SDcard.vwd()->getName (  _SDcard_Line, sizeof(_SDcard_Line) ) ;
        String Path = _SDcard_Line ;
      } else String Path = Filename ;
      
      Serial.println ( "======  Files in :  \"" + Path + "\"  ======" ) ;
      SdFile SDfile ( Filename.c_str(), O_RDONLY ) ;
      SDfile.ls ( &Serial ) ;
      SDfile.close () ;   
      Serial.println ("=====================================") ;
    }
    
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void Dir2 ( String Path = "/" ) {
      if ( !_Card_Present ) return ;
      
      SDcard.chdir ( Path.c_str() ) ;
      Serial.println ( "======  Files in :  \"" + Path + "\"  ======" ) ;
      while ( SDfile.openNext ( SDcard.vwd(), O_READ )) {
        if ( SDfile.isFile () ) {
          SDfile.printName ( &Serial ) ;
          Serial.print ( '\t' ) ;
          SDfile.printFileSize ( &Serial ) ;
          Serial.print ( '\t' ) ;
          SDfile.printModifyDateTime ( &Serial ) ;
          Serial.println () ;
          SDfile.close () ;
        }
        else SDfile.close();
      }
      Serial.println ("=====================================") ;
    }

    
    // **************************************************    
    // **************************************************    
    bool Delete ( String Filename ) {
      //Serial.println ( "===== Delete " + Filename ) ;
      //SdFile SDfile ( Filename.c_str(), O_RDWR ) ;
      SDfile.open ( Filename.c_str(), O_RDWR ) ;
      if ( SDfile.isOpen() ) {
        bool Result = SDfile.remove () ;
        SDfile.close () ;

        if ( Result ) {
          Serial.println ( "=============== File removed "  + Filename ) ;
        }
        else {
          Serial.println ( "ERROR: file not found " + Filename  ) ;        
        }
      }
      Serial.println ( "Removing File, doesn't exists " + Filename ) ;
    }
    
    
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
/*
    void Get_Ordered_DirList ( String Path = "/", String Ends = "", bool Reversed=false ) {
      if ( !_Card_Present ) return ;
      
      SDcard.chdir ( Path.c_str() ) ;
      int i = 0 ;
      while ( SDfile.openNext ( SDcard.vwd(), O_READ ) && ( i < _FileList_Max_N ) ) {
        if ( SDfile.isFile () ) {
          String Line ;
          SDfile.getName (  _SDcard_Line, sizeof(_SDcard_Line) ) ;
          SDfile.close () ;

          Line += String ( _SDcard_Line ) ;
          Line += "&emsp;&emsp;[" + String ( SDfile.fileSize() ) + "]" ;       

          dir_t dir;
          if ( SDfile.dirEntry ( &dir ) ) {
          uint16_t FileDate = dir.lastWriteDate ;
          char CharLine [30] ;
          sprintf ( CharLine, "&emsp;&emsp;%04d-%02d-%02d %02d:%02d:%02d", 
                              FAT_YEAR(FileDate), FAT_MONTH(FileDate), FAT_DAY(FileDate), 
                              FAT_HOUR(FileDate), FAT_MINUTE(FileDate), FAT_SECOND(FileDate) ) ;
          Line += String ( CharLine ) ;
          }

          _FileList[i] = Line ;
          i++ ;
        }
        else SDfile.close();
      }
      _FileList_Len = i ;
      MySort ( _FileList, _FileList_Len, Reversed ) ;
    }
*/

    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void Dump ( String Filename ) {
      if ( !_Card_Present ) return ;
      
      Serial.println ( "===== Dump :  " + Filename + "  ======" ) ;
      SdFile SDfile ( Filename.c_str(), O_RDONLY ) ;
      if ( SDfile.isOpen() ) {
        int n ;
        while ( ( n = SDfile.fgets ( _SDcard_Line, sizeof(_SDcard_Line) ) ) > 0 ) {
          Serial.print ( _SDcard_Line ) ;
        }
        SDfile.close () ;
        Serial.println ( "\n===============" ) ;
      } 
      else {
        Serial.println ( "ERROR: File not found." ) ;
      }
    }
    
      

    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    void MakeDir ( String NewPath ) {
      if ( !_Card_Present ) return ;
      
      //String Path = "/" ;
      //SDcard.chdir ( Path, true ) ;
      // vwd= "working Directory"
      SDfile.mkdir ( SDcard.vwd(), NewPath.c_str(), true ) ;
    }
  
    // Receiver_SDfat ********************************************************
    // De file wordt verondersteld een tab-delimited csv-file te zijn
    // De eerste kolom bevat het aatal seconden sinds de start van deze file
    // returns de last genoteerde seconde teller
    //         -1 als file niet bestaat
    // ***********************************************************************
    int Get_LastTime_In_File ( String Filename ) {
      ifstream sdin ( Filename.c_str() ) ;
      String Last_Line ;

      while ( sdin.getline ( _SDcard_Line, sizeof(_SDcard_Line), '\n') || sdin.gcount()) {
        if (sdin.fail()) sdin.clear(sdin.rdstate() & ~ios_base::failbit);
        else {
          if ( _SDcard_Line[0] != 0 ) {
            Last_Line = _SDcard_Line;
          }
        }
      }
      int x1 = Last_Line.indexOf ( '\t' ) ;
      if ( x1 >= 0 ) {
        Last_Line = Last_Line.substring ( 0, x1 ) ;
        x1 = Last_Line.toInt() ;
      }
      return x1 ;
    }
  
  
    // Receiver_SDfat ********************************************************
    // ***********************************************************************
    int Get_Last_File_Number ( String Path = "/" ) {
      int    Last_FileNr = 0 ;
      String Filename ;

      SDcard.chdir ( Path.c_str() ) ;
      while ( SDfile.openNext ( SDcard.vwd(), O_READ ) ) {
        if ( SDfile.isFile () ) {
          SDfile.getName (  _SDcard_Line, sizeof(_SDcard_Line) ) ;
          Filename = _SDcard_Line ;
//Serial.println ( "found file: " + Filename ) ;
          if ( Filename.startsWith ( this->_Data_Filename_Prefix ) ) {
//Serial.println ( "   found file: " + this->_Data_Filename_Prefix ) ;
            int FileNr = Parse_FileNr ( Filename ) ;
            if ( FileNr > Last_FileNr ) Last_FileNr = FileNr ;
          }
        }
        SDfile.close () ;
      }
      
      return Last_FileNr ;
    }

  
  
};


#endif
