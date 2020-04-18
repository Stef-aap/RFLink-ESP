// ***********************************************************************************
// Version 0.3, 24-07-2019, SM, checked by ...
//
// Version 0.2, 18-07-2019, SM, checked by ...
//    - LED detectie dmv falling edges counting
// Version 0.1, 17-07-2019, SM, checked by ...
//    - initial version
// ***********************************************************************************

// SSD 1306 OLED 128*64:  https://lastminuteengineers.com/oled-display-arduino-tutorial/
// https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/

#ifndef Sensor_Blauwe_Engel_h
#define Sensor_Blauwe_Engel_h   0.2

//#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define _Boiler_Period    15 * 60 * 1000        


//Adafruit_SSD1306 Boiler_Display ( -1 ) ;  // reset pin not used on 4-pin OLED module
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 Boiler_Display ( SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET ) ;


// ***********************************************************************************
// interrupt routine, detecteert a falling edge van de Start_Knop
// ***********************************************************************************
//int           _Sensor_Blauwe_Engel_Knop_State          = 0 ;
//unsigned long _Sensor_Blauwe_Engel_Falling_Start       = 0 ;
int           _Sensor_Blauwe_Engel_StartKnop_Count = 0 ;
unsigned long _Sensor_Blauwe_Engel_StartKnop_Start = 0 ;
static void _Sensor_Blauwe_Engel_Knop_Falling () ICACHE_RAM_ATTR ;
static void _Sensor_Blauwe_Engel_Knop_Falling () {
  _Sensor_Blauwe_Engel_StartKnop_Count +=  1 ;
//Serial.println ( "iiiiiiiiiiiiiii") ;
/*
  if ( _Sensor_Blauwe_Engel_Knop_State == 0 ) {
    _Sensor_Blauwe_Engel_Falling_Start = millis () ;
    _Sensor_Blauwe_Engel_Knop_State    =  1 ;
Serial.println ( "iiiiiiiiiiiiiii") ;
  }
*/
}

