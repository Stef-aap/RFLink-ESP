
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
    Version_Name = "V" + String(Receiver_ScratchPad_h) + "   Receiver_ScratchPad.h";
    Serial.println("CREATE    " + Version_Name);
    Help_Text = "ScartchPad is bedoeld voor de programmeur die er stukjes code mee kan testen.\n\
De inhoud van deze Receiver is dan ook zeer veranderlijk en \n\
In normaal gebruik moet deze Reeceiver dus altijd zijn uitgeschakeld.";
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
