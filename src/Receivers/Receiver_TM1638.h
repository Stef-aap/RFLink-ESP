
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
//    - listening to MQTT message works
//
// Version 0.1
//    - initial release, only create/setup works

#ifndef Receiver_TM1638_h
#define Receiver_TM1638_h 0.6

#include "Receiver_Base.h"
#include <TM1638plus.h>

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
          _Display_Module->brightness(_Intensity);
        } break;

        case 0x41: {
          if (_Intensity > 0) _Intensity -= 1;
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

      if (_TM1638_Regel[_Current_Line] != _Current_Regel) {
        _Current_Regel = _TM1638_Regel[_Current_Line];
        _Update_Line(_Current_Line);
      }

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
            }
          }
        } break;

        // ***********************************
        // scroll through the alphabet, when ready go to State=5
        // ***********************************
        case 61: {
        } break;
      }
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {
    Serial.println("TM1638 received MQTT Message " + Payload);

    _LED_Change = 0;

    // ATTENTION: a new string must be defined here every time, otherwise compile error
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
  }
};
#endif
