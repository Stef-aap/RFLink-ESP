// ***********************************************************************************
// Version 1.6   17-04-2020, SM
//    - property _Topic  vervangen door global MQTT_Topic
//    - several subtopics derived from MQTT_Topic
//
// Version 1.5   12-04-2020, SM
//    - Method Publish_Without_ added
//
// Version 1.4   29-02-2020, SM
//    - dynamisch wijzigen van broker address, bijv na setuo:
//        if ( WiFi.localIP()[2] == 0 ) MQTT_Broker_IP = "192.168.0.18" ;
//
// Version 1.3   22-02-2020, SM
//    - support for 2 broker addresses
//
// Version 1.2   15-02-2020, SM
//    - SOCKET_TIMEOUT in PubSubClient.h toch maar verlaagd (lijkt beter te werken)
//        //#define MQTT_SOCKET_TIMEOUT   600
//        #define MQTT_SOCKET_TIMEOUT   15

// Version 1.1   15-10-2019, SM
//    - WifiClient vanuit Sensor_Wifi.h naar hier gebracht
//
// Version 1.0   24-09-2019, SM
//    - ReConnect called in loop()
//    - ReConnect doesn't need to publish anymore
//
// Version 0.9   06-08-2019, SM
//    - test wifi available removed
//    - reset on no conection removed
//
// Version 0.8   19-07-2019, SM
//    - MQTT Callback overgeheveld naarSensor_Receiver_2.h  (nu kunnen meerdere Sensors/Receivers MQTT ontvangen )
//
// Version 0.7   29-05-2019, SM
//    - doet geen restart als geen connectie kan worden gerealiseerd en Allow_Wifi_Not_Found == true
//    - Reageert op MQTT message: { "Loop_Time" : 2 }
//    - MQTT Callback ingebouwd
//
// Version 0.6   04-03-2019, SM
//    - Multiple Broker compleet verwijderd
//
// Version 0.5   02-01-2019, SM
//    - External_Watchdog_Disarm added
//
// Version 0.4, 20-10-2018, SM, checked by ..
//    - toch maar weer mqtt reconnect toegevoegd
//
// Version 0.3, 10-10-2018, SM, checked by ..
//    - even terug naar single broker
//
// Version 0.2, 27-06-2018, SM, checked by ..
//    - increased time to get an reset from 2 to 10 seconds
//
// Version 0.1, 30-03-2018, SM, checked by ..
//    - initial version
// ***********************************************************************************

#ifndef Receiver_MQTT_h
#define Receiver_MQTT_h   1.6

#include "Receiver_Base.h"
#include "PubSubClient.h"       // MQTT client 

