// Version 0.4, 28-01-2019, SM
//   - again full MQTT message as Home Assistant problems are solved
//
// Version 0.3, 27-01-2019, SM
//   - problem with empty fields in json command solved (by first testing string length
//   - output MQTT message limited to S1 (without any space), needed for HomeAssistant
//
// Version 0.2, 25-01-2019, SM
//   -Force_Push added
//
// Version 0.1, 22-01-2019, SM
//   - initial version
//   - External Watchdog support included (NOT TESTED)

#ifndef Sensor_SonOff_h
#define Sensor_SonOff_h 0.4

const int _SonOff_Button = 0;
const int _SonOff_LED = 13; //( The RED LED is connected to the RF device, which isn't there)
const int _SonOff_Relay = 12;
const int _SonOff_LED_msec = 200;

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_SonOff : public _Sensor_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_SonOff() { Serial.println("V" + String(Sensor_SonOff_h) + "   Sensor_SonOff.h"); }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    _LED_State = HIGH; // High = LED OFF
    pinMode(_SonOff_LED, OUTPUT);
    pinMode(_SonOff_Relay, OUTPUT);
    pinMode(_SonOff_Button, INPUT);
    digitalWrite(_SonOff_Relay, _Relay_State);
    digitalWrite(_SonOff_LED, _LED_State);
    _Button_Previous = digitalRead(_SonOff_Button);

    _LED_Notification(2);
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (true) {
      // ***************************************************
      // read the button and invert it, to get positive logic
      // ***************************************************
      int Button = !digitalRead(_SonOff_Button);
      unsigned long Now = millis();

      if (Button == _Button_Previous) {
        // *****************************************************
        // if button was not pressed and 1 second passed,
        // then action if there have been down presses
        // *****************************************************
        if (!Button) {
          if (_Button_Counter > 0) {
            if ((Now - _Button_Time_Off) > 1000) {
              switch (_Button_Counter) {
                // **********************
                // 1 Press = toggle Relay
                // **********************
                case 1:
                  _Relay_State = !_Relay_State;
                  digitalWrite(_SonOff_Relay, _Relay_State);
                  _LED_Notification(1);
                  _Force_Push = true;
                  break;

                default:
                  Serial.println("Button Presses not implemented, N = " + String(_Button_Counter));
                  break;
              }
              _Button_Counter = 0;
            }
          }
        }

        // ***************
        // Button pressed
        // ***************
        else {
          // ****************************************
          // RESTART if pressed longer than 5 seconds
          // ****************************************
          if ((Now - _Button_Time_On) > 5000) {
            // first time after serial programming:  ets Jan  8 2013,rst cause:4, boot mode:(1,7)
            Serial.println("RESTARTING: first time after serial programming resetting fails");
            _LED_Notification(3);
            _Force_Push = true;
            ESP.restart();
          }
        }
      }

      // ************************************
      // Button change
      // ************************************
      else {
        if (Button) {
          _Button_Time_On = Now;
          _Button_Counter += 1;
        } else {
          _Button_Time_Off = Now;
        }
        _Button_Previous = Button;
      }
    }
  }

  // ***********************************************************************
  // huis/verdieping0/ergens/SonOff_2 {"S1" : "ON", "LED" : 1, "Button" = 1 }
  // S1     = relay state
  // LED    = 0,  LED will always be OFF
  // Button = 0, Button presses will have no action
  // ***********************************************************************
  void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {
    String Switch_1 = root["S1"];
    if (Switch_1.length() > 0) {
      if (Switch_1 == "ON") {
        _Relay_State = HIGH;
        digitalWrite(_SonOff_Relay, _Relay_State);
      } else if (Switch_1 == "OFF") {
        _Relay_State = LOW;
        digitalWrite(_SonOff_Relay, _Relay_State);
      }
    }

    String Temp_LED = root["LED"];
    if (Temp_LED.length() > 0) {
      int LED = root["LED"];
      if (LED == 1) {
        _LED_Enabled = true;
      } else if (LED == 0) {
        _LED_Enabled = false;
      }
    }

    String Temp_Button = root["Button"];
    if (Temp_Button.length() > 0) {
      int Button = root["Button"];
      if (Button == 1) {
        _Button_Enabled = true;
      } else if (Button == 0) {
        _Button_Enabled = false;
      }
    }

    _Force_Push = true;
    _LED_Notification(2);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"S1\":";
    if (_Relay_State) {
      JSON_Data += "\"ON\",";
    } else {
      JSON_Data += "\"OFF\",";
    }
    JSON_Data += " \"LED\":";
    JSON_Data += !_LED_State;
    JSON_Data += ",";

    JSON_Data += " \"Button\":";
    int Button = !digitalRead(_SonOff_Button);
    JSON_Data += Button;
    JSON_Data += ",";
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Pushed() {
    if (_Force_Push) {
      _Force_Push = false;
      return true;
    }
    return false;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  int _Button_Counter = 0;
  bool _Button_Enabled = true;
  int _Button_Previous = true;
  int _Button_Time_On;
  int _Button_Time_Off;
  bool _Force_Push = false;
  bool _LED_Enabled = true;
  bool _LED_State;
  bool _Relay_State = false;

  // ***************************************************************************
  // ***************************************************************************
  void _LED_Notification(int N) {
    if (_LED_Enabled) {
      for (int i = 0; i < 2 * N; i++) {
        _LED_State = !_LED_State;
        digitalWrite(_SonOff_LED, _LED_State);
        delay(_SonOff_LED_msec);
      }
    }
  }
};

#endif
