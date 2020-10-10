// ***********************************************************************************
// Version 1.6 17-04-2020, SM
//    - replace property _Topic with global MQTT_Topic
//    - several subtopics derived from MQTT_Topic
//
// Version 1.5 04/12/2020, SM
//    - Method Publish_Without_ added
//
// Version 1.4 29-02-2020, SM
//    - dynamically change broker address, e.g. after setup:
//   if (WiFi.localIP()[2] == 0) MQTT_Broker_IP = "192.168.0.18";
//
// Version 1.3 22-02-2020, SM
//    - support for 2 broker addresses
//
// Version 1.2 15-02-2020, SM
//    - SOCKET_TIMEOUT in PubSubClient.h lowered anyway (seems to work better)
//        // #define MQTT_SOCKET_TIMEOUT 600
//        #define MQTT_SOCKET_TIMEOUT 15

// Version 1.1 15-10-2019, SM
//    - WifiClient brought here from Sensor_Wifi.h
//
// Version 1.0 24-09-2019, SM
//    - ReConnect called in loop ()
//    - ReConnect doesn't need to publish anymore
//
// Version 0.9 06-08-2019, SM
//    - test wifi available removed
//    - reset on no conection removed
//
// Version 0.8 2019-07-19, SM
//    - MQTT Callback transferred to Sensor_Receiver.h (now multiple Sensors / Receivers can receive MQTT)
//
// Version 0.7 2019-05-29, SM
//    - will not restart if no connection can be established and Allow_Wifi_Not_Found == true
//    - Responds to MQTT message: {"Loop_Time": 2}
//    - MQTT Callback built in
//
// Version 0.6 04-03-2019, SM
//    - Multiple Broker completely removed
//
// Version 0.5 02-01-2019, SM
//    - External_Watchdog_Disarm added
//
// Version 0.4, 10/20/2018, SM, checked by ..
//    - added mqtt reconnect anyway
//
// Version 0.3, 10-10-2018, SM, checked by ..
//    - back to single broker for a moment
//
// Version 0.2, 06/27/2018, SM, checked by ..
//    - increased time to get an reset from 2 to 10 seconds
//
// Version 0.1, 03/30/2018, SM, checked by ..
//    - initial version
// ***********************************************************************************

#ifndef Receiver_MQTT_h
#define Receiver_MQTT_h 1.6

#include "Clients/PubSubClient.h" // MQTT client
#include "Receiver_Base.h"

