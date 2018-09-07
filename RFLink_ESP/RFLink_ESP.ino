// ****************************************************************************
// Version 2.1
//   - in the source you can select the communication mode RS232 / MQTT
//
// ****************************************************************************
#define Version  "2.1"
#define Revision  0x01
#define Build     0x01

#define Home_Automation  "MQTT"
//#define Home_Automation  "RS232"

// ****************************************************************************
// used in Raw signal
//
// original: 20=8 bits. Minimal number of bits*2 that need to have been received 
//    before we spend CPU time on decoding the signal.
//
// MAX might be a little to low ??
// ****************************************************************************
#define MIN_RAW_PULSES         26   //20  // =8 bits. Minimal number of bits*2 that need to have been received before we spend CPU time on decoding the signal.
#define MAX_RAW_PULSES        150   
// ****************************************************************************
#define MIN_PULSE_LENGTH      40   // Pulses shorter than this value in uSec. will be seen as garbage and not taken as actual pulses.
#define SIGNAL_TIMEOUT         7   // Timeout, after this time in mSec. the RF signal will be considered to have stopped.
#define RAW_BUFFER_SIZE      512   // Maximum number of pulses that is received in one go.
#define INPUT_COMMAND_SIZE    60   // Maximum number of characters that a command via serial can be.


// ****************************************************************************
// Global Variables
// ****************************************************************************
byte          PKSequenceNumber = 0 ;  // 1 byte packet counter

char          PreFix [20]              ;
unsigned long Last_Detection_Time = 0L ;
int           Learning_Mode       = 0  ;  // always start in production mode

char          pbuffer  [ 60 ] ;           // Buffer for printing data
char          pbuffer2 [ 30 ] ;
char          InputBuffer_Serial [ INPUT_COMMAND_SIZE ];   // Buffer for Seriel data

struct RawSignalStruct {                 // Raw signal variabelen places in a struct
  int           Number ;                 // Number of pulses, times two as every pulse has a mark and a space.
  int           Min    ;
  int           Max    ;
  long          Mean   ;
  unsigned long Time   ;                 // Timestamp indicating when the signal was received (millis())
  int Pulses [ RAW_BUFFER_SIZE + 2 ] ;   // Table with the measured pulses in microseconds divided by RawSignal.Multiply. (halves RAM usage)
                                         // First pulse is located in element 1. Element 0 is used for special purposes, like signalling the use of a specific plugin
} RawSignal= { 0, 0, 0, 0, 0L };
//} RawSignal={0,0,0,0,0,0L};

unsigned long Last_BitStream      = 0L    ;  // holds the bitstream value for some plugins to identify RF repeats
bool          Serial_Command      = false ;
int           SerialInByteCounter = 0     ;  // number of bytes counter 
byte          SerialInByte                ;  // incoming character value
String        Unknown_Device_ID   = ""    ;


// ***********************************************************************************
// File with the device registrations
// ***********************************************************************************
#include "RFLink_File.h"
_RFLink_File  RFLink_File ; // ( "/RFLink.txt" ) ;





// ***********************************************************************************
// ***********************************************************************************
/*

unsigned long Send_Time_ms   = 10000 ;

//#define OneWire_Pin  2   // PIN for OneWire, if necessary must be defined before library Sensor_Receiver_2

//#include "user_interface.h"      //system_get_sdk_version()
//#define WIFI_TX_POWER  82
//#define WIFI_MODE_BGN  PHY_MODE_11G

//#define CAMPER
#include "Sensor_Receiver_2.h"
// */

String MQTT_ID = "RFLink-ESP" ;
String Topic   = "ha/from_HA/" ;
#include <WiFi.h>
#include "PubSubClient.h"
#include "Wifi_Settings_ESP32.h"
WiFiClient My_WifiClient ;
PubSubClient MQTT_Client ( My_WifiClient ) ;

String Received_MQTT_Topic   ;
String Received_MQTT_Payload ;



// ***********************************************************************************
// Hardware pins
// ***********************************************************************************
#ifdef ESP32
  #define TRANSMIT_PIN    5    // Data to the 433Mhz transmitter on this pin
  #define RECEIVE_PIN    19    // On this input, the 433Mhz-RF signal is received. LOW when no signal.
#else
  #define TRANSMIT_PIN    5    // Data to the 433Mhz transmitter on this pin
  #define RECEIVE_PIN     4    // On this input, the 433Mhz-RF signal is received. LOW when no signal.
#endif

