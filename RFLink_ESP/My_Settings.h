//
// Version 0.1,  08-08-2019, SM

#ifndef My_Settings_h
#define My_Settings_h   0.1

#include <ArduinoJson.h>

#ifdef FileSystem_SPIFFS
  #include "FS_support.h"
#endif

#include "html_templates.h"

String _My_Settings_Filename = "/Settings.h" ;

DynamicJsonDocument _My_Settings_Buffer ( 2000 ) ;   // ArduinoJson V6
//StaticJsonDocument  <2000> _My_Settings_Buffer ;   // ArduinoJson V6




// ***********************************************************************************
// ***********************************************************************************
class _My_Settings_Class {
  public :
    JsonObject  DocumentRoot ;  //= _My_Settings_Buffer.as<JsonObject>() ;

    // ***********************************************************************
    // ***********************************************************************
    _My_Settings_Class () {
    }
    
    void Setup () {
      //*   DIT LIJKT ME ONZIN HIER  IS WEL DEGELIJK NODIG !!!    
#ifdef FileSystem_SPIFFS
      #ifdef ESP32
        SPIFFS.begin ( true ) ;  // format if no filesystem yet
      #else
        if ( ! SPIFFS.begin ( ) ) {
          SPIFFS.format () ;
        }
        SPIFFS.begin ( ) ;
      #endif

      Serial.println ( "===== Read_Settings from file = " + _My_Settings_Filename ) ;
      fs::File file = SPIFFS.open ( _My_Settings_Filename, "r" ) ;
      
      if ( file ) {
        DeserializationError error = deserializeJson ( _My_Settings_Buffer, file );
        if ( error )
          Serial.println( "Failed to read file JJJJJaaaa, using default configuration" + _My_Settings_Filename );
        file.close () ;
      }
      else {
        Serial.println ( "ERROR: file not found = " + _My_Settings_Filename ) ;
      }

      JsonObject  DocumentRoot = _My_Settings_Buffer.as<JsonObject>() ;
#endif

    }

/*    
    // ***********************************************************************
    // ***********************************************************************
    void Create_Sensors ( ) {
      for ( JsonPair KeyValue : DocumentRoot ) {
        String      Key   = String ( KeyValue.key().c_str() ) ;
        JsonVariant Value = KeyValue.value () ;
        
        if ( Key == "Sensor_Blauwe_Engel" ) {
          Sensors.Add ( new _Sensor_Blauwe_Engel  () ) ;
        } 
      }
    }
    
//*/

    // ***********************************************************************
    // ***********************************************************************
    void Remove ( String Key ) {
      DocumentRoot.remove ( Key ) ;
    }

    // ***********************************************************************
    // ***********************************************************************
    void Store_Settings ( String Filename = _My_Settings_Filename ) {
#ifdef FileSystem_SPIFFS
      fs::File file = SPIFFS.open ( Filename, "w" ) ;
      if ( serializeJsonPretty ( _My_Settings_Buffer, file ) == 0 ) {
         Serial.println ( "Failed to write to file " + Filename ) ;
      }
      else UnStored_Changes = false ;
      file.close () ;
#endif
    }

    // ***********************************************************************
    // ***********************************************************************
    void Set_Unstored_Changes () {
      UnStored_Changes = true ;
    }

    // ***********************************************************************
    // ***********************************************************************
    /*
    void Store_Changes ( String Filename = _My_Settings_Filename ) {
      if ( UnStored_Changes ) {
        this -> Store_Settings () ;
        UnStored_Changes = false ;
      }
    }
*/