#define Broker_Port 1883
WiFiClient _MQTT_WifiClient;

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_MQTT : public _Receiver_BaseClass {

public:
  PubSubClient *MyMQTT;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_MQTT(int Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }

  _Receiver_MQTT(String Topic = "MQTT_Receiver/Test", String User = "", String Password = "") {
    MQTT_Topic = Topic;
    this->_MQTT_User = User;
    this->_MQTT_PWD = Password;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    MQTT_Topics_Append(MQTT_Topic);

    Version_Name = "V" + String(Receiver_MQTT_h) + "  ──────  Receiver_MQTT.h";
    Serial.println("\n──────  CREATE  ──────  " + Version_Name);
    Help_Text = "Topic = " + MQTT_Topic;

    if (!this->Device_Active) return;

    High_Priority_Loop = 2;

    MyMQTT = new PubSubClient(_MQTT_WifiClient);

    MyMQTT->setServer(MQTT_Broker_IP.c_str(), Broker_Port);
    this->_MQTT_Broker_IPx = MQTT_Broker_IP;
    _MQTT_ID = String(WiFi.macAddress());

    // *****************************************************
    // Since it is not a callback but an event, this must be so difficult
    // https://hobbytronics.com.pk/arduino-custom-library-and-pubsubclient-call-back/
    // *****************************************************

    MyMQTT->setCallback(_MQTT_Callback_Wrapper);

    // *****************************************************
    // *****************************************************

    Serial.print("IP = ");
    Serial.print(WiFi.localIP());
    Serial.print("   Broker = ");
    Serial.println(MQTT_Broker_IP);

    _Subscription_Out = MQTT_Topic + "_";
    _LWT = "\"$$Dead " + _MQTT_ID + "\"";
    _ALIVE = "\"$$Alive " + _MQTT_ID + "\"";
    _Error_Time = 0;

    _Debug_MQTT_Available = true;
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    if (!this->Device_Active) return;
    MQTT_Topic = Settings.Get_Set_Default_String("MQTT Topic", "huis/verdieping/kamer/ding", Force);
    MQTT_Broker_IP = Settings.Get_Set_Default_String("MQTT Broker-IP", "192.168.0.18", Force);
    _MQTT_User = Settings.Get_Set_Default_String("MQTT User", "", Force);
    _MQTT_PWD = Settings.Get_Set_Default_String("$MQTT Password", "", Force);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() {
    if (!this->Device_Active) return false;

    bool Restart = false;

    String New_Value;
    String Topic = Settings.Read_String("MQTT Topic");
    String Broker = Settings.Read_String("MQTT Broker-IP");
    String User = Settings.Read_String("MQTT User");
    String Password = Settings.Read_String("$MQTT Password");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value = My_Webserver.arg(i);

      if (My_Webserver.argName(i) == "MQTT Topic") {
        if (New_Value != Topic) {
          _My_Settings_Buffer["MQTT Topic"] = New_Value;
          Restart = true;
        }
      }

      else if (My_Webserver.argName(i) == "MQTT Broker-IP") {
        if (New_Value != Broker) {
          _My_Settings_Buffer["MQTT Broker-IP"] = New_Value;
          Restart = true;
        }
      }

      else if (My_Webserver.argName(i) == "MQTT User") {
        if (New_Value != User) {
          _My_Settings_Buffer["MQTT User"] = New_Value;
          Restart = true;
        }
      }

      else if (My_Webserver.argName(i) == "$MQTT Password") {
        if (New_Value != Password) {
          _My_Settings_Buffer["$MQTT Password"] = New_Value;
          Restart = true;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (!this->Device_Active) return;

    if (!MyMQTT->connected()) {
      // **********************************************************
      // Check if the broker address (dynamic) has been changed
      // **********************************************************
      if (MQTT_Broker_IP != this->_MQTT_Broker_IPx) {
        MyMQTT->setServer(MQTT_Broker_IP.c_str(), Broker_Port);
        this->_MQTT_Broker_IPx = MQTT_Broker_IP;
        this->ReConnect();
        _ReConnect_Start = millis();
        return;
      }

      if ((_ReConnect_Start <= 0) || ((millis() - _ReConnect_Start) > 2000)) {
        this->ReConnect();
        _ReConnect_Start = millis();
      }
    }

    MyMQTT->loop();
  }

  // ***********************************************************************
  // OF COURSE MAY NOT INCLUDE DEBUG STATEMENTS !!!
  // ***********************************************************************
  bool Publish(String Topic, String Payload) { return this->Publish_Without_(Topic + "_", Payload); }

  // ***********************************************************************
  bool Publish_Without_(String Topic, String Payload) {
    if (!this->Device_Active) return false;

    if (MyMQTT->connected()) {
      MyMQTT->publish(Topic.c_str(), Payload.c_str());
      _Error_Time = 0;
      Serial.println("Published To: " + this->_MQTT_Broker_IPx + "   Topic: " + Topic);
      return true; // SHOULD BE IMPROVED
    }

    Serial.println("No MQTT Connection to " + this->_MQTT_Broker_IPx);

    return false;
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Connected() { return MyMQTT->connected(); }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String Payload) {
    if (!this->Device_Active) return false;

    Serial.println("    publish: " + MQTT_Topic + " // " + Payload);
    return Publish(MQTT_Topic, Payload);
  }

  // ***********************************************************************
  // ***********************************************************************
private:
  int _State = 0;
  unsigned long _Error_Time = 0;
  String _MQTT_ID;
  String _MQTT_User;
  String _MQTT_PWD;
  String _Subscription_Out;
  String _LWT;
  String _ALIVE;
  unsigned long _ReConnect_Start = 0;
  int _ReConnect_Count = 0;
  String _MQTT_Broker_IPx;

  // ***********************************************************************
  // ***********************************************************************
  bool ReConnect(String ToPublish = "") {

    bool Result;
    if (this->_MQTT_User.length() > 0) {
      Result = MyMQTT->connect(_MQTT_ID.c_str(), this->_MQTT_User.c_str(), this->_MQTT_PWD.c_str(),
                               _Subscription_Out.c_str(), 1, 1, _LWT.c_str());
    } else {
      Result = MyMQTT->connect(_MQTT_ID.c_str(), _Subscription_Out.c_str(), 1, 1, _LWT.c_str());
    }

    if (Result) {
      Set_Signal_LED(4, 100, 100);
      for (int i = 0; i < MAX_MQTT_TOPICS; i++) {
        if (MQTT_Topics[i].length() > 0) {
          MyMQTT->subscribe(MQTT_Topics[i].c_str(), MQTTQOS0);
          Serial.println("MQTT Subscribe: " + MQTT_Topics[i]);
        } else
          break;
      }

      if (ToPublish.length() > 0) {
        MyMQTT->publish(_Subscription_Out.c_str(), ToPublish.c_str());
      }

      return true;
    }

    Serial.println("try reconnect FIN, did not work");

    // **************************************
    //  if more than one broker, toggle between brokers if reconnect necessary
    // **************************************
    if (MQTT_Broker_IP2.length() > 5) {
      int Mode = _ReConnect_Count % 10;
      if (Mode == 5) {
        MyMQTT->setServer(MQTT_Broker_IP2.c_str(), Broker_Port);
        this->_MQTT_Broker_IPx = MQTT_Broker_IP2;
        _ReConnect_Count = 5;
        Serial.print("MQTT-broker: " + MQTT_Broker_IP2);
      } else if (Mode == 0) {
        MyMQTT->setServer(MQTT_Broker_IP.c_str(), Broker_Port);
        this->_MQTT_Broker_IPx = MQTT_Broker_IP;
        _ReConnect_Count = 0;
        Serial.print("MQTT-broker: " + MQTT_Broker_IP);
      }
    }
    _ReConnect_Count += 1;

    return false;
  }
};

#endif
