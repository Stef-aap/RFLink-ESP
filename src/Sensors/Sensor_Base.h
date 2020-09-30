// Version 0.6   20-12-2019, SM
//
// Version 0.5   08-08-2019, SM
//    - json5 naar6, MQTT_Callback changed
// Version 0.4   17-07-2019, SM
//    - Get_Name_Version() removed and replaced with Version_Name
// Version 0.3   12-06-2019, SM
//    - Get_Name_Version() added
// Version 0.2   08-05-2019, SM
//    - globals Wire_SDA, Wire_SCL added
// Version 0.1  original release

#ifndef Sensor_Base_h
#define Sensor_Base_h 0.6

#include <Arduino.h>
#include <ArduinoJson.h>

// ***********************************************************************************
// Defaults for ESP8266 boards
// Since these pins cannot be retrieved, we take over Wire.begin
// ***********************************************************************************
int Wire_SDA = 4;
int Wire_SCL = 5;
void Wire_Print() {
  Serial.print(F(">>>>>>>>> I2C  Clock/Data = "));
  Serial.print(Wire_SCL);
  Serial.print("/");
  Serial.println(Wire_SDA);
}
void Wire_Begin(int SDA, int SCL) {
  Wire_SDA = SDA;
  Wire_SCL = SCL;
  Wire.begin(Wire_SDA, Wire_SCL);
  Wire_Print();
}

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_BaseClass {
public:
  uint16_t Device_ID = -1;
  String Version_Name = "Sensor, Version and Name = UNKNOWN";
  String Help_Text = ""; //"No help information available for this module" ;
  String Device_Firmware = "unknown";
  int Sample_Time_ms = 1000; // The time between consecutive samples
  String _JSON_Sample = "";  // APPEARSELY NOT TO BE PRIVATE
  char msg[1000];
  unsigned long Sample_Time_Last = 0;
  String MQTT_Callback_Topic = "";
  int High_Priority_Loop = 0;
  String _JSON_Short_Header = "";
  String _JSON_Long_Header = "";
  bool Device_Active = true;

  // **********************************************************************************************
  // By defining the method virtual,
  // The method in the derived class (if available) will take over the functionality
  // If no method with the same name is in the derived class, this virtual method will be effective
  // **********************************************************************************************
  virtual void setup() {}

  // **********************************************************************************************
  // **********************************************************************************************
  virtual void Default_Settings(bool Force = false) {
    Serial.println("WARNING: no Default Settings for : " + Version_Name);
  }

  // **********************************************************************************************
  // This method is called after settings have been changed by the webserver.
  // This method can validate the changes and if OK, should transfer the settings.
  // Return should be true ia a restart is necessary
  // **********************************************************************************************
  virtual bool Check_Modified_Settings() { return false; }

  // **********************************************************************************************
  // **********************************************************************************************
  virtual void loop() {
    // ************************************************************
    // Not allowed to do here a print statement
    // because it's possible that the sensor doesn't need a loop !!
    // ************************************************************
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Handle_Serial_Command(String Serial_Command) { return false; }

  // ***********************************************************************
  // The first parameter should always be the sampletime in msec
  // ***********************************************************************
  virtual void Set_Parameters(int Sample_Time_msec) { Sample_Time_ms = Sample_Time_msec; }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  virtual void Get_JSON_Data() {}

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Pushed() { return false; }

  // ***********************************************************************
  // ***********************************************************************
  virtual int Get_JSON_Length() { return 0; }

  // ***********************************************************************
  // Get all the sampled data as a partial JSON string, suited for Luftdaten
  // ***********************************************************************
  virtual String Get_JSON_LuftData() { return ""; }

  // ***********************************************************************
  // ***********************************************************************
  virtual void Send_IR(unsigned long Data) {}

  // ***********************************************************************
  // ***********************************************************************
  virtual void Print_Help() {
    if (Help_Text.length() > 0) {
      Serial.println(Help_Text);
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Hardware_Test(int Test_Nr = 1) {
    Serial.println(F("No Hardware Test for this device available"));
    return true;
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {
    Serial.println("WARNING: no MQTT Callback function implemented for : " + Version_Name);
  }

  // ***********************************************************************
  // it is only created for OKE4
  // ***********************************************************************
  virtual bool Read_SX(uint8_t ID) { return true; }
};
#endif
