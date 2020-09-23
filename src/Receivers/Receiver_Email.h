
// Version 0.1  23-04-2020
//    - original release, only for ESP32

#ifndef Receiver_Email_h
#define Receiver_Email_h 0.1

#ifdef ESP32
  #include "ESP32_MailClient.h"

// should be part of the class
SMTPData Mail_Client;
#else
  #include "Clients/Email_Support.h"
#endif

// _Receiver_Email  ******************************************************************
// ***********************************************************************************
class _Receiver_Email : public _Receiver_BaseClass {
public:
  // _Receiver_Email  ******************************************************
  // ***********************************************************************
  _Receiver_Email() {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Receiver_Email(String xSMTP_Server, int xSMTP_Port, String xSMTP_User, String xSMTP_Password) {
    this->_SMTP_Server = xSMTP_Server;
    this->_SMTP_Port = xSMTP_Port;
    this->_SMTP_User = xSMTP_User;
    this->_SMTP_PWD = xSMTP_Password;
    this->Constructor_Finish();
  }

  // _Sensor_RFLink ********************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    this->_Is_Receiver_Email = true;

    Version_Name = "V" + String(Receiver_Email_h) + "   Receiver_Email.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // _Receiver_Email  ******************************************************
  // ***********************************************************************
  void setup() {}

  // _Receiver_Email  ******************************************************
  // ***********************************************************************
  void Default_Settings(bool Force = false) {
#ifdef __SECRET_SMTP_Server
    this->_SMTP_Server = Settings.Get_Set_Default_String("Email Server", __SECRET_SMTP_Server, Force);
#else
    this->_SMTP_Server = Settings.Get_Set_Default_String("Email Server", "smtp.gmail.com", Force);
#endif

#ifdef __SECRET_SMTP_Port
    this->_SMTP_Port = Settings.Get_Set_Default_Int("Email Port", __SECRET_SMTP_Port, Force);
#else
    this->_SMTP_Port = Settings.Get_Set_Default_Int("Email Port", 465, Force);
#endif

#ifdef __SECRET_SMTP_User
    this->_SMTP_User = Settings.Get_Set_Default_String("Email User", __SECRET_SMTP_User, Force);
#else
    this->_SMTP_User = Settings.Get_Set_Default_String("Email User", "User@gmail.com", Force);
#endif

#ifdef __SECRET_SMTP_PWD
    this->_SMTP_PWD = Settings.Get_Set_Default_String("$Email Password", __SECRET_SMTP_PWD, Force);
#else
    this->_SMTP_PWD = Settings.Get_Set_Default_String("$Email Password", "Email Password", Force);
#endif

#ifdef __SECRET_SMTP_MailTo
    this->_SMTP_MailTo = Settings.Get_Set_Default_String("Email To:", __SECRET_SMTP_MailTo, Force);
#else
    this->_SMTP_MailTo = Settings.Get_Set_Default_String("Email To:", this->_SMTP_User, Force);
#endif
  }

  // _Receiver_Email  ******************************************************
  // ***********************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;

    int New_Value_Int;
    String New_Value;

    String xSMTP_Server = Settings.Read_String("Email Server");
    int xSMTP_Port = Settings.Read_Int("Email Port");
    String xSMTP_User = Settings.Read_String("Email User");
    String xSMTP_PWD = Settings.Read_String("$Email Password");
    String xSMTP_MailTo = Settings.Read_String("Email To:");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value = My_Webserver.arg(i);
      New_Value_Int = (My_Webserver.arg(i)).toInt();

      if (My_Webserver.argName(i) == "Email Server") {
        if (New_Value != xSMTP_Server) {
          _My_Settings_Buffer["Email Server"] = New_Value;
        }
      }

      else if (My_Webserver.argName(i) == "Email Port") {
        if (New_Value_Int != xSMTP_Port) {
          _My_Settings_Buffer["Email Port"] = New_Value_Int;
        }
      }

      else if (My_Webserver.argName(i) == "Email User") {
        if (New_Value != xSMTP_User) {
          _My_Settings_Buffer["Email User"] = New_Value;
        }
      }

      else if (My_Webserver.argName(i) == "$Email Password") {
        if (New_Value != xSMTP_PWD) {
          _My_Settings_Buffer["$Email Password"] = New_Value;
        }
      }

      else if (My_Webserver.argName(i) == "Email To:") {
        if (New_Value != xSMTP_MailTo) {
          _My_Settings_Buffer["Email To:"] = New_Value;
        }
      }
    }
    return Restart;
  }

// _Receiver_Email  ******************************************************
// ***********************************************************************
#ifdef ESP32
  void Mail_Callback(SendStatus msg) { Serial.println(msg.info()); }
#endif

  // _Receiver_Email  ******************************************************
  // ***********************************************************************
  void Send_Email(String MailTo, String Subject, String Body, bool HTML_Format = false) {
#ifdef ESP32
    if (WiFi.status() == WL_CONNECTED) {
      Mail_Client.setLogin(this->_SMTP_Server, this->_SMTP_Port, this->_SMTP_User, this->_SMTP_PWD);
      Mail_Client.setSTARTTLS(true);
      Mail_Client.setSender(_Main_Name, this->_SMTP_User);
      Mail_Client.setPriority("High");
      Mail_Client.setSubject(Subject);
      Mail_Client.setMessage(Body, HTML_Format);
      if (MailTo.length() > 0) Mail_Client.addRecipient(MailTo);
      else
        Mail_Client.addRecipient(this->_SMTP_User);

      if (MailClient.sendMail(Mail_Client)) {
        Serial.println("Email send: " + Subject);
      } else {
        Serial.println("ERROR: Receiver_Email, sending Email," + MailClient.smtpErrorReason());
      }

      Mail_Client.empty();
    }
#endif
  }
  // ***********************************************************************
private:
  // ***********************************************************************
  String _SMTP_Server = "";
  int _SMTP_Port;
  String _SMTP_User;
  String _SMTP_PWD;
  String _SMTP_MailTo;

  bool Email_Do_Send = true;
};
#endif
