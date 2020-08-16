// Version 0.1

#ifndef Sensor_Watchdog_h
#define Sensor_Watchdog_h 0.1

#include "Sensor_Base.h"

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_Watchdog : public _Sensor_BaseClass {

public:
  // ***********************************************************************
  // ***********************************************************************
  _Sensor_Watchdog() {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_Watchdog(int Watchdog_Pin) {
    _External_Watchdog_Pin = Watchdog_Pin;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    Version_Name = "V" + String(Sensor_Watchdog_h) + "   Sensor_Watchdog.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    if (_External_Watchdog_Pin >= 0) {
      Serial.println("++++++++++++++++++ EXTERNAL WATCHDOG on pin = " + String(_External_Watchdog_Pin));
      pinMode(_External_Watchdog_Pin, OUTPUT);
      digitalWrite(_External_Watchdog_Pin, _External_Watchdog_Pin_State);
    } else {
      Serial.println("++++++++++++++++++ NO EXTERNAL WATCHDOG");
    }
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _External_Watchdog_Pin = Settings.Get_Set_Default_Int("Watchdog GPIO", -1, Force);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;
    int Watchdog_Pin = Settings.Read_Int("Watchdog GPIO");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "Watchdog GPIO") {
        // Serial.println ( "JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ  " + String ( New_Value_Int ) + " JJJJJ " +
        // String(Watchdog_Pin)) ;
        if (New_Value_Int != Watchdog_Pin) {
          _My_Settings_Buffer["Watchdog GPIO"] = New_Value_Int;
          Restart = true;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
};

#endif