// ***********************************************************************************
// ***********************************************************************************
#include "RFL_Protocols.h" 


// ***********************************************************************************
// ***********************************************************************************
void MQTT_Receive_Callback ( char* topic, byte* payload, unsigned int length ) {
  String Payload = "" ;
  for ( int i = 0; i < length; i++ ) {
    Payload += (char) payload[i]  ;
  }
  Received_MQTT_Payload = Payload ;
  
  // ************************************************
  // from:   ha/from_HA/ev1527_005df     S02
  // to:     10;EV1527;005DF;2;ON;
  // ************************************************
  String Line = "10;" ;
  String Topic = String ( topic ) ;
  Received_MQTT_Topic = Topic ;
  Topic.toUpperCase () ;
  Topic = Topic.substring ( 11 ) ;     //  EV1527_005DF
  int x1 = Topic.indexOf ( "_" ) ;
  Line += Topic.substring ( 0, x1 ) + ";" + Topic.substring ( x1+1 ) +";" ;
  Payload = Payload.substring ( 1 ) ;
  int Switch = Payload.toInt () ;
  Line += String ( Switch ) + ";ON;" ;

  //if ( Learning_Mode > 0 ) {
    Serial.print   ( "MQTT Received Topic: " ) ;
    Serial.print   ( topic ) ;
    Serial.print   ( "  Payload: " + Payload ) ;
    Serial.println ( "    Converted: " + Line ) ;
  //}

  Line.toCharArray ( InputBuffer_Serial, sizeof ( InputBuffer_Serial ) ) ;
  Process_Serial () ;
}


/* ***********************************************************************
*********************************************************************** */
void MQTT_Reconnect() {
  // Loop until we're reconnected
  while ( !MQTT_Client.connected() ) {
    //Serial.print ( "Attempting MQTT connection ... " ) ;
    //Serial.print ( Broker_IP ) ;

    if ( MQTT_Client.connect ( MQTT_ID.c_str() ) ) {  //, MQTT_User, MQTT_Pwd,    Subscription_Out, 1, 1, LWT )) {
    //if ( MQTT_Client.connect ( MQTT_ID, MQTT_User, MQTT_Pwd ) ) {   //,    Subscription_Out, 1, 1, LWT )) {
      //Serial.println ( " connected" ) ;
      
      // ... and resubscribe
      MQTT_Client.subscribe ( ( Topic + "#").c_str() ) ;
    } 
    else {
      Serial.print ( "failed, rc=" ) ;
      Serial.print ( MQTT_Client.state() ) ;
      //Serial.println ( " try again in 5 seconds" ) ;
      // Wait 0.5 seconds before retrying
      delay ( 500 ) ;
    }
  }
}


// ***********************************************************************************
// ***********************************************************************************
void setup() {
  Serial.begin ( 57600 ) ;                    

  RFLink_File.Begin () ;

  WiFi.begin ( Wifi_Name, Wifi_PWD ) ;
  while ( WiFi.status () != WL_CONNECTED ) {
    delay ( 500 ) ;
    //Serial.print ( "." ) ;
  }

  MQTT_Client.setServer ( Broker_IP, Broker_Port ) ;
  MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
  
  
  pinMode      ( RECEIVE_PIN,  INPUT        ) ;
  pinMode      ( TRANSMIT_PIN, OUTPUT       ) ;
  digitalWrite ( RECEIVE_PIN,  INPUT_PULLUP ) ;  // pull-up resister on (to prevent garbage)

  // *********   PROTOCOL CLASSES, available and in this order   ************
  RFL_Protocols.Add ( new _RFL_Protocol_KAKU             () ) ;  
  RFL_Protocols.Add ( new _RFL_Protocol_EV1527           () ) ;  
  RFL_Protocols.Add ( new _RFL_Protocol_Paget_Door_Chime () ) ;  
  RFL_Protocols.setup () ;
  // ************************************************************************

  delay ( 200 ) ;
  Serial.printf ( "20;%02X;Nodo RadioFrequencyLink - MiRa V%s - R%02x\r\n", 
                  PKSequenceNumber++, Version, Revision );

  RawSignal.Time = millis() ;
}


// ***********************************************************************************
// ***********************************************************************************
void loop () {
  if ( !MQTT_Client.connected () ) {
    MQTT_Reconnect () ;
  }
  MQTT_Client.loop ();

  
  if ( FetchSignal () ) {
    RFL_Protocols.Decode ();
  }
  Collect_Serial () ;
}

