
// Version 0.3,  15-08-2019, SM
// Version 0.1,  6-5-2019, SM

#ifndef Receiver_Fijnstof_Conditionering_h
#define Receiver_Fijnstof_Conditionering_h 0.3

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_Fijnstof_Conditionering : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_Fijnstof_Conditionering() {
    this->Default_Settings();
    Version_Name = "V" + String(Receiver_Fijnstof_Conditionering_h) + "   Receiver_Fijnstof_Conditionering.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {}

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _Delay = Settings.Get_Set_Default_Int("FSC Delay", 100, Force);
    _Tcool = Settings.Get_Set_Default_Int("FSC Tcool", 15, Force);
    _Theat = Settings.Get_Set_Default_Int("FSC Theat", 50, Force);
    _RHcool = Settings.Get_Set_Default_Int("FSC RHcool", 50, Force);
    _RHheat = Settings.Get_Set_Default_Int("FSC RHheat", 70, Force);
    _Thyst = Settings.Get_Set_Default_Int("FSC Thyst", 5, Force);
    _RHhyst = Settings.Get_Set_Default_Int("FSC RHhyst", 5, Force);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int Delay = Settings.Read_Int("FSC Delay");
    int Tcool = Settings.Read_Int("FSC Tcool");
    int Theat = Settings.Read_Int("FSC Theat");
    int RHcool = Settings.Read_Int("FSC RHcool");
    int RHheat = Settings.Read_Int("FSC RHheat");
    int Thyst = Settings.Read_Int("FSC Thyst");
    int RHhyst = Settings.Read_Int("FSC RHhyst");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();
      if (My_Webserver.argName(i) == "FSC Delay") {
        if (New_Value_Int != Delay) _My_Settings_Buffer["FSC Delay"] = New_Value_Int;
      } else if (My_Webserver.argName(i) == "FSC Tcool") {
        if (New_Value_Int != Tcool) _My_Settings_Buffer["FSC Tcool"] = New_Value_Int;
      } else if (My_Webserver.argName(i) == "FSC Theat") {
        if (New_Value_Int != Theat) _My_Settings_Buffer["FSC Theat"] = New_Value_Int;
      } else if (My_Webserver.argName(i) == "FSC RHcool") {
        if (New_Value_Int != RHcool) _My_Settings_Buffer["FSC RHcool"] = New_Value_Int;
      } else if (My_Webserver.argName(i) == "FSC RHheat") {
        if (New_Value_Int != RHheat) _My_Settings_Buffer["FSC RHheat"] = New_Value_Int;
      } else if (My_Webserver.argName(i) == "FSC Thyst") {
        if (New_Value_Int != Thyst) _My_Settings_Buffer["FSC Thyst"] = New_Value_Int;
      } else if (My_Webserver.argName(i) == "FSC RHhyst") {
        if (New_Value_Int != RHhyst) _My_Settings_Buffer["FSC RHhyst"] = New_Value_Int;
      }
    }

    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String Payload) { return true; }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {}

  // ***********************************************************************
private:
  // ***********************************************************************

  char _msg[500];
  int _Delay;
  int _Tcool;
  int _Theat;
  int _RHcool;
  int _RHheat;
  int _Thyst;
  int _RHhyst;
};

#endif