// ***********************************************************************************
// interrupt routine, detecteert het aan zijn van de WarmWater-LED 
// ***********************************************************************************
int           _Sensor_Blauwe_Engel_LED_Count = 0 ;
unsigned long _Sensor_Blauwe_Engel_LED_Start = 0 ;
static void _Sensor_Blauwe_Engel_LED_Falling () ICACHE_RAM_ATTR ;
static void _Sensor_Blauwe_Engel_LED_Falling () {
  _Sensor_Blauwe_Engel_LED_Count +=  1 ;
//Serial.println ( "OOOOOOOOOOOOOOOOOOOO") ;
}


      const int _State_Boiler_Off           = 0 ;
      const int _State_Boiler_ON            = 5 ;

      const int _State_Boiler_Knop_Rest     = 0 ;
      const int _State_Boiler_Knop_Press    = 1 ;
      const int _State_Boiler_Knop_Pressed  = 2 ;
      const int _State_Boiler_Knop_Released = 3 ;

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_Blauwe_Engel : public _Sensor_BaseClass {

  public:
    //#define   ON    true
    //#define   OFF   false

    #define  _BOILER_KNOP_PRESS      HIGH
    #define  _BOILER_KNOP_RELEASE    LOW
    
    #define  _StartKnop_PRESS_TIME   100 
    #define  _BOILER_PRESS_TIME      500
    #define  _BOILER_RELEASE_TIME    1000
    #define  _MAX_BOILER_TRY         4

      

    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _Sensor_Blauwe_Engel ( char* Dummy ) {
      this -> Default_Settings () ;
      this -> Constructor_Finish () ;
    }

    _Sensor_Blauwe_Engel ( int Start_Knop=13, int Boiler_Knop=15, int Boiler_LED=27, int User_LED=26, int OLED_Address=0x3C ) { 
      _Start_Knop_Pin  = Start_Knop ;
      _Boiler_Knop_Pin = Boiler_Knop ;
      _Boiler_LED_Pin  = Boiler_LED ;
      _User_LED_Pin    = User_LED ;
      _OLED_Address    = OLED_Address ;
      this -> Constructor_Finish () ;
    }  


    // ***********************************************************************
    // ***********************************************************************
    void Constructor_Finish () {
      Version_Name = "V" + String ( Sensor_Blauwe_Engel_h ) + "   Sensor_Blauwe_Engel.h" ;
      Serial.println ( "CREATE    " + Version_Name ) ;

      Help_Text = "    Boiler besturing van de Blauwe Engel\n\
    Na het drukken op de knop, wordt de warmwater-voorziening voor een halfuur ingeschakeld.\n\
    Op het display wordt de gebruikte hoeveelheid water weergegeven.\n\
_Sensor_Blauwe_Engel ( int Chan_0, int Chan_1 )\n\
" ;
      MQTT_Callback_Topic = "huis/verdieping0/meterkast/SlimmeEnergieMeter" ;
      //MQTT_Callback_Topic = "huis/ergens/Boiler_Test" ;
      //MQTT_Callback_Topic = "camper/verdieping0/leefruimte/Sensors_02_" ;
      //MQTT_Callback_Topic = "huis/verdieping0/kamer1/CO2_Sensor_REF2_" ;

      _JSON_Short_Header = "Blauwe_Engel_LED" ;
      _JSON_Short_Header += "\t" ;
      _JSON_Short_Header += "ToGo" ;
      _JSON_Short_Header += "\t" ;
      _JSON_Short_Header += "Water" ;
      _JSON_Short_Header += "\t" ;
      _JSON_Short_Header += "Boiler_State" ;
      _JSON_Short_Header += "\t" ;
      _JSON_Short_Header += "Boiler_State_Machine" ;
      _JSON_Short_Header += "\t" ;
      _JSON_Short_Header += "Knop" ;
      _JSON_Short_Header += "\t" ;
      _JSON_Short_Header += "Knop_State_Machine" ;
      _JSON_Short_Header += "\t" ;
    }
    
    
    // ***********************************************************************
    // ***********************************************************************
    void setup () {
      pinMode ( _Start_Knop_Pin , INPUT  ) ;
      pinMode ( _Boiler_LED_Pin , INPUT  ) ;
      pinMode ( _Boiler_Knop_Pin, OUTPUT ) ;
      pinMode ( _User_LED_Pin   , OUTPUT ) ;
      attachInterrupt ( _Start_Knop_Pin, _Sensor_Blauwe_Engel_Knop_Falling, FALLING ) ;
      attachInterrupt ( _Boiler_LED_Pin, _Sensor_Blauwe_Engel_LED_Falling , FALLING ) ;
      digitalWrite ( _Boiler_Knop_Pin, _BOILER_KNOP_RELEASE ) ;
      
      // ***********************************
      // setup the OLED display
      // ***********************************
      // 128 x 64 pixel display
      //#if (SSD1306_LCDHEIGHT != 64)
      //#error("Height incorrect, please fix Adafruit_SSD1306.h!");
      //#endif
      // initialize and clear display
      //Boiler_Display.begin ( SSD1306_SWITCHCAPVCC, _OLED_Address ) ;
      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
      if ( !Boiler_Display.begin ( SSD1306_SWITCHCAPVCC, 0x3C ) ) { 
        Serial.println ( "SSD1306 allocation failed" ) ;
        _Boiler_Display_Available = false ;
      }
      else _Boiler_Display_Available = true ;
      
      if ( _Boiler_Display_Available ) {
        Boiler_Display.clearDisplay();
        Boiler_Display.display();
      }
      Serial.println ( "===============================: " + String ( _Boiler_Display_Available));
      Display_Update () ;  
      
      
      // ***********************************
      // Fixed strings fro the BOM Display
      // ***********************************
      _TM1638_Regel [0] = String ( " 684 20.4" ) ;       //  CO2 [ppm]  // Temperature [Celsius]
      _TM1638_Regel [1] = String ( "45  1009"  ) ;       //  RH [%]     // Pressure [mbar]
      _TM1638_Regel [2] = String ( " 20   40"  ) ;       //  ppm2_5     // ppm10
      _TM1638_Regel [4] = String ( "12 1 27.5" ) ;       //  Time left [min]     //  Water [Liter]
      _TM1638_Regel [5] = String ( "78 2 27.5" ) ;       //  TBoiler [ Celsius]  //  Water [Liter]
      _TM1638_Regel [6] = String ( "SENS  03"  ) ;       //  Programm Name and Version
      //_TM1638_Regel [7] = String ( _Main_Name  ) ;       //  Programm Name and Version
      
      //My_Prefs.putChar ( "Reboot_Options", Reboot_Options ) ;

    }
    
    // **********************************************************************************************
    // **********************************************************************************************
    void Default_Settings ( bool Force = false ) {
      _Start_Knop_Pin  = Settings.Get_Set_Default_Int  ( "Bengel Start Knop" , 13  , Force ) ;
      _Boiler_Knop_Pin = Settings.Get_Set_Default_Int ( "Bengel Boiler Knop" , 16  , Force ) ;
      _Boiler_LED_Pin  = Settings.Get_Set_Default_Int ( "Bengel Boiler LED"  , 27  , Force ) ;
      _User_LED_Pin    = Settings.Get_Set_Default_Int ( "Bengel User LED"    , 26  , Force ) ;
      _OLED_Address    = Settings.Get_Set_Default_Int ( "Bengel OLED Address", 0x3C, Force ) ;
    }

    // **********************************************************************************************
    bool Check_Modified_Settings () {
      bool   Restart   = false ;
      int    New_Value_Int ;

      int    Start_Knop   = Settings.Read_Int ( "Bengel Start Knop"   ) ;
      int    Boiler_Knop  = Settings.Read_Int ( "Bengel Boiler Knop"  ) ;
      int    Boiler_LED   = Settings.Read_Int ( "Bengel Boiler LED"   ) ;
      int    User_LED     = Settings.Read_Int ( "Bengel User LED"     ) ;
      int    OLED_Address = Settings.Read_Int ( "Bengel OLED Address" ) ;
      
      for ( int i=0; i<My_Webserver.args(); i++ ) {
        New_Value_Int = My_Webserver.arg(i).toInt() ;
        
        if ( My_Webserver.argName(i) == "Bengel Start Knop" ) {
          if ( New_Value_Int != Start_Knop ) {
            _My_Settings_Buffer [ "Bengel Start Knop" ] = New_Value_Int ;
            Restart = true ;
          }
        }
        else if ( My_Webserver.argName(i) == "Bengel Boiler Knop" ) {
          if ( New_Value_Int != Boiler_Knop ) {
            _My_Settings_Buffer [ "Bengel Boiler Knop" ] = New_Value_Int ;
            Restart = true ;
          }
        }
        else if ( My_Webserver.argName(i) == "Bengel Boiler LED" ) {
          if ( New_Value_Int != Boiler_LED ) {
            _My_Settings_Buffer [ "Bengel Boiler LED" ] = New_Value_Int ;
            Restart = true ;
          }
        }
        else if ( My_Webserver.argName(i) == "Bengel User LED" ) {
          if ( New_Value_Int != User_LED ) {
            _My_Settings_Buffer [ "Bengel User LED" ] = New_Value_Int ;
            Restart = true ;
          }
        }
        else if ( My_Webserver.argName(i) == "Bengel OLED Address" ) {
          if ( New_Value_Int != OLED_Address ) {
            _My_Settings_Buffer [ "Bengel OLED Address" ] = New_Value_Int ;
            Restart = true ;
          }
        }
      }
      return Restart ;
    }

    
    // ***********************************************************************
    // ***********************************************************************
    void loop () {
      // ***********************************************************
      // Sla periodiek het aantal pulsen van de Boiler LED op
      // ***********************************************************
        if ( ( millis() - _Sensor_Blauwe_Engel_LED_Start ) > 200 ) {
          _Sensor_Blauwe_Engel_LED_Start = millis() ;
          _LED_Pulses  = _Sensor_Blauwe_Engel_LED_Count ;
          _Sensor_Blauwe_Engel_LED_Count = 0 ; 
          //_LED_Sum  += _LED_Pulses ;
          //_LED_NSum += 1 ;
//Serial.println ( _LED_Sum ) ;

char buffer [ 40 ] ;
snprintf ( buffer, sizeof(buffer), "% 9.1f", (millis()/1000.0) ) ;
_TM1638_Regel [3] = String ( String ( buffer ) ) ;

        }  

/*        
      // ***********************************************************
      // Lees de gebruikers Knop en kijk of de Boiler aangezet moet worden
      // ***********************************************************
      if ( _Sensor_Blauwe_Engel_Knop_State == 1 ) {
        if ( digitalRead ( _Start_Knop_Pin ) == LOW ) {
          if ( ( millis() - _Sensor_Blauwe_Engel_Falling_Start ) > 100 ) {
            _Sensor_Blauwe_Engel_Knop_State = 2 ;
            Set_Boiler ( true ) ;
          }
        }
        else {
          _Sensor_Blauwe_Engel_Knop_State = 0 ;
        }
      }
      else if ( _Sensor_Blauwe_Engel_Knop_State == 2 ) {
        delay ( 100 ) ;
        _Sensor_Blauwe_Engel_Knop_State = 0 ;
      }
*/
      
      // ***********************************************************
      // Stop the Boiler als de tijd is verstreken
      // Display hoe lang de Boiler nog aan blijft staan
      // ***********************************************************
      if ( _Boiler_Start > 0 ) {
        if ( ( millis() - _Boiler_Start ) > ( _Boiler_Period )) { 
          //Set_Boiler ( false ) ;
          _Boiler_Start = 0 ;
          _Boiler_Should_be_ON = false ;
          _Boiler_Knop_Try = 0 ;
Serial.println ( "ZET TO ZERO");              
        }

        if ( ( millis() - _Display_Update ) > ( 2 * 1000 ) ) {
          _Display_Update = millis() ;
          float Water = _W_Dag - _Water_Start ;
          unsigned long Time_ToGo ;
          if ( _Boiler_Start > 0 ) {
            Time_ToGo = ( _Boiler_Period - ( millis() - _Boiler_Start ))/1000 ;
          }
          Serial.println ( "Boiler wil be on for : " + String ( Time_ToGo ) + "  Water: " + String ( Water ) ) ;
        }

        Display_Update () ;  
      }

      // ***********************************************************
      // StartKnop state machine
      // ***********************************************************
int Count = _Sensor_Blauwe_Engel_StartKnop_Count ; 

      
      const int  _StartKnop_OFF           = 0 ;
      const int  _StartKnop_OFF_Pressed   = 1 ;
      const int  _StartKnop_ON            = 2 ;
      const int  _StartKnop_ON_Released   = 3 ;
      switch ( _StartKnop_State_Machine ) {
        case _StartKnop_OFF : 
          // **********************************************************************
          // als StartKnop ingedrukt wordt
          // **********************************************************************
          if ( _Sensor_Blauwe_Engel_StartKnop_Count > 0 ) {
            _StartKnop_Status_Start = millis () ;
            _StartKnop_State_Machine = _StartKnop_OFF_Pressed ;
          }  
          break;

        case _StartKnop_OFF_Pressed :
          // **********************************************************************
          // als de knop lang genoeg is ingedrukt zet de Boiler aan
          // **********************************************************************
          _Sensor_Blauwe_Engel_StartKnop_Count = 0 ;
          if ( millis() - _StartKnop_Status_Start > _StartKnop_PRESS_TIME ) {
            if ( digitalRead ( _Start_Knop_Pin ) == LOW ) {
              _StartKnop_Status_Start = millis () ;
              _StartKnop_State_Machine = _StartKnop_ON ;
              _Boiler_Should_be_ON = true ;
              _Boiler_Knop_Try = 0 ;
Serial.println ( "ZET TO ZERO");              
              if ( _Boiler_Start == 0 ) {
//                _Boiler_First_Start = millis () ;
                _Water_Start = _W_Dag ;
              }
              _Boiler_Start = millis () ;
            }
            else _StartKnop_State_Machine = _StartKnop_OFF ;
          }
          break;
           
        case _StartKnop_ON :
          // **********************************************************************
          // als de knop wordt losgelaten
          // **********************************************************************
          _Sensor_Blauwe_Engel_StartKnop_Count = 0 ;
          if ( digitalRead ( _Start_Knop_Pin ) == HIGH ) {
            _StartKnop_Status_Start = millis () ;
            _StartKnop_State_Machine = _StartKnop_ON_Released ;
          }
          break;
           
        case _StartKnop_ON_Released : 
          // **********************************************************************
          // als de knop lang genoeg is losgelaten, ga weer in de ruststand
          // **********************************************************************
          _Sensor_Blauwe_Engel_StartKnop_Count = 0 ;
          if ( millis() - _StartKnop_Status_Start > _StartKnop_PRESS_TIME ) {
            if ( digitalRead ( _Start_Knop_Pin ) == LOW ) {
              _StartKnop_Status_Start = millis () ;
              _StartKnop_State_Machine = _StartKnop_ON;
            }
            else _StartKnop_State_Machine = _StartKnop_OFF ;
          }
          break;
           
        default :  //for extra safety
          _StartKnop_State_Machine = _StartKnop_OFF ;
          break;
      }
      
      // ***********************************************************
      // Boiler state machine
      // ***********************************************************
      switch ( _Boiler_Knop_State_Machine ) {
        // ******************************************************
        // start van het indrukken van de Boiler knop, registreer de startijd
        // ******************************************************
        case _State_Boiler_Knop_Press :
          digitalWrite ( _Boiler_Knop_Pin, _BOILER_KNOP_PRESS ) ;
          _Boiler_Status_Start = millis () ;
          _Boiler_Knop_State_Machine = _State_Boiler_Knop_Pressed ;
          break ;
        // ******************************************************
        // wacht tot de knop lang genoeg is ingedrukt, laat hem dan los
        // ******************************************************
        case _State_Boiler_Knop_Pressed :
          if ( millis() - _Boiler_Status_Start > _BOILER_PRESS_TIME ) {
            digitalWrite ( _Boiler_Knop_Pin, _BOILER_KNOP_RELEASE ) ;
            _Boiler_Status_Start = millis () ;
            _Boiler_Knop_State_Machine = _State_Boiler_Knop_Released ;
          }
          break ;
        // ******************************************************
        // wacht tot de knop lang genoeg is losgelaten, ga dan de status weer vergelijken
        // ******************************************************
        case _State_Boiler_Knop_Released :
          if ( millis() - _Boiler_Status_Start > _BOILER_RELEASE_TIME ) {
            _Boiler_Knop_State_Machine = _State_Boiler_Knop_Rest ;
          }
          break ;
        default :
          // ******************************************************
          // Controleer of de Boiler gelijk is aan gewenste status
          // als niet en het maximum aantal pogingen is nog niet bereikt
          // druk dan de knop nog een keer in
          // ******************************************************
          if ( Boiler_State() != _Boiler_Should_be_ON ) {
            if ( _Boiler_Knop_Try < _MAX_BOILER_TRY ) {
              _Boiler_Knop_State_Machine =  _State_Boiler_Knop_Press ; 
              _Boiler_Knop_Try += 1 ;
            }
          }                
          break ;
      }
   }

    // ***********************************************************************
    // ***********************************************************************
    void Display_Update () {
      if ( _Boiler_Display_Available ) {
        Boiler_Display.clearDisplay ();
        Boiler_Display.setCursor ( 0, 0 ) ;
        Boiler_Display.setTextSize ( 1 ) ;
        Boiler_Display.println ( "Boiler" ) ;
      }
    }    

    // ***********************************************************************
    // Get all the sampled data as a JSON string
    // ***********************************************************************
    void Get_JSON_Data () {
      float         Water     = _W_Dag - _Water_Start ;
      unsigned long Time_ToGo = 0 ;
      if ( _Boiler_Start > 0 ) {
        Time_ToGo = ( _Boiler_Period - ( millis() - _Boiler_Start ))/1000 ;
      }
      
//      Serial.println ( "pppp" + String ( _LED_Sum) + "---" + String ( _LED_NSum ) + "---" + String ( _LED_Pulses ) ) ;
/*      int LED = 0 ;
      if ( _LED_NSum > 0 ) {
        LED = _LED_Sum / _LED_NSum ;
      }
      _LED_Sum  = 0 ;
      _LED_NSum = 0 ;
*/      
      JSON_Data += " \"Blauwe_Engel_LED\":" ;
      JSON_Data += String ( _LED_Pulses ) ;
      JSON_Data += "," ;
      JSON_Data += " \"ToGo\":" ;
      JSON_Data += String ( Time_ToGo ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Water\":" ;
      JSON_Data += String ( Water ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Boiler_On\":" ;
      JSON_Data += String ( Boiler_State() ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Boiler\":" ;
      JSON_Data += String ( _Boiler_Knop_State_Machine ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Knop_Echt\":" ;
      JSON_Data += String ( digitalRead ( _Start_Knop_Pin ) ) ;
      JSON_Data += "," ;
      JSON_Data += " \"Knop\":" ;
      JSON_Data += String ( _StartKnop_State_Machine ) ;
      JSON_Data += "," ;

      JSON_Short_Data += String ( _LED_Pulses ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( Time_ToGo ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( Water ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( Boiler_State() ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( _Boiler_Knop_State_Machine ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( digitalRead ( _Start_Knop_Pin ) ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( _StartKnop_State_Machine ) ;
      JSON_Short_Data += "\t" ;
      
      Display_Update () ;  
   }

    // ***********************************************************************
    // ***********************************************************************
    void MQTT_Callback ( String Topic, String Payload, DynamicJsonDocument root ) {
      //Serial.println ( Payload ) ;
      _W_Dag = root [ "W_Dag" ] ;
//Serial.println ( "GGGGGGGGGGGGGGGGGGGGGGGG: "+ String ( _W_Dag ) ) ;
      Display_Update () ;  

    }
   
  // ***********************************************************************
  private:
  // ***********************************************************************
    int           _Start_Knop_Pin ;
    int           _Boiler_Knop_Pin  ;
    int           _Boiler_LED_Pin ;
    int           _User_LED_Pin ;
    int           _State               = 0 ;
//    unsigned long _Boiler_First_Start  = 0 ;
    unsigned long _Boiler_Start        = 0 ;
    float         _Water_Start         = 0 ;
    unsigned long _Display_Update      = 0 ;
    float         _W_Dag               = 0 ;
    int           _OLED_Address ;
    bool          _Boiler_Display_Available ;
    int           _LED_Pulses = 0 ;
    

    int           _StartKnop_State_Machine     = 0 ;  
    unsigned long _StartKnop_Status_Start      = 0 ;
    
    int           _Boiler_Knop_State_Machine   = 0 ;

    bool          _Boiler_Should_be_ON         = false ;
    unsigned long _Boiler_Status_Start         = 0 ;
    int           _Boiler_Knop_Try             = 0 ;
   
    
    // ***********************************************************************
    // ***********************************************************************
    bool Boiler_State () {
      const int Min_Pulses = 50 ;
      digitalWrite ( _User_LED_Pin, _LED_Pulses > Min_Pulses ) ;
      return _LED_Pulses > Min_Pulses ;
    }
   
    
};

#endif
