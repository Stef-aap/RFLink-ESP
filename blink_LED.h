


// ***********************************************************************************
// ***********************************************************************************
/*
// Define the Object
  _Blink_LED   Blink_LED ( 22, bool Invert = falsee ) ;

// Turn the LED On for some ms
  Blink_LED.On ( 100 ) ;

// In the loop Test the LED  
  Blink_LED.Test ();

*/

// ***********************************************************************************
// ***********************************************************************************
class _Blink_LED {
  public:
    int           _Blink_LED_Pin  ;
    bool          _Blink_LED_Invert ;
    unsigned long _Blink_LED_Time ;

    // *********************************************************
    // *********************************************************
    _Blink_LED ( int Pin = 22, bool Invert = false ) { 
      _Blink_LED_Pin    = Pin ;
      _Blink_LED_Invert = Invert ;
      pinMode ( _Blink_LED_Pin, OUTPUT ) ;
      digitalWrite ( _Blink_LED_Pin, HIGH ) ;
      this -> Off () ;
    }

    // *********************************************************
    // *********************************************************
    void On ( int Duration_ms = 100 ) { 
      if ( _Blink_LED_Invert ) digitalWrite ( _Blink_LED_Pin, HIGH ) ;
      else                     digitalWrite ( _Blink_LED_Pin, LOW  ) ;
      _Blink_LED_Time = Duration_ms + millis () ;
    }

    // *********************************************************
    // *********************************************************
    void Off () { 
      if ( _Blink_LED_Invert ) digitalWrite ( _Blink_LED_Pin, LOW  ) ;
      else                     digitalWrite ( _Blink_LED_Pin, HIGH ) ;
      _Blink_LED_Time = 0 ;
    }

    // *********************************************************
    // *********************************************************
    void Test () {
      if ( _Blink_LED_Time > 0 ) {
        if ( millis () > _Blink_LED_Time ) {
          this -> Off () ;
        }
      }
    }
};
