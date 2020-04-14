
#ifndef RFL_Protocol_Paget_Door_Chime_h
#define RFL_Protocol_Paget_Door_Chime_h

// ***********************************************************************************
// dit is het oude protocol 071
// ***********************************************************************************
class _RFL_Protocol_Paget_Door_Chime : public _RFL_Protocol_BaseClass {
	
  public:
    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _RFL_Protocol_Paget_Door_Chime () {
      Name = "Paget_Door_Chime" ;
    }
 
    // ***********************************************************************
    // ***********************************************************************
    bool RF_Decode (  ) {
      #define PulseCount 64 

      // ****************************************************
      // Check the length of the sequence
      // ****************************************************
      if ( RawSignal.Number != PulseCount + 3 ) return false;

      // ****************************************************
      //  Translate the sequence in bit-values and 
      //     jump out this method if errors are detected
      // 0-bit = a short high followed by a long low
      // 1-bit = a long  high followed by a short low
      // ****************************************************
      unsigned long BitStream = 0L ;
      unsigned long P1 ;
      unsigned long P2 ;  
      for ( byte x=2; x < PulseCount + 1 ; x=x+2 ) {
        P1 = RawSignal.Pulses [ x   ] ;
        P2 = RawSignal.Pulses [ x+1 ] ;
        if ( P1 > RawSignal.Mean ) {
          BitStream = ( BitStream << 1 ) | 0x1;  // append "1"
        } else { 
          BitStream =   BitStream << 1;          // append "0"
        }
      }

      //==================================================================================
      // Prevent repeating signals from showing up
      //==================================================================================
        if ( ( BitStream != Last_BitStream ) ||
             ( millis() > 700 + Last_Detection_Time ) ) {
         // not seen the RF packet recently
         Last_BitStream = BitStream;
      } else {
         // already seen the RF packet recently
         return true;
      } 

      // ****************************************************
      // 1010 1001 1010 1010 0000 0000 1110 0000  Zwart
      // 1010 1010 1010 1010 0000 0000 1110 0000   Wit
      // IIII IIII IIII IIII zzzz zzzz   CHIME
      // I = unique ID
      // z should be 0
      // CHIME should be one of : 
      //   0000 0011  =  0x03   == 0x03
      //   1000 1100  =  0x1C   => 0x01
      //   1110 0000  =  0xE0   => 0x02
      // ****************************************************
      byte Chime = BitStream & 0xFF ;
      if      ( Chime == 0x1C ) Chime = 1 ;
      else if ( Chime == 0xE0 ) Chime = 2 ;
      else if ( Chime != 3    ) return false ;

      BitStream  = ( BitStream >> 16 ) & 0xFFFFFFFF ; 
      /*
      sprintf ( pbuffer, "20;%02X;Door_Chime;ID=%04X;SWITCH=1;CMD=ON;CHIME=%02X;", PKSequenceNumber++, BitStream, Chime ) ; 
      Serial.println  (  pbuffer ) ;
      return true;
      */
      return Send_Message ( "Door_Chime", BitStream, 1, "ON", "CHIME=" + String(Chime)  ) ;     
    }
};
#endif