#define Broker_Port   1883
//WiFiClient espClient ;
WiFiClient   _MQTT_WifiClient;

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_MQTT : public _Receiver_BaseClass {

  public:
    PubSubClient *MyMQTT           ;
    //int          Broker_Index  = 0 ;

    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _Receiver_MQTT ( int Dummy ) {
      this -> Default_Settings () ;
      this -> Constructor_Finish () ;
    }  
      
    _Receiver_MQTT ( String Topic = "MQTT_Receiver/Test", bool Make_Device_Active = true ) {  //, bool Keep_Connection = false ){
      MQTT_Topic = Topic ;
      this->Device_Active = Make_Device_Active ;
      this -> Constructor_Finish () ;
    }  

    // ***********************************************************************
    // ***********************************************************************
    void Constructor_Finish () {
      MQTT_Topics_Append ( MQTT_Topic ) ;
      
//      _Keep_Connection = Keep_Connection ;
////      _MQTT_WifiClient     = espClient ;
      
      Version_Name = "V" + String ( Receiver_MQTT_h ) + "  ======  Receiver_MQTT.h" ;
      Serial.println ( "CREATE    " + Version_Name ) ;
      
      /*
      Help_Text =  "_Receiver_MQTT   MQTT ( Topic, Keep_Connection ) ;\n\
_Receiver_MQTT   MQTT ( Topic ) ;\n\
_Receiver_MQTT   MQTT () ;\n\
Topic = " ;
      Help_Text += String ( MQTT_Topic ) ;
*/
      Help_Text = "Topic = " + MQTT_Topic ;

      if ( ! this->Device_Active ) return ;
      
      High_Priority_Loop  = 2 ;
     
      MyMQTT           = new PubSubClient ( _MQTT_WifiClient ) ;
//      MyMQTT->setServer ( MQTT_Broker_IPs [ Broker_Index ].c_str(), Broker_Port ) ;


      MyMQTT -> setServer ( MQTT_Broker_IP.c_str(), Broker_Port ) ;
      this->_MQTT_Broker_IPx = MQTT_Broker_IP ;
      _MQTT_ID = String ( WiFi.macAddress () ) ;
      
      // *****************************************************
      // Omdat het geen callback is maar een event, moet dit zo moeilijk
      // https://hobbytronics.com.pk/arduino-custom-library-and-pubsubclient-call-back/
      // *****************************************************
      //#ifndef ESP32
//        MyMQTT->setCallback ( [this] (char* topic, byte* payload, unsigned int length) { this->Receive_Callback(topic, payload, length); });
MyMQTT->setCallback ( _MQTT_Callback_Wrapper ) ;
//((_Receiver_MQTT*)My_MQTT_Client)->MyMQTT->setCallback ( _MQTT_Callback_Wrapper ) ;
     //#endif
      // *****************************************************
      // *****************************************************

      
      //Serial.println ( "Trying to connect toe MQTT: " + Broker_IPs [ Broker_Index ] ) ;
      Serial.print   ( "                                                          IP = " ) ;
      Serial.print   ( WiFi.localIP() ) ;
      Serial.print   ( "         Broker = " ) ;
      Serial.println ( MQTT_Broker_IP ) ;
      
      //_Subscription_Out = _Topic + "_" ;          
      _Subscription_Out = MQTT_Topic + "_" ;          
      _LWT              = "\"$$Dead " + _MQTT_ID + "\"" ;
      _ALIVE            = "\"$$Alive " + _MQTT_ID + "\"" ;
      _Error_Time       = 0 ;

      _Debug_MQTT_Available = true ;
    }
    
    // **********************************************************************************************
    // **********************************************************************************************
    void Default_Settings ( bool Force = false  ) {
      if ( ! this->Device_Active ) return ;
      MQTT_Topic     = Settings.Get_Set_Default_String ( "MQTT Topic"    , "huis/verdieping/kamer/ding", Force ) ;
      MQTT_Broker_IP = Settings.Get_Set_Default_String ( "MQTT Broker-IP", "192.168.0.18"              , Force ) ;
    }
    
    // **********************************************************************************************
    // **********************************************************************************************
    bool Check_Modified_Settings () {
      if ( ! this->Device_Active ) return false;

      bool   Restart   = false ;

      String New_Value ;
      String Topic  = Settings.Read_String ( "MQTT Topic"     ) ;
      String Broker = Settings.Read_String ( "MQTT Broker-IP" ) ;
      
      for ( int i=0; i<My_Webserver.args(); i++ ) {
        New_Value = My_Webserver.arg(i) ;
        
        if ( My_Webserver.argName(i) == "MQTT Topic" ) {
          if ( New_Value != Topic ) {
            _My_Settings_Buffer [ "MQTT Topic" ] = New_Value ;
            Restart = true ;
          }
        }
        else if ( My_Webserver.argName(i) == "MQTT Broker-IP" ) {
          if ( New_Value != Broker ) {
            _My_Settings_Buffer [ "MQTT Broker-IP" ] = New_Value ;
            Restart = true ;
          }
        }  
      }
      return Restart ;
    }

    // ***********************************************************************
    // ***********************************************************************
    void loop(){
      if ( ! this->Device_Active ) return ;
      
      if ( ! MyMQTT -> connected() ) {
        // **********************************************************
        // Controleer of het Broker address (dynamisch) is gewijzigd
        // **********************************************************
        if ( MQTT_Broker_IP != this->_MQTT_Broker_IPx ) {
          MyMQTT -> setServer ( MQTT_Broker_IP.c_str(), Broker_Port ) ;
          this->_MQTT_Broker_IPx = MQTT_Broker_IP ;
          this -> ReConnect () ;
          _ReConnect_Start = millis () ;
          return ;
        }
        
        if ( ( _ReConnect_Start <= 0 ) || ( ( millis() - _ReConnect_Start ) > 2000 ) ) {
          this -> ReConnect () ;
          _ReConnect_Start = millis () ;
        }
      }

      MyMQTT->loop() ;
    }
 
    
    // ***********************************************************************
    // HIER MOGEN NATUURLIJK GEEN DEBUG STATEMENTS IN STAAN !!!
    // ***********************************************************************
    bool Publish ( String Topic, String Payload ) {
      return this->Publish_Without_ ( Topic + "_", Payload ) ;
    }
    bool Publish_Without_ ( String Topic, String Payload ) {
      if ( ! this->Device_Active ) return false ;

      if ( MyMQTT -> connected() ) {
        MyMQTT -> publish ( Topic.c_str(), Payload.c_str() ) ; 
        _Error_Time = 0 ;        
Serial.println ( "Published To: " + this->_MQTT_Broker_IPx + "    Topic: " + Topic ) ;
        return true;                          // SHOULD BE IMPROVED
      } 


      //Serial.println ( "No MQTT Connection: " + Broker_IPs [ Broker_Index ] ) ;
      Serial.println ( "No MQTT Connection to " + this->_MQTT_Broker_IPx ) ;

      
//return true; 
/* volgens mij ook niet nodig
      if ( ReConnect ( _ALIVE ) ) {
        Serial.println ( "Alive" ) ;
        _State = 1 ;
      }
//*/
      return false;
    }   

    
    // ***********************************************************************
    // ***********************************************************************
    bool Send_Data ( String Payload ) {
      if ( ! this->Device_Active ) return false ;

Serial.println ( "    publish: " + MQTT_Topic  + " // " + Payload ) ;      
      return Publish ( MQTT_Topic, Payload ) ;
    }   


  // ***********************************************************************
  // ***********************************************************************
  private :
    int           _State       = 0        ;
    unsigned long _Error_Time  = 0        ;
//    unsigned long _State_Start = 0        ;
    String        _MQTT_ID                ;
//    int           _MQTT_Connect_Retries   ;
//    unsigned long _MQTT_Connect_Start     ;
//    unsigned long _Alive_Time             ;
//    bool          _Keep_Connection        ;
    String        _Subscription_Out       ;          
    String        _LWT                    ;
    String        _ALIVE                  ; 
    unsigned long _ReConnect_Start = 0 ;
    int           _ReConnect_Count = 0 ;
    String        _MQTT_Broker_IPx ;
      

    // ***********************************************************************
    // ***********************************************************************
    bool ReConnect ( String ToPublish = "" ) {
/*
      #ifdef ESP32  
        if ( WiFi.status () != WL_CONNECTED ) return false ;
      #else
        if ( wifiMulti.run() != WL_CONNECTED ) return false ;
      #endif
//*/

Serial.println ( "try reconnect 0 ") ;                               
      //if ( MyMQTT->connect ( _MQTT_ID.c_str(), MQTT_User, MQTT_Pwd, 
      if ( MyMQTT -> connect ( _MQTT_ID.c_str(),
                              _Subscription_Out.c_str(), 1, 1, _LWT.c_str() )) {
Serial.println ( "try reconnect 1 ") ;                               
Set_Signal_LED ( 4, 100, 100 ) ;
        for ( int i=0; i<MAX_MQTT_TOPICS; i++ ) {
          if ( MQTT_Topics[i].length() > 0 ) {
            MyMQTT -> subscribe ( MQTT_Topics[i].c_str(), MQTTQOS0 ) ;
            Serial.println ( "MQTT Subsrcibe : "+ MQTT_Topics[i] ) ;
          }
          else break ;
        }
Serial.println ( "try reconnect 2 ") ;                              

        if ( ToPublish.length() > 0 ) {
          MyMQTT->publish ( _Subscription_Out.c_str(), ToPublish.c_str() );
        }

//        Set_Signal_LED ( 3, 300, 100 ) ;
        
        return true ;
      }

//      Set_Signal_LED ( 3, 100, 100 ) ;
Serial.println ( "try reconnect FIN, niet gelukt ") ;                               
      
      // **************************************
      //  if more than one broker, toggle between brokers if reconnect necessary
      // **************************************
      if ( MQTT_Broker_IP2.length() > 5 ) {
        int Mode = _ReConnect_Count % 10 ;
        if ( Mode == 5 ) {
          MyMQTT -> setServer ( MQTT_Broker_IP2.c_str(), Broker_Port ) ;
          this->_MQTT_Broker_IPx = MQTT_Broker_IP2 ;
          _ReConnect_Count = 5 ;
Serial.print ( "MQQQQQQQQQQQQQQQQQQQQQQQQQQQQT-broker : " + MQTT_Broker_IP2 ) ;         
        }      
        else if ( Mode == 0 ){
          MyMQTT -> setServer ( MQTT_Broker_IP.c_str(), Broker_Port ) ;
          this->_MQTT_Broker_IPx = MQTT_Broker_IP ;
          _ReConnect_Count = 0 ;
Serial.print ( "MQQQQQQQQQQQQQQQQQQQQQQQQQQQQT-broker : " + MQTT_Broker_IP ) ;         
        }
      }
      _ReConnect_Count += 1 ;
      
      
      return false ;
    }
    
};



#endif
