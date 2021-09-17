// Version 0.2
//    - Home_Command made case insensitive

// Version 0.1

#ifndef RFL_Protocol_KAKU_h
#define RFL_Protocol_KAKU_h 0.2

// ***********************************************************************************
// ***********************************************************************************
class _RFL_Protocol_KAKU : public _RFL_Protocol_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _RFL_Protocol_KAKU(int Receive_Pin, int Transmit_Pin) {
    this->_Receive_Pin = Receive_Pin;
    this->_Transmit_Pin = Transmit_Pin;
    Name = "NewKaku";
    NAME = Name;
    NAME.toUpperCase();
  }

  // ***********************************************************************
  // KAKU always consists of start bit + 32 bits + possibly 4 dim bits.
  // ***********************************************************************
  bool RF_Decode() {
#define KAKUPulseCount     132 // regular KAKU packet length
#define KAKUPulseCount_DIM 148 // KAKU packet length including DIM bits
#define MidTime            750 // usec, approx between 1T and 4T

    if ((RawSignal.Number != (KAKUPulseCount + 1)) && (RawSignal.Number != (KAKUPulseCount_DIM + 1))) return false;
    boolean Bit = 0;
    int i;
    int P0, P1, P2, P3;
    byte dim = 0;
    unsigned long BitStream = 0L;

    i = 4; // RawSignal.Pulses [4] is the first of a T,xT,T,xT combination
    do {
      P0 = RawSignal.Pulses[i];
      P1 = RawSignal.Pulses[i + 1];
      P2 = RawSignal.Pulses[i + 2];
      P3 = RawSignal.Pulses[i + 3];

      if (P0 < MidTime && P1 < MidTime && P2 < MidTime && P3 > MidTime) {
        Bit = 0; // T,T,T,4T
      } else if (P0 < MidTime && P1 > MidTime && P2 < MidTime && P3 < MidTime) {
        Bit = 1; // T,4T,T,T
      } else if (P0 < MidTime && P1 < MidTime && P2 < MidTime && P3 < MidTime) {
        // T,T,T,T This should be on i = 111 because: 27th NewKAKU bit times 4 plus 2 positions for start bit
        if (RawSignal.Number != (KAKUPulseCount_DIM + 1)) { // if no DIM bits
          return false;
        }
      } else {
        return false; // other options are not valid
      }

      // all bits belonging to the 32-bit pulse train 32bits * 4 positions per bit + pulse / space for start bit
      if (i < KAKUPulseCount - 1) {
        BitStream = (BitStream << 1) | Bit;
      } else { // the remaining four bits that belong to the dim level
        dim = (dim << 1) | Bit;
      }
      i += 4;
      //-2 because the space / pulse of the stop bit is no longer part of the signal.
    } while (i < RawSignal.Number - 2);

    //==================================================================================
    // Prevent repeating signals from showing up
    //==================================================================================
    if ((BitStream != Last_BitStream) || (millis() > 700 + Last_Detection_Time)) {
      // not seen the RF packet recently
      Last_BitStream = BitStream;
    } else {
      return true;
    }

    String On_Off = "OFF";
    if ((BitStream & 0x10) != 0) On_Off = "ON";
    int Switch = BitStream & 0x0F;
    unsigned long Id = BitStream >> 5;

    return Send_Message(Name, Id, Switch, On_Off);
  }

  // ***********************************************************************************
  // ***********************************************************************************
  bool Home_Command(String Device, unsigned long ID, int Switch, String On) {
    if (Device.compareTo(NAME)) return false;

    unsigned long Data = (ID << 5) | Switch;
    if (On == "ON") Data = Data | 0x10;
    int NData = 32;
    unsigned long Mask;
    bool Zero;
    int uSec = 260;

    // ************************************
    // send the sequence a number of times
    // ************************************
    for (int R = 0; R < 9; R++) {
      // *************************
      // start at the most significant bit position
      // *************************
      Mask = 0x01 << (NData - 1);

      // *************************
      // Start Bit = T, 10*T
      // *************************
      digitalWrite(this->_Transmit_Pin, HIGH);
      delayMicroseconds(uSec);
      digitalWrite(this->_Transmit_Pin, LOW);
      delayMicroseconds(10 * uSec);

// *************************
// 32 information bits
// *************************
#define NT 4 // 3,4,5 all work correctly
      for (int i = 0; i < NData; i++) {
        Zero = (Data & Mask) == 0;
        if (Zero) { // T, T, T, 4*T
          digitalWrite(this->_Transmit_Pin, HIGH);
          delayMicroseconds(uSec);
          digitalWrite(this->_Transmit_Pin, LOW);
          delayMicroseconds(uSec);
          digitalWrite(this->_Transmit_Pin, HIGH);
          delayMicroseconds(uSec);
          digitalWrite(this->_Transmit_Pin, LOW);
          delayMicroseconds(NT * uSec);
        } else { // T, 4*T, T, T
          digitalWrite(this->_Transmit_Pin, HIGH);
          delayMicroseconds(uSec);
          digitalWrite(this->_Transmit_Pin, LOW);
          delayMicroseconds(NT * uSec);
          digitalWrite(this->_Transmit_Pin, HIGH);
          delayMicroseconds(uSec);
          digitalWrite(this->_Transmit_Pin, LOW);
          delayMicroseconds(uSec);
        }
        // *************************
        // go to the next bit
        // *************************
        Mask = Mask >> 1;
      }
      // *************************
      // Stop Bit = T, 39*T
      // *************************
      digitalWrite(this->_Transmit_Pin, HIGH);
      delayMicroseconds(uSec);
      digitalWrite(this->_Transmit_Pin, LOW);
      delayMicroseconds(39 * uSec);
    }
    return true;
  }

  // ***********************************************************************************
  // ***********************************************************************************
private:
  int _Receive_Pin;
  int _Transmit_Pin;
};
#endif
