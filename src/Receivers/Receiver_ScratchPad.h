
#ifndef Receiver_ScratchPad_h
#define Receiver_ScratchPad_h

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_ScratchPad : public _Receiver_BaseClass {

public:
  HardwareSerial *_Serial_SDS011;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_ScratchPad() {
    Version_Name = "V" + String(Receiver_ScratchPad_h) + "  ──────  Receiver_ScratchPad.h";
    Serial.println("\n──────  CREATE  ──────  " + Version_Name);
    Help_Text = "ScartchPad is intended for programmers to test pieces of code.\n\
The content of this receiver is therefore very changeable and \n\
in normal use, this receiver must always be switched off.";
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void setup() {
    _Serial_SDS011 = &Serial_Device;
    _Serial_SDS011->begin(115200);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() { return false; }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) { Serial.println("WARNING: no Default Settings for : " + Version_Name); }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {}

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) {
    _Serial_SDS011->println("en via een pointer");
    Serial.println(">>>>>>>>>>>>>>>>>>>>>> TEST OF SCRATCHPAD <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    return true;
  }

  // ***********************************************************************
  // ***********************************************************************
  void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {}
};
#endif