    // ***********************************************************************
    // ***********************************************************************
    void Create_WebPage ( String Filename = "/Settings.html" ) {
#ifdef FileSystem_SPIFFS
      Serial.print ( "\n      Creating Settings Webpage  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ IP = " ) ;
      Serial.println ( WiFi.localIP() ) ;

      fs::File file = SPIFFS.open ( Filename, "w" ) ;
      if ( ! file ) {
        Serial.println ( "EEERRROOORRRRRR" ) ;
        return ;
      }

      String Line = "" ;
      Line += String ( FPSTR(HTML_Sensor_Settings_Begin) ) ;
//      Line += "<h2><img src=\"favicon.ico\" /> MiRa Sensors </h2>\n" ;
//      Line += "<a href=\"index.html\" target=\"_self\" >StartPage</a><br><br>\n" ;
//      Line += "<form action=\"/Settings_Page.php\"> \n" ;
      file.print ( Line ) ;

      // *************************************************     
      // omdat My_Settings array gewijzigd kan zijn, 
      // moet er onieuw een JSON object van gemaakt worden
      // *************************************************     
      JsonObject  DocumentRoot = _My_Settings_Buffer.as<JsonObject>() ;
      //JsonVariant Variant      = _My_Settings_Buffer.to<JsonVariant>();

      for ( JsonPair KeyValue : DocumentRoot ) {
        String Key = String ( KeyValue.key().c_str() ) ;
//Serial.println ( "KEY " + Key ) ;        
        //const char* key = p.key;
        JsonVariant Value = KeyValue.value () ;
        
        // ************************************************
        // ************************************************
        if ( Value.is<bool>() ) {
          bool Value =  (bool)_My_Settings_Buffer [ Key ] ;
          
          Line =  "<label><input type=\"checkbox\" name=\"" ;
          Line += Key ;
          Line += "\" " ;
          if ( Value ) Line += "checked" ;
          Line += "> " ;
          Line += Key ;
          Line += "</label><br>" ;
          file.println ( Line ) ;
//Serial.println ( Line ) ;          
        }

        // ************************************************
        // ************************************************
        //else if ( Value.is<signed int>() ) {
        //  Serial.println ( "INTTT" ) ;
        //}

        // ************************************************
        // ************************************************
        else {
          String Value = _My_Settings_Buffer [ Key ].as<String>();
          Line = "<input type=\"text\" name=\"" ;
          Line += Key ;
          Line += "\" value=\"" ;
          if ( Key.startsWith ( "$" ) ) Line += "********" ;
          else                          Line += Value ;
          Line += "\"> " ;
          Line += Key ;
          Line += "<br>" ;
          file.println ( Line ) ;
//Serial.println ( Line ) ;          
        }

      }
      
      Line =  "<br><label><input type=\"checkbox\" name=\"All_Default_Values\">All Default Values</label><br>" ;
      file.print ( Line ) ;
      Line = "<br><input type=\"submit\" value=\"Submit\">\n\
</form>\n\
</div>\n\
</body>\n\
</html>" ;
      file.print ( Line ) ;
      
      file.close () ;

//File_System.Dump ( Filename ) ;
#endif
    }
    
    // ***********************************************************************
    // ***********************************************************************
    String Read_String ( String Key ) {
      String Line = _My_Settings_Buffer [ Key ].as<String>();
      if ( Line == "null" ) Line = "" ;
      return Line ;
    }

    // ***********************************************************************
    // ***********************************************************************
    String Get_Set_Default_String ( String Key, String Default, bool Force = false ) {
      String Value = _My_Settings_Buffer [ Key ].as<String>();
Serial.println ( "Get_Set_Default_String  ::: " + Value + "::: " + Default + "  ;;;  " + String (Force) )  ;     
      if ( Force || ( Value == "null" ) ) {
        _My_Settings_Buffer [ Key ] = Default ;
        UnStored_Changes = true ;
        return Default ;
      }
      return Value ;
    }
 
 
    // ***********************************************************************
    // ***********************************************************************
    int Read_Int ( String Key ) {
//Serial.println ( "999999999999999999999999999999999999999999999999  " + Key + String((int)_My_Settings_Buffer [ Key ]|-1))  ;      
      return (int)_My_Settings_Buffer [ Key ] ; //| -1 ;
    }

    // ***********************************************************************
    // ***********************************************************************
    int Get_Set_Default_Int ( String Key, int Default, bool Force = false ) {
      int Value = _My_Settings_Buffer [ Key ] | Default ;
      if ( Force || ( Value == Default ) ) {
        _My_Settings_Buffer [ Key ] = Default ;
        UnStored_Changes = true ;
        return Default ;
      }
      return Value ;
    }
 
    // ***********************************************************************
    // ***********************************************************************
    bool Read_Bool ( String Key ) {
      return (bool)_My_Settings_Buffer [ Key ] ;
    }
    
    
    
  // ***********************************************************************************
  private :
  // ***********************************************************************************
    bool UnStored_Changes = false ;
}; 

// ***********************************************************************************
// Create an instance
// ***********************************************************************************
_My_Settings_Class    Settings ;


/*
#include "Receiver_Serial.h"
// ***********************************************************************************
// ***********************************************************************************
void Settings_Setup () {
  int Baudrate = Settings.Read_Int ( "RS232-Baudrate" ) ;
  Serial_Setup ( Baudrate ) ;
  Serial.println ( "Settings.Baudrate = " + String ( Baudrate ) ) ;

  File_System.Dump ( _My_Settings_Filename ) ;
  Settings.Create_WebPage () ;
  
  //Settings.Create_Sensors () ;
  
}
//*/


#endif