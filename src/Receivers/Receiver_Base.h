
// Version 0.2, 20-12-2019, SM
//
// Version 0.1  original release

#ifndef Receiver_Base_h
#define Receiver_Base_h 0.2

#include <Arduino.h>
#include <ArduinoJson.h>

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_BaseClass {
public:
  int High_Priority_Loop = 0;
  String Name = "Unknown";
  String Version_Name = "Receiver, Version and Name = UNKNOWN";
  String Help_Text = ""; //"No help information available for this module" ;
  String MQTT_Callback_Topic = "";
  bool Character_Display = false;
  bool _Is_Receiver_SDFat = false;
  bool _Is_Receiver_Email = false;
  bool Device_Active = true;

  // **********************************************************************************************
  // By defining the method virtual,
  // The method in the derived class (if available) will take over the functionality
  // If no method with the same name is in the derived class, this virtual method will be effective
  // **********************************************************************************************
  virtual void setup() {}

  // **********************************************************************************************
  // This method is called after settings have been changed by the webserver.
  // This method can validate the changes and if OK, should transfer the settings.
  // Return should be true ia a restart is necessary
  // **********************************************************************************************
  virtual bool Check_Modified_Settings() { return false; }

  // **********************************************************************************************
  // **********************************************************************************************
  virtual void Default_Settings(bool Force = false) {
    Serial.println("WARNING: no Default Settings for : " + Version_Name);
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual void loop() {}

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Handle_Serial_Command(String Serial_Command) { return false; }

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Send_Data(String JSON_Message) {
    Serial.print("Send_Data is not implemented for this receiver ");
    Serial.println(this->Name);
    return true;
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {
    Serial.println("WARNING: no MQTT Callback function implemented for : " + Version_Name);
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual void Print_Help() {
    if (Help_Text.length() > 0) {
      Serial.println(Help_Text);
    }
  }

  // ***********************************************************************
  // it is created only for debugging
  // ***********************************************************************
  virtual bool Publish(String Topic, String Payload) { return true; }
  virtual bool Publish_Without_(String Topic, String Payload) { return true; }
  virtual bool Connected() { return true; }

  // ***********************************************************************
  // equally for SSD1306
  // ***********************************************************************
  virtual void Show(String Line, int x = -1, int y = -1, int Len = -1) {}
};
#endif
