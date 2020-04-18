// Version 0.2
//    - Home_Command made case insensitive

// Version 0.1

#ifndef RFL_Protocol_KAKU_h
#define RFL_Protocol_KAKU_h  0.2

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_KAKU : public _RFL_Protocol_BaseClass {
	
  public:

    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _RFL_Protocol_KAKU ( int Receive_Pin, int Transmit_Pin ) {
      this->_Receive_Pin  = Receive_Pin  ;
      this->_Transmit_Pin = Transmit_Pin ;
      //Name = "KAKU" ;
      Name = "NewKaku" ;
      NAME = Name ;
      NAME.toUpperCase () ;
   }
 
    // ***********************************************************************
    // KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits.
    // ***********************************************************************
    bool RF_Decode (  ) {
      #define PulseCount       132   // regular KAKU packet length
      #define PulseCount_DIM   148   // KAKU packet length including DIM bits
      #define MidTime          750   // usec, approx between 1T and 4T
	  
      if ( ( RawSignal.Number != ( PulseCount     + 1 ) ) && 
           ( RawSignal.Number != ( PulseCount_DIM + 1 ) ) ) return false;
      boolean Bit;
      int i;
      int P0,P1,P2,P3;
      byte dim = 0;
      unsigned long BitStream = 0L;
      
      i = 4 ;   // RawSignal.Pulses [ 4 ] is de eerste van een T,xT,T,xT combinatie
      do {
        P0 = RawSignal.Pulses [ i     ] ;
        P1 = RawSignal.Pulses [ i + 1 ] ;
        P2 = RawSignal.Pulses [ i + 2 ] ;
        P3 = RawSignal.Pulses [ i + 3 ] ;
          
        if ( P0 < MidTime && P1 < MidTime && P2 < MidTime && P3 > MidTime ) { 
          Bit=0; // T,T,T,4T
        } else 
        if ( P0 < MidTime && P1 > MidTime && P2 < MidTime && P3 < MidTime ) {
          Bit=1; // T,4T,T,T
        } else 
        if ( P0 < MidTime && P1 < MidTime && P2 < MidTime && P3 < MidTime ) {      
          // T,T,T,T Deze hoort te zitten op i=111 want: 27e NewKAKU bit maal 4 plus 2 posities voor startbit
          if( RawSignal.Number != ( PulseCount_DIM + 1 ) ) {    // als geen DIM bits
            return false;
          }
        } else {
          return false;     // andere mogelijkheden zijn niet geldig
        }
          
        // alle bits die tot de 32-bit pulstrein behoren 32bits * 4posities per bit + pulse/space voor startbit
        if ( i < PulseCount-1 ) {                                              
          BitStream = ( BitStream << 1 ) | Bit;
        } else {                                     // de resterende vier bits die tot het dimlevel behoren 
          dim = ( dim << 1 ) | Bit ;
        }       
        i += 4;
      //-2 omdat de space/pulse van de stopbit geen deel meer van signaal uit maakt.  
      } while ( i < RawSignal.Number - 2 ) ;   
     
      //==================================================================================
      // Prevent repeating signals from showing up
      //==================================================================================
      if ( ( BitStream !=Last_BitStream ) || 
           ( millis() > 700 + Last_Detection_Time ) ) {
        // not seen the RF packet recently
        Last_BitStream = BitStream ;
      } else {
        return true;
      } 

      String On_Off = "OFF" ;
      if ( ( BitStream & 0x10 ) != 0 ) On_Off = "ON" ; 
      int           Switch = BitStream & 0x0F ;
      unsigned long Id     = BitStream >> 5 ;

      /*
      sprintf ( _RFLink_pbuffer, "%s;ID=%05X;", Name.c_str(), Id ) ; 
      if ( Unknown_Device ( _RFLink_pbuffer ) ) return false ;
      
      Serial.print   ( PreFix ) ;
      Serial.print   ( _RFLink_pbuffer ) ;
      sprintf ( _RFLink_pbuffer2, "SWITCH=%0X;CMD=%s;", Switch, On_Off.c_str() ) ; 
      Serial.println ( _RFLink_pbuffer2 ) ;
      PKSequenceNumber += 1 ;
      return true;
      */
      return Send_Message ( Name, Id, Switch, On_Off ) ;
      
    }

    // ***********************************************************************************
    // ***********************************************************************************
    bool Home_Command ( String Device, unsigned long ID, int Switch, String On ) {
      //if ( Device !=  Name.toUpperCase () ) return false ;
      if ( Device.compareTo ( NAME ) ) return false ;

      unsigned long Data  = ( ID << 5 ) | Switch ;
      if ( On == "ON" ) Data = Data | 0x10 ;
      int           NData = 32 ;
      unsigned long Mask ;
      bool          Zero ;
      int           uSec  = 260 ; 
      
      // ************************************
      // send the sequence a number of times
      // ************************************
      for ( int R=0; R<9; R++ ) {
        // *************************
        // start at the most significant bit position
        // *************************
        Mask = 0x01 << ( NData -1 ) ;

        // *************************
        // Start Bit = T, 10*T
        // *************************
        digitalWrite ( this->_Transmit_Pin, HIGH ) ;
        delayMicroseconds (      uSec ) ;
        digitalWrite ( this->_Transmit_Pin, LOW ) ;
        delayMicroseconds ( 10 * uSec ) ;
        
        // *************************
        // 32 information bits
        // *************************
        #define NT 4     // 3,4,5 all work correctly
        for ( int i=0; i<NData; i++ ) {
          Zero = ( Data & Mask ) == 0 ;
          if ( Zero ) {                                       // T, T, T, 4*T
            digitalWrite      ( this->_Transmit_Pin, HIGH ) ;
            delayMicroseconds (      uSec                  ) ;
            digitalWrite      ( this->_Transmit_Pin, LOW  ) ;
            delayMicroseconds (      uSec                  ) ;
            digitalWrite      ( this->_Transmit_Pin, HIGH ) ;
            delayMicroseconds (      uSec                  ) ;
            digitalWrite      ( this->_Transmit_Pin, LOW  ) ;
            delayMicroseconds ( NT * uSec                  ) ;
          } else {                                            // T, 4*T, T, T
            digitalWrite      ( this->_Transmit_Pin, HIGH ) ;
            delayMicroseconds (      uSec                  ) ;
            digitalWrite      ( this->_Transmit_Pin, LOW  ) ;
            delayMicroseconds ( NT * uSec                  ) ;
            digitalWrite      ( this->_Transmit_Pin, HIGH ) ;
            delayMicroseconds (      uSec                  ) ;
            digitalWrite      ( this->_Transmit_Pin, LOW  ) ;
            delayMicroseconds (      uSec                  ) ;
          }
          // *************************
          // go to the next bit
          // *************************
          Mask = Mask >> 1 ;
        }
        // *************************
        // Stop Bit = T, 39*T
        // *************************
        digitalWrite      ( this->_Transmit_Pin, HIGH ) ;
        delayMicroseconds (      uSec                  ) ;
        digitalWrite      ( this->_Transmit_Pin, LOW  ) ;
        delayMicroseconds ( 39 * uSec                  ) ;
      }
      return true ;
    }

  // ***********************************************************************************
  // ***********************************************************************************
  private:
    int _Receive_Pin  ;
    int _Transmit_Pin ;
    
    
};
#endif
