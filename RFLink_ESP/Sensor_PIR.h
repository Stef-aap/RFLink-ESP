// Version 0.2, 08-05-2019, SM
//   - Topic was hernoemd naar MQTT_Topic

// Version 0.1, 22-01-2019, SM
//   - initial version

#ifndef Sensor_PIR_h
#define Sensor_PIR_h    0.2


// ***********************************************************************************
// ***********************************************************************************
class _Sensor_PIR : public _Sensor_BaseClass {

  public:
   
    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _Sensor_PIR ( int PIR_Pin, int LED_Pin = -1 ) {
      _PIR_Pin = PIR_Pin ;
      _LED_Pin = LED_Pin ;
      Serial.println ( "V" + String ( Sensor_PIR_h ) + "   Sensor_PIR.h" ) ;
    }

    // ***********************************************************************
    // ***********************************************************************
    void setup () {
      pinMode ( _PIR_Pin, INPUT ) ;
      if ( _LED_Pin >= 0 ) {
        pinMode ( _LED_Pin, OUTPUT ) ;
      }
    }

    // ***********************************************************************
    // ***********************************************************************
    void loop () {
      if ( digitalRead ( _PIR_Pin ) != _PIR_Status ) {
        _PIR_Status = digitalRead ( _PIR_Pin ) ;
        if ( _LED_Pin >= 0 ) {
          digitalWrite ( _LED_Pin, _PIR_Status ) ;
        }
        if ( _PIR_Status == HIGH ) { 
         My_MQTT_Client->Publish ( MQTT_Topic + "_PIR", "{ \"PIR\" : \"1\" }" ) ;
        } 
        else {
         My_MQTT_Client->Publish ( MQTT_Topic + "_PIR", "{ \"PIR\" : \"0\" }" ) ;
        }
      } 
    }
    
    // ***********************************************************************
    // Get all the sampled data as a JSON string
    // ***********************************************************************
    void Get_JSON_Data () {
      JSON_Data += " \"PIR\":" ;
      JSON_Data += _PIR_Status ;
      JSON_Data += "," ;
    } 
    
  // ***********************************************************************
  private:
  // ***********************************************************************
    int  _PIR_Pin ;
    int  _LED_Pin ;
    bool _PIR_Status = HIGH ;
  
};

#endif


