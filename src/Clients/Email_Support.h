

// Version 0.1  25-04-2020
//    - special for ESP8266, gmail works on port = 465

#ifndef Email_Support_h
#define Email_Support_h 0.1

// ***********************************************************************************
// Encode64 is based on the code from AlertMe
// ***********************************************************************************
const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz"
                                    "0123456789+/";

// ***********************************************************************************
inline void a3_to_a4(unsigned char *a4, unsigned char *a3) {
  a4[0] = (a3[0] & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}

// ***********************************************************************************
int base64_encode(char *output, char *input, int inputLen) {
  int i = 0, j = 0;
  int encLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];

  while (inputLen--) {
    a3[i++] = *(input++);
    if (i == 3) {
      a3_to_a4(a4, a3);

      for (i = 0; i < 4; i++) {
        output[encLen++] = pgm_read_byte(&b64_alphabet[a4[i]]);
      }

      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) {
      a3[j] = '\0';
    }

    a3_to_a4(a4, a3);

    for (j = 0; j < i + 1; j++) {
      output[encLen++] = pgm_read_byte(&b64_alphabet[a4[j]]);
    }

    while ((i++ < 3)) {
      output[encLen++] = '=';
    }
  }
  output[encLen] = '\0';
  return encLen;
}

// ***********************************************************************************
int base64_enc_length(int plainLen) {
  int n = plainLen;
  return (n + 2 - ((n + 2) % 3)) / 3 * 4;
}

// ***********************************************************************************
const char *encode64_f(char *Line, uint8_t len) {
  static char encoded[256];
  // note input is consumed in this step: it will be empty afterwards
  base64_encode(encoded, Line, len);
  return encoded;
}

// ****************************************************************************
// ****************************************************************************
String Encode64(String Line) {
  const int MaxLen = 100;

  char Copy[MaxLen];
  Line.toCharArray(Copy, MaxLen);

  String Result = encode64_f(Copy, Line.length());
  return Result;
}

#ifndef ESP32
  #include <WiFiClientSecure.h>

// ***********************************************************************************
bool Email_Wait(WiFiClientSecure &Mail_Client, String Response = "", uint16_t timeOut = 10000) {
  uint32_t ts = millis();
  while (!Mail_Client.available()) {
    if (millis() > (ts + timeOut)) {
      Serial.println("ERROR: Email_Wait timed out");
      return false;
    }
    yield();
  }
  String Server_Response = Mail_Client.readStringUntil('\n');
  Serial.println("Waiting for: " + Response + "     Received: " + Server_Response);
  if ((Response.length() > 0) && Server_Response.indexOf(Response) == -1) return false;
  return true;
}

// ***********************************************************************************
// ***********************************************************************************
class _Email_Client_Class {
public:
  // _Email_Client_Class  **************************************************
  // ***********************************************************************
  _Email_Client_Class(String xSMTP_Server, int xSMTP_Port, String xSMTP_User, String xSMTP_PWD, String xSMTP_MailTo) {
    this->_SMTP_Server = xSMTP_Server;
    this->_SMTP_Port = xSMTP_Port;
    this->_SMTP_User = xSMTP_User;
    this->_SMTP_PWD = xSMTP_PWD;
    this->_SMTP_MailTo = xSMTP_MailTo;
  }
  _Email_Client_Class(String xSMTP_Server, int xSMTP_Port, String xSMTP_User, String xSMTP_PWD) {
    this->_SMTP_Server = xSMTP_Server;
    this->_SMTP_Port = xSMTP_Port;
    this->_SMTP_User = xSMTP_User;
    this->_SMTP_PWD = xSMTP_PWD;
    this->_SMTP_MailTo = xSMTP_User;
  }

  // _Email_Client_Class  **************************************************
  // ***********************************************************************
  bool Send_Mail(String MailTo, String Subject, String Message, bool HTML_Format = false) {

    if (WiFi.status() != WL_CONNECTED) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(__SECRET_Wifi_Name, __SECRET_Wifi_PWD);
      Serial.println("..... Trying to connect to " + String(__SECRET_Wifi_Name));
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println();
    }

    Serial.println("Connecting to mailserver:" + this->_SMTP_Server + "   Port:" + String(this->_SMTP_Port));

    WiFiClientSecure Email_Client;
    Email_Client.setInsecure();
    delay(1000);

    if (!Email_Client.connect(this->_SMTP_Server, this->_SMTP_Port)) {
      Serial.println("Could not connect to mail server:" + this->_SMTP_Server + "    Port:" + String(this->_SMTP_Port));
      return false;
    }

    if (!Email_Wait(Email_Client, "220")) {
      Serial.println("Connection Error");
      return false;
    }

    Email_Client.println("HELO friend");
    if (!Email_Wait(Email_Client, "250")) {
      Serial.println("identification error");
      return false;
    }

    Email_Client.println("AUTH LOGIN");
    Email_Wait(Email_Client, "");

    Email_Client.println(Encode64(this->_SMTP_User));
    Email_Wait(Email_Client, "");

    Email_Client.println(Encode64(this->_SMTP_PWD));
    if (!Email_Wait(Email_Client, "235")) {
      Serial.println("SMTP AUTH error");
      return false;
    }

    String mailFrom = "MAIL FROM: <MiRa@gmail.com>";
    Email_Client.println(mailFrom);
    Email_Wait(Email_Client, "");

    String Recipient = "RCPT TO: <" + MailTo + ">";
    Email_Client.println(Recipient);
    Email_Wait(Email_Client, "");

    Email_Client.println("DATA");
    if (!Email_Wait(Email_Client, "354")) {
      Serial.println("SMTP DATA error");
      return false;
    }

    String From = "From: <" + this->_SMTP_User + ">";
    Email_Client.println(From);
    String To = "To: <" + MailTo + ">";
    Email_Client.println(To);

    Email_Client.print("Subject: ");
    Email_Client.println(Subject);

    Email_Client.println("Mime-Version: 1.0");
    Email_Client.println("Content-Type: text/html; charset=\"UTF-8\"");
    Email_Client.println("Content-Transfer-Encoding: 7bit");
    Email_Client.println();
    String Body = Message;
    if (HTML_Format) Body = "<!DOCTYPE html><html lang=\"en\">" + Message + "</html>";
    Email_Client.println(Body);
    Email_Client.println(".");
    if (!Email_Wait(Email_Client, "250")) {
      Serial.println("Sending message error");
      return false;
    }

    Email_Client.println("QUIT");
    if (!Email_Wait(Email_Client, "221")) {
      Serial.println("SMTP QUIT error");
      return false;
    }
    return true;
  }

  bool Send_Mail(String Subject, String Message) { return this->Send_Mail(this->_SMTP_MailTo, Subject, Message); }

  // _Email_Client_Class  **************************************************
  // ***********************************************************************
private:
  String _SMTP_Server;
  int _SMTP_Port;
  String _SMTP_User;
  String _SMTP_PWD;
  String _SMTP_MailTo;
};

#endif
#endif
