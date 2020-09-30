
// Version 0.2,  19-10-2018, SM
//    - Improved Off command by doing Chan-UP first so it is definitely on
//
// Version 0.1, Initial_Release, 23-09-2018

#ifndef Sensor_IR_Samsung_TV_h
#define Sensor_IR_Samsung_TV_h 0.2

#include "Sensor_Base.h"

#include <IRremoteESP8266.h>
#include <IRsend.h>

#define Samsung_TV_Chan_Up 0xE0E048B7UL
#define Samsung_TV_Off 0xE0E040BFUL
#define Samsung_TV_NBit 32

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_IR_Samsung_TV : public _Sensor_BaseClass {
public:
  int LDR;
  IRsend *IR_Transmitter; // ( IR_LED, true );  // Inverted = true

  // ***********************************************************************
  // Creators
  // ***********************************************************************
  _Sensor_IR_Samsung_TV(int IR_LED, bool Inverted = true) { IR_Transmitter = new IRsend(IR_LED, Inverted); }

  // ***********************************************************************
  // ***********************************************************************
  void setup() { IR_Transmitter->begin(); }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (Time_TurnOff_Started > 0) {
      if (millis() > Time_TurnOff_Started) {
        Time_TurnOff_Started = 0;
        IR_Transmitter->sendSAMSUNG(Samsung_TV_Off, Samsung_TV_NBit);
        Serial.println("Samsung TV Echt UITgezet");
      }
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void Send_IR(unsigned long Data) { IR_Transmitter->sendSAMSUNG(Data, Samsung_TV_NBit); }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    LDR = analogRead(A0);
    JSON_Data += " \"LDR\":";
    JSON_Data += LDR;
    JSON_Data += ",";
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Hardware_Test(int Test_Nr = 1, unsigned long Time_Period = 2000) {
    switch (Test_Nr) {

      // ***************************************************
      // Toggle TV On/OFF ( 30 second cycle )
      // ***************************************************
      case 1:
        if (_Test_State == 0) {
          if (millis() - _Last_Time > 20000) {
            _Last_Time = millis();
            Serial.println("SAMSUNG AAN");
            Send_IR(Samsung_TV_Chan_Up);
            _Last_Command = "ON";
            _Test_State += 1;
          }
          return true;
        } else if (_Test_State == 1) {
          if (millis() - _Last_Time > 10000) {
            _Last_Time = millis();
            Serial.println("SAMSUNG UIT");
            Send_IR(Samsung_TV_Off);
            _Last_Command = "OFF";
            _Test_State = 0;
          }
          return false;
        } else {
          _Last_Time = millis();
          _Test_State = 0;
        }
        return false;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {
    String Line = "";
    for (int i = 0; i < length; i++) {
      Line += (char)payload[i];
    }
    Serial.printf("MQTT message received, topic = %s,  payload = %s\n", topic, Line.c_str());

    if (Line == "ON") {
      // ***********************************************
      // because the ON/OFF command is a toggle command
      // we use another command that will turn on the TV, namely CHAN-UP
      // ***********************************************
      IR_Transmitter->sendSAMSUNG(Samsung_TV_Chan_Up, Samsung_TV_NBit);
      _Last_Command = Line;
      Serial.println("Samsung TV AANgezet");
    } else {
      if (Line == "OFF") {
        // ***********************************************
        // first we must ensure the TV = On
        // ***********************************************
        IR_Transmitter->sendSAMSUNG(Samsung_TV_Chan_Up, Samsung_TV_NBit);
        _Last_Command = Line;
        Serial.println("Samsung TV AANgezet, om daarna te kunnen uitzetten");

        // ***********************************************
        // then switch it off after about 10 seconds (if we are sure it is on)
        // ***********************************************
        Time_TurnOff_Started = 10000 + millis();
      }
    }
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  unsigned long _Last_Time = millis();
  unsigned long Time_TurnOff_Started = 0;
  int _Test_State = 0;
  String _Last_Command = "";
};
#endif
