
// Version 0.6
//    - other library used: TM1638plus.h
//
// Version 0.5
//    - ??
//
// Version 0.4
//    - initial _Current_Line set to 0
//
// Version 0.3
//
// Version 0.2
//    - changed from Sensor to Receiver
//    - luisteren naar MQTT bericht werkt
//
// Version 0.1
//    - initial release, only create/setup works

#ifndef Receiver_TM1638_h
#define Receiver_TM1638_h 0.6

#include "Receiver_Base.h"
#include <TM1638plus.h>

//    String _TM1638_Regel [8] = { "noSIGNAL", "        ", "        ", "        ", "        ", "        ", "        ", "
//    " } ; word   _TM1638_Dots  [8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } ;

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_TM1638 : public _Receiver_BaseClass {

public:
  TM1638plus *_Display_Module;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_TM1638(char *Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Receiver_TM1638(byte Data_Pin = 14, byte Clock_Pin = 12, byte Strobe_Pin = 13, byte Intensity = 1) {
    _Data_Pin = Data_Pin;
    _Clock_Pin = Clock_Pin;
    _Strobe_Pin = Strobe_Pin;
    _Intensity = Intensity;
    // Serial.println ( "V" + String ( Receiver_TM1638_h ) + "   Receiver_TM1638.h" ) ;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    Version_Name = "V" + String(Receiver_TM1638_h) + "   Receiver_TM1638.h";
    Serial.println("CREATE    " + Version_Name);

    Help_Text = "    ToDo";

    MQTT_Callback_Topic = _Topic;
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    _Display_Module = new TM1638plus(_Strobe_Pin, _Clock_Pin, _Data_Pin);
    _Display_Module->brightness(_Intensity);

    _State = 1;
    _Sample_Time_Last = millis();
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _Data_Pin = Settings.Get_Set_Default_Int("TM1638 Data Pin", 5, Force);
    _Clock_Pin = Settings.Get_Set_Default_Int("TM1638 Clock Pin", 12, Force);
    _Strobe_Pin = Settings.Get_Set_Default_Int("TM1638 Strobe Pin", 14, Force);
    _Topic = Settings.Get_Set_Default_String("TM1638 MQTT Topic", "huis/verdieping/Display_Ding", Force);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int Data_Pin = Settings.Read_Int("TM1638 Data Pin");
    int Clock_Pin = Settings.Read_Int("TM1638 Clock Pin");
    int Strobe_Pin = Settings.Read_Int("TM1638 Strobe Pin");
    String Topic = Settings.Read_String("TM1638 MQTT Topic");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "TM1638 Data Pin") {
        if (New_Value_Int != Data_Pin) {
          _My_Settings_Buffer["TM1638 Data Pin"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "TM1638 Clock Pin") {
        if (New_Value_Int != Clock_Pin) {
          _My_Settings_Buffer["TM1638 Clock Pin"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "TM1638 Strobe Pin") {
        if (New_Value_Int != Strobe_Pin) {
          _My_Settings_Buffer["TM1638 Strobe Pin"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "TM1638 MQTT Topic") {
        if (My_Webserver.arg(i) != Topic) {
          _My_Settings_Buffer["TM1638 MQTT Topic"] = My_Webserver.arg(i);
          Restart = true;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Send_Data(String JSON_Message) {
    Serial.println(JSON_Message);
    // this -> setDisplayToString ( "AA.pje  . " ) ;
    // this -> setDisplayToString ( "AApje   ", 0x46 ) ;
    return true;
  }

  // ***********************************************************************
  // ***********************************************************************
  void setDisplayToString(String Line, word Dots = 0) {
    int Start = 1;
    for (int i = 0; i < 8; i++) {
      if (Dots & 0x80) {
        Line = Line.substring(0, Start + i) + "." + Line.substring(Start + i);
        Start += 1;
      }
      Dots *= 2;
    }

    _Display_Module->displayText(Line.c_str());
  }

  // ***********************************************************************
  // ***********************************************************************
  void setLEDs(byte LEDs) {
    for (int i = 0; i < 8; i++) {
      _Display_Module->setLED(i, (LEDs & 0x01));
      LEDs /= 2;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void setDisplayToHexNumber(unsigned long HexNumber, byte Dots = 0) {}

  // ***********************************************************************
  // ***********************************************************************
  void Do_Button_Action() {
    if (_Buttons_Old != _Buttons_Previous) {
      // Serial.println ( "TM1638 Buttons Changed" );
      _Buttons_Previous = _Buttons_Old;
      switch (_Buttons_Old) {
        case 0x01: {
          this->setDisplayToString(_TM1638_Regel[0], _TM1638_Dots[0]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 0;
        } break;
        case 0x02: {
          this->setDisplayToString(_TM1638_Regel[1], _TM1638_Dots[1]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 1;
        } break;
        case 0x04: {
          this->setDisplayToString(_TM1638_Regel[2], _TM1638_Dots[2]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 2;
        } break;
        case 0x08: {
          this->setDisplayToString(_TM1638_Regel[3], _TM1638_Dots[3]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 3;
        } break;
        case 0x10: {
          this->setDisplayToString(_TM1638_Regel[4], _TM1638_Dots[4]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 4;
        } break;
        case 0x20: {
          this->setDisplayToString(_TM1638_Regel[5], _TM1638_Dots[5]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 5;
        } break;
        case 0x40: {
          this->setDisplayToString(_TM1638_Regel[6], _TM1638_Dots[6]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 6;
        } break;
        case 0x80: {
          this->setDisplayToString(_TM1638_Regel[7], _TM1638_Dots[7]);
          _MySetLeds(_Buttons_Old);
          _Current_Line = 7;
        } break;

        case 0x81: {
          if (_Intensity < 7) _Intensity += 1;
          // this -> setupDisplay ( true, _Intensity ) ;
          _Display_Module->brightness(_Intensity);
        } break;

        case 0x41: {
          if (_Intensity > 0) _Intensity -= 1;
          // this -> setupDisplay ( true, _Intensity ) ;
          _Display_Module->brightness(_Intensity);
        } break;

        case 0x01 | 0x04: {
          My_MQTT_Client->Publish(_Topic + "/Buttons/2", "1");
        } break;

        case 0x01 | 0x08: {
          My_MQTT_Client->Publish(_Topic + "/Buttons/3", "1");
        } break;
        case 0x01 | 0x10: {
          My_MQTT_Client->Publish(_Topic + "/Buttons/4", "1");
        } break;

        case 0x01 | 0x20: {
          My_MQTT_Client->Publish(_Topic + "/Buttons/5", "1");
        } break;

        case 0x80 | 0x02: {
          My_MQTT_Client->Publish(_Topic + "/Buttons2/1", "1");
        } break;

        case 0x80 | 0x04: {
          My_MQTT_Client->Publish(_Topic + "/Buttons2/2", "1");
        } break;

        case 0x80 | 0x08: {
          My_MQTT_Client->Publish(_Topic + "/Buttons2/3", "1");
        } break;

        case 0x80 | 0x10: {
          My_MQTT_Client->Publish(_Topic + "/Buttons2/4", "1");
        } break;

        case 0x80 | 0x20: {
          My_MQTT_Client->Publish(_Topic + "/Buttons2/5", "1");
        } break;

        case 0x80 | 0x40: {
          My_MQTT_Client->Publish(_Topic + "/Buttons2/6", "1");
        } break;

        case 0x03: {
          _State = 61;
          _Font_Count = 32;
          _Scroll_Line = "        ";
          _Scroll_Dots = 0x01;
        } break;
      }
    }
  }
  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (millis() - _Sample_Time_Last >= _Sample_Time_ms) {
      _Sample_Time_Last += _Sample_Time_ms;
      _Loop_Count += 1;
#define Delay_Intro 3
      // this -> setDisplayToString ( "AApje   ", 0xFF ) ;

      if (_TM1638_Regel[_Current_Line] != _Current_Regel) {
        _Current_Regel = _TM1638_Regel[_Current_Line];
        _Update_Line(_Current_Line);
      }

      /*
if ( _State != 5 ) {
  Serial.println ( "TMTMTMTMT STate = " + String (_State) ) ;
}
//*/
      switch (_State) {
        case 1: {
          if ((_Loop_Count % Delay_Intro) == 0) {
            this->setDisplayToString("88888888", 0xFF);
            this->setLEDs(0xFF);
            _State = 2;
          }
        } break;

        case 2: {
          if ((_Loop_Count % Delay_Intro) == 0) {
            this->setDisplayToString("", 0x00);
            this->setLEDs(0x00);
            _State = 3;
          }
        } break;

        case 3: {
          if ((_Loop_Count % Delay_Intro) == 0) {
            this->setDisplayToString(_TM1638_Regel[0], _TM1638_Dots[0]);
            // this -> setLEDs ( 0x01 );
            _MySetLeds(0x01);
            _State = 5;
          }
        } break;

        // ***********************************
        // Main State, listen to MQTT messages
        // ***********************************
        case 5: {
          byte Buttons = _Display_Module->readButtons();
          if (Buttons != _Buttons_Old) {
            _Buttons_Old = Buttons;
          } else {
            Do_Button_Action();

            if (_LED_Change != 0) {
              word LEDs = _LED ^ _LED_Change;
              this->setLEDs(LEDs);
              _LED_Change = 0;
            } else {
              this->setLEDs(_LED);
              // Debugf ( "Set LEDS back %i", _LED ) ;
            }
          }
        } break;

        // ***********************************
        // scroll through te alfabet, when ready go to State=5
        // ***********************************
        case 61: {
          /*
              if ( ( _Loop_Count % 7 ) == 0 ) {
                const int N = sizeof ( FONT_DEFAULT ) ;
                _Scroll_Line = _Scroll_Line.substring ( 1 ) + String ( (char)_Font_Count) ; //FONT_DEFAULT [ _Font_Count
  ] ; Serial.print ( N ) ; Serial.println ( _Scroll_Line ) ; _Font_Count += 1 ; if ( _Font_Count >= 127 ) { _State = 5 ;
                }
                _Scroll_Dots = _Scroll_Dots << 1 ;
                if ( _Scroll_Dots > 0x80 ) {
                  _Scroll_Dots = 0x01 ;
                }
                this -> setDisplayToString ( _Scroll_Line ) ; //, _Scroll_Dots ) ;
                //_Scroll_LED
              }
  //*/
        } break;

          //          default :
          // do nothing
      }
      // Debug ( "Loop of Receiver_TM1638" ) ;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {
    Serial.println("TM1638 received MQTT Message " + Payload);

    _LED_Change = 0;

    // LET OP: er moet hier iedere keer een nieuwe string worden gedefineerd, anders compile error
    String Regel_S7 = root["R8"]["S"];
    if (Regel_S7 != "null") {
      _TM1638_Dots[7] = root["R8"]["D"];
      _TM1638_Regel[7] = Regel_S7;
      _LED_Change |= 0x80;
      _Update_Line(7);
    }

    String Regel_S6 = root["R7"]["S"];
    if (Regel_S6 != "null") {
      _TM1638_Dots[6] = root["R7"]["D"];
      _TM1638_Regel[6] = Regel_S6;
      _LED_Change |= 0x40;
      _Update_Line(6);
    }

    String Regel_S5 = root["R6"]["S"];
    if (Regel_S5 != "null") {
      _TM1638_Dots[5] = root["R6"]["D"];
      _TM1638_Regel[5] = Regel_S5;
      _LED_Change |= 0x20;
      _Update_Line(5);
    }

    String Regel_S4 = root["R5"]["S"];
    if (Regel_S4 != "null") {
      _TM1638_Dots[4] = root["R5"]["D"];
      _TM1638_Regel[4] = Regel_S4;
      _LED_Change |= 0x10;
      _Update_Line(4);
    }

    String Regel_S3 = root["R4"]["S"];
    if (Regel_S3 != "null") {
      _TM1638_Dots[3] = root["R4"]["D"];
      _TM1638_Regel[3] = Regel_S3;
      _LED_Change |= 0x08;
      _Update_Line(3);
    }

    String Regel_S2 = root["R3"]["S"];
    if (Regel_S2 != "null") {
      _TM1638_Dots[2] = root["R3"]["D"];
      _TM1638_Regel[2] = Regel_S2;
      _LED_Change |= 0x04;
      _Update_Line(2);
    }

    String Regel_S1 = root["R2"]["S"];
    if (Regel_S1 != "null") {
      _TM1638_Dots[1] = root["R2"]["D"];
      _TM1638_Regel[1] = Regel_S1;
      _LED_Change |= 0x02;
      _Update_Line(1);
    }

    String Regel_S0 = root["R1"]["S"];
    if (Regel_S0 != "null") {
      _TM1638_Dots[0] = root["R1"]["D"];
      _TM1638_Regel[0] = Regel_S0;
      _LED_Change |= 0x01;
      _Update_Line(0);
    }
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  byte _Data_Pin;
  byte _Clock_Pin;
  byte _Strobe_Pin;
  byte _Intensity = 2;
  String _Topic;
  int _State = 0;
  byte _Buttons_Old = 0;
  byte _Buttons_Previous = 0;
  String _Scroll_Line = "        ";
  int _Font_Count = 0;
  word _Scroll_Dots;
  word _LED_Change = 0;
  word _LED = 0;
  int _Current_Line = 0;
  String _Current_Regel = "";

  unsigned long _Sample_Time_ms = 100;
  unsigned long _Sample_Time_Last;
  unsigned long _Loop_Count = 0;

  // ***********************************************************************
  // ***********************************************************************
  void _Update_Line(int Line_Nr) {
    if (Line_Nr == _Current_Line) {
      this->setDisplayToString(_TM1638_Regel[_Current_Line], _TM1638_Dots[_Current_Line]);
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void _MySetLeds(word NewLeds) {
    _LED = NewLeds;
    this->setLEDs(_LED);
    // Serial.println ( "LEDs : " + String ( _LED ) ) ;
  }
};
#endif

/*

String Aap= " !\"#$%&'()*+,-./0123456789:;=

const byte FONT_DEFAULT[] = {
  0b00000000, // (32)  <space>
  0b10000110, // (33) !
  0b00100010, // (34) "
  0b01111110, // (35) #
  0b01101101, // (36) $
  0b00000000, // (37) %
  0b00000000, // (38) &
  0b00000010, // (39) '
  0b00110000, // (40) (
  0b00000110, // (41) )
  0b01100011, // (42) *
  0b00000000, // (43) +
  0b00000100, // (44) ,
  0b01000000, // (45) -
  0b10000000, // (46) .
  0b01010010, // (47) /
  0b00111111, // (48) 0
  0b00000110, // (49) 1
  0b01011011, // (50) 2
  0b01001111, // (51) 3
  0b01100110, // (52) 4
  0b01101101, // (53) 5
  0b01111101, // (54) 6
  0b00100111, // (55) 7
  0b01111111, // (56) 8
  0b01101111, // (57) 9
  0b00000000, // (58) :
  0b00000000, // (59) ;
  0b00000000, // (60) <
  0b01001000, // (61) =
  0b00000000, // (62) >
  0b01010011, // (63) ?
  0b01011111, // (64) @
  0b01110111, // (65) A
  0b01111111, // (66) B
  0b00111001, // (67) C
  0b00111111, // (68) D
  0b01111001, // (69) E
  0b01110001, // (70) F
  0b00111101, // (71) G
  0b01110110, // (72) H
  0b00000110, // (73) I
  0b00011111, // (74) J
  0b01101001, // (75) K
  0b00111000, // (76) L
  0b00010101, // (77) M
  0b00110111, // (78) N
  0b00111111, // (79) O
  0b01110011, // (80) P
  0b01100111, // (81) Q
  0b00110001, // (82) R
  0b01101101, // (83) S
  0b01111000, // (84) T
  0b00111110, // (85) U
  0b00101010, // (86) V
  0b00011101, // (87) W
  0b01110110, // (88) X
  0b01101110, // (89) Y
  0b01011011, // (90) Z
  0b00111001, // (91) [
  0b01100100, // (92) \ (this can't be the last char on a line, even in comment or it'll concat)
  0b00001111, // (93) ]
  0b00000000, // (94) ^
  0b00001000, // (95) _
  0b00100000, // (96) `
  0b01011111, // (97) a
  0b01111100, // (98) b
  0b01011000, // (99) c
  0b01011110, // (100)  d
  0b01111011, // (101)  e
  0b00110001, // (102)  f
  0b01101111, // (103)  g
  0b01110100, // (104)  h
  0b00000100, // (105)  i
  0b00001110, // (106)  j
  0b01110101, // (107)  k
  0b00110000, // (108)  l
  0b01010101, // (109)  m
  0b01010100, // (110)  n
  0b01011100, // (111)  o
  0b01110011, // (112)  p
  0b01100111, // (113)  q
  0b01010000, // (114)  r
  0b01101101, // (115)  s
  0b01111000, // (116)  t
  0b00011100, // (117)  u
  0b00101010, // (118)  v
  0b00011101, // (119)  w
  0b01110110, // (120)  x
  0b01101110, // (121)  y
  0b01000111, // (122)  z
  0b01000110, // (123)  {
  0b00000110, // (124)  |
  0b01110000, // (125)  }
  0b00000001, // (126)  ~
};


class TM1638plus  {

public:
    // Constructor
    // Init the module
    TM1638plus(uint8_t strobe, uint8_t clock, uint8_t data);
    // Methods
    // Send a command to module
    void sendCommand(uint8_t value);

    // Reset module
    void reset(void);

    //Sets the brightness level on a scale of brightness = 0 to 7.
    //0 is not turned off, it's just the lowest brightness.
    //If user wishes to change the default brightness at start-up change.
     //The DEFAULT_BRIGHTNESS define in header file.
    void brightness(uint8_t brightness);

    //Read buttons returns a byte with value of buttons 1-8 b7b6b5b4b3b2b1b0
    // 1 pressed, zero not pressed.
    //User may have to debounce buttons depending on application.
    //See [URL LINK](https://github.com/gavinlyonsrepo/Arduino_Clock_3)
    // for de-bonce example.
    uint8_t readButtons(void);

    // Set an LED, pass it LED position 0-7 and value 0 or 1
    void setLED(uint8_t position, uint8_t value);

    // Send Text to Seven segments, passed char array pointer
    // dots are removed from string and dot on preceding digit switched on
    // "abc.def" will be shown as "abcdef" with c decimal point turned on.
    void displayText(const char *text);

    // Send ASCII value to seven segment, pass position 0-7 and ASCII value byte
    void displayASCII(uint8_t position, uint8_t ascii);

    // Same as displayASCII function but turns on dot/decimal point  as well
    void displayASCIIwDot(uint8_t position, uint8_t ascii) ;

    // Send HEX value to seven segment, pass position 0-7 and hex value(DEC) 0-15
    void displayHex(uint8_t position, uint8_t hex);

    // Send seven segment value to seven segment
    //  pass position 0-7 byte of data corresponding to segments (dp)gfedcba
    // i.e 0b01000001 will set g and a on.
    void display7Seg(uint8_t position, uint8_t value);

private:
        uint8_t _STROBE_IO;
        uint8_t _DATA_IO;
        uint8_t _CLOCK_IO;

};

*/
