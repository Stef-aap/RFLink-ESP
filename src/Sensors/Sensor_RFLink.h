// Version 0.2, 16-04-2020, SM
//   - Transmit_Pin and Receive_Pin passed a s variable (instead of #define)
//
// Version 0.1, 07-04-2020, SM
//   - initial version
//   - 19;print;   added (=print devices)
//   - 19;dir;     now also shows filesize
// ***********************************************************************************
// ***********************************************************************************

#ifndef Sensor_RFLink_h
#define Sensor_RFLink_h 0.2

#ifdef INCLUDE_RECEIVER_MQTT
  #include "Receivers/Receiver_MQTT.h"
#endif

#include "RFLink/RFLink_File.h"
_RFLink_File RFLink_File; // ( "/RFLink.txt" ) ;

byte PKSequenceNumber = 0; // 1 byte packet counter
String Unknown_Device_ID = "";
int Learning_Mode = 0; // always start in production mode
char PreFix[20];

// ****************************************************************************
// used in Raw signal
//
// original: 20=8 bits. Minimal number of bits*2 that need to have been received
//    before we spend CPU time on decoding the signal.
//
// MAX might be a little to low ??
// ****************************************************************************
#define MIN_RAW_PULSES                                                                                                 \
  26 // 20  // =8 bits. Minimal number of bits*2 that need to have been received before we spend CPU time on decoding
     // the signal.
#define MAX_RAW_PULSES 150
// ****************************************************************************
#define MIN_PULSE_LENGTH                                                                                               \
  40 // Pulses shorter than this value in uSec. will be seen as garbage and not taken as actual pulses.
#define SIGNAL_TIMEOUT     7   // Timeout, after this time in mSec. the RF signal will be considered to have stopped.
#define RAW_BUFFER_SIZE    512 // Maximum number of pulses that is received in one go.
#define INPUT_COMMAND_SIZE 60  // Maximum number of characters that a command via serial can be.

String Received_MQTT_Topic;
String Received_MQTT_Payload;
String Line_2_File;

#include "RFLink/RFL_Protocols.h"
#include "RFLink/RFLink_RawSignal.h"

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_RFLink : public _Sensor_BaseClass {

public:
  // _Sensor_RFLink ********************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_RFLink(String FS) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_RFLink(int Receive_Pin, int Transmit_Pin) {
    this->_Receive_Pin = Receive_Pin;
    this->_Transmit_Pin = Transmit_Pin;
    this->Constructor_Finish();
  }

  // _Sensor_RFLink ********************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    Version_Name = "V" + String(Sensor_RFLink_h) + "   Sensor_RFLink.h";
    Serial.println("CREATE    " + Version_Name);

    My_StringSplitter *Splitter = new My_StringSplitter(MQTT_Topic, '/');
    String First_Part = Splitter->getItemAtIndex(0);

    this->MQTT_Callback_Topic = First_Part + "/from_HA/"; // MQTT_Topic_Rec ;
    _RFLink_MQTT_Topic_Send = First_Part + "/from_RFLink/";

    Help_Text = "    >>>>>>> ToDO Help tekst";
  }

  // _Sensor_RFLink ********************************************************
  // ***********************************************************************
  void setup() {

    pinMode(this->_Receive_Pin, INPUT);
    pinMode(this->_Transmit_Pin, OUTPUT);
    digitalWrite(this->_Receive_Pin, INPUT_PULLUP); // pull-up resister on (to prevent garbage)

    // *********   PROTOCOL CLASSES, available and in this order   ************
    RFL_Protocols.Add(new _RFL_Protocol_KAKU(this->_Receive_Pin, this->_Transmit_Pin));
    RFL_Protocols.Add(new _RFL_Protocol_EV1527(this->_Receive_Pin, this->_Transmit_Pin));
    RFL_Protocols.Add(new _RFL_Protocol_Paget_Door_Chime(this->_Receive_Pin, this->_Transmit_Pin));
    RFL_Protocols.setup();

    RawSignal.Time = millis();

    My_StringSplitter *Splitter = new My_StringSplitter(MQTT_Topic, '/');
    String First_Part = Splitter->getItemAtIndex(0);

    this->MQTT_Callback_Topic = First_Part + "/from_HA/"; // MQTT_Topic_Rec ;
    _RFLink_MQTT_Topic_Send = First_Part + "/from_RFLink/";

    RFLink_File.Begin();
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    this->_Receive_Pin = Settings.Get_Set_Default_Int("RFLink Receive  GPIO", 12, Force);
    this->_Transmit_Pin = Settings.Get_Set_Default_Int("RFLink Transmit GPIO", 14, Force);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;

    int New_Value_Int;
    int Receive_Pin = Settings.Read_Int("RFLink Receive  GPIO");
    int Transmit_Pin = Settings.Read_Int("RFLink Transmit GPIO");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = (My_Webserver.arg(i)).toInt();

      if (My_Webserver.argName(i) == "RFLink Receive  GPIO") {
        if (New_Value_Int != Receive_Pin) {
          _My_Settings_Buffer["RFLink Receive  GPIO"] = New_Value_Int;
          Restart = true;
        }
      }

      else if (My_Webserver.argName(i) == "RFLink Transmit GPIO") {
        if (New_Value_Int != Transmit_Pin) {
          _My_Settings_Buffer["RFLink Transmit GPIO"] = New_Value_Int;
          Restart = true;
        }
      }
    }
    return Restart;
  }

  // _Sensor_RFLink ********************************************************
  // ***********************************************************************
  void loop() {
    if (FetchSignal(this->_Receive_Pin)) {
      RFL_Protocols.Decode();
    }

    if ((millis() - this->_Heartbeat_Last_Time) > 60000) {
      String Topic = _RFLink_MQTT_Topic_Send + "Heartbeat";

      String Payload;
      Payload += "{\"Seconds\":";
      Payload += String(millis() / 1000);
      Payload += ", \"RSSI\":";
      Payload += String(WiFi.RSSI());
      Payload += "}";
      My_MQTT_Client->Publish_Without_(Topic, Payload);
      this->_Heartbeat_Last_Time = millis();
    }

    if (Learning_Mode == 9) {
      if (millis() > 2000 + _Learning_Mode_9_LastTime) {
        if (_Learning_Mode_9_State) {
          RFL_Protocols.Home_Command(_Learning_Mode_9_Cmd1);
          Serial.println("LM-9, send " + _Learning_Mode_9_Cmd1);
        } else {
          RFL_Protocols.Home_Command(_Learning_Mode_9_Cmd2);
          Serial.println("LM-9, send " + _Learning_Mode_9_Cmd2);
        }
        _Learning_Mode_9_State = !_Learning_Mode_9_State;
        _Learning_Mode_9_LastTime = millis();
      }
    }

    // ***********************************
    // On Restart, Send Email
    // ***********************************
    if (this->First_Time_After_Restart_Email && (WiFi.status() == WL_CONNECTED)) {
      this->First_Time_After_Restart_Email = false;
      String Email_Subject = (String)_Main_Name + " Warning: Device Restarted";
      Send_Email("", Email_Subject, "", false);
    }

    // ***********************************
    // On restart, Send Special MQTT message
    // ***********************************
    if (this->First_Time_After_Restart_MQTT && My_MQTT_Client->Connected()) {
      String Topic = _RFLink_MQTT_Topic_Send + "Restarted";
      String Payload;
      Payload += "{\"Seconds\":";
      Payload += String(millis() / 1000);
      Payload += ", \"RSSI\":";
      Payload += String(WiFi.RSSI());
      Payload += "}";
      My_MQTT_Client->Publish_Without_(Topic, Payload);
      this->First_Time_After_Restart_MQTT = false;
    }
  }

  // _Sensor_RFLink ********************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {}

  // ***********************************************************************
  // ***********************************************************************
  void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {

    // ************************************************
    // from:   ha/from_HA/ev1527_005df     S02_ON
    // to:     10;EV1527;005DF;2;ON;
    // ************************************************
    if (Topic.startsWith(this->MQTT_Callback_Topic) && not(Topic.endsWith("_"))) {
      Received_MQTT_Topic = Topic;
      Received_MQTT_Payload = Payload;

      String Line = "10;";
      Topic.toUpperCase();

      Topic = Topic.substring(11); //  EV1527_005DF
      int x1 = Topic.indexOf("_");
      Line += Topic.substring(0, x1) + ";" + Topic.substring(x1 + 1) + ";";

      // Payload = SWITCH + ON/OFF  i.e. "S2_OFF",  "SA_ON"
      Payload = Payload.substring(1);
      Payload.replace("_", ";");
      Line += Payload + ";";

      Serial.print("Sensor_RFLink, MQTT Received Topic: ");
      Serial.print(Topic);
      Serial.print("  Payload: " + Payload);
      Serial.println("    Converted: " + Line);

      Line_2_File = "MQTT-Receive  Topic=" + Topic + "   Payload=" + Payload + " - Converted: " + Line;
      RFLink_File.Log_Line(Line_2_File);

      Handle_Serial_Command(Line);
    }
  }

  // _Sensor_RFLink ********************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.println(Learning_Modes_Text);
    Serial.println(Commands_Text);
  }

  // _Sensor_RFLink ********************************************************
  // ***********************************************************************
  bool Handle_Serial_Command(String Serial_Command) {
    String LowerCase = Serial_Command;
    LowerCase.toLowerCase();
    char InputBuffer_Serial[100]; // Buffer for Seriel data
    Serial_Command.toCharArray(InputBuffer_Serial, 100);

    if (LowerCase.startsWith("help")) {
      this->Print_Help();
      return false;
    } else if (LowerCase.startsWith("?")) {
      this->Print_Help();
      return false;
    }

    // *********************************************
    // *********************************************
    else if ((InputBuffer_Serial[0] == 'X') && (Learning_Mode == 1)) {
      if (Unknown_Device_ID.length() > 0) {
        RFLink_File.Add_Device(Unknown_Device_ID);
        RFLink_File.Print_Devices();
      }
      return true;
    }

    // *********************************************
    // *********************************************
    else if ((InputBuffer_Serial[0] == 'Y') && (Learning_Mode == 1)) {
      if (Unknown_Device_ID.length() > 0) {
        RFLink_File.Add_Device("-" + Unknown_Device_ID);
        RFLink_File.Print_Devices();
        RFLink_File.Print_Devices();
      }
      return true;
    }

    // *********************************************
    // 10;   // COMMAND
    // *********************************************
    else if (LowerCase.startsWith("10;")) {

      // *********************************************
      // LIST all Commands
      // *********************************************
      if (strcasecmp(InputBuffer_Serial + 3, "LIST;") == 0) {
        Serial.printf(InputBuffer_Serial, "20;%02X;LIST;\r\n", PKSequenceNumber++);
        this->Print_Help();
      }

      // *********************************************
      // PING
      // Very Important, because this is used by Domoticz
      //    to see if the RFLink is working properly
      // *********************************************
      else if (strcasecmp(InputBuffer_Serial + 3, "PING;") == 0) {
        sprintf(InputBuffer_Serial, "20;%02X;PONG;", PKSequenceNumber++);
        Serial.println(InputBuffer_Serial);
      }

      // *********************************************
      // REBOOT
      // *********************************************
      else if (LowerCase.startsWith("reboot;", 3)) {
        ESP.restart();
      }

      // *********************************************
      // VERSION
      // *********************************************
      else if (strcasecmp(InputBuffer_Serial + 3, "VERSION;") == 0) {
        Serial.printf("20;%02X;VER;", PKSequenceNumber++);
        Serial.print(_Main_Version, 1);
        Serial.println(";");
        RFLink_File.Log_Line("Main Version = " + String(_Main_Version));
      }

      // *********************************************
      // DEBUG = ...
      // *********************************************
      else if (strncasecmp(InputBuffer_Serial + 3, "DEBUG=", 6) == 0) {
        byte kar = InputBuffer_Serial[9];
        Learning_Mode = kar - 0x30;
        Line_2_File = "\r\n=====  Change Learning Mode to : " + String(Learning_Mode);
        RFLink_File.Log_Line(Line_2_File);

        // *********************************************
        // *********************************************
        if (Learning_Mode > 0) {
          Serial.println(Learning_Modes_Text);
        }

        // *********************************************
        // *********************************************
        if (Learning_Mode == 1) {
          RFLink_File.Print_Devices();
        }

        // *********************************************
        // *********************************************
        else if (Learning_Mode == 8) {

          RFL_Protocols.Home_Command(_Learning_Mode_9_Cmd1);
          Serial.println("LM-8, send " + _Learning_Mode_9_Cmd1);
          delay(2000);
          RFL_Protocols.Home_Command(_Learning_Mode_9_Cmd2);
          Serial.println("LM-8, send " + _Learning_Mode_9_Cmd2);
        }
      }

      // *********************************************
      // Unhandled Commands
      // 10;TRISTATEINVERT; => Toggle Tristate ON/OFF inversion
      // 10;RTSCLEAN; => Clean Rolling code table stored in internal EEPROM
      // 10;RTSRECCLEAN=9 => Clean Rolling code record number (value from 0 - 15)
      // 10;RTSSHOW; => Show Rolling code table stored in internal EEPROM (includes RTS settings)
      // 10;RTSINVERT; => Toggle RTS ON/OFF inversion
      // 10;RTSLONGTX; => Toggle RTS long transmit ON/OFF
      //
      // 11;20;0B;NewKaku;ID=000005;SWITCH=2;CMD=ON; => 11;
      //    is the required node info it can be followed by any custom data which will be echoed
      // *********************************************

      // *********************************************
      // All other commands that starts with 10;
      // Handle Generic Commands / Translate protocol data into Nodo text commands
      // *********************************************
      else {
        // 10;EV1527;0005df;2;ON
        if (RFL_Protocols.Home_Command(InputBuffer_Serial)) {
          Received_MQTT_Topic.replace("from_HA", "from_RFLink");
          My_MQTT_Client->Publish_Without_(Received_MQTT_Topic, Received_MQTT_Payload);
        }
      }
      return true;
    }

    // *********************************************
    // 12; if Learning_Mode 1, Add New Device
    // *********************************************
    else if ((Learning_Mode == 1) && (strncmp(InputBuffer_Serial, "12;", 3) == 0)) {
      String Command = String(InputBuffer_Serial);
      int x1 = Command.indexOf(";", 3);
      x1 = Command.indexOf(";", x1 + 1);
      String New = Command.substring(3, x1 + 1);

      RFLink_File.Add_Device(New);
      RFLink_File.Print_Devices();
      return true;
    }

    // *********************************************
    // 19; System Commands
    // *********************************************
    else if (strncmp(InputBuffer_Serial, "19;", 3) == 0) {
      String Command = String(InputBuffer_Serial);
      int x1 = Command.indexOf(";", 3);
      String CMD = Command.substring(3, x1);
      int x2 = Command.indexOf(";", x1 + 1);
      String ARG = Command.substring(x1 + 1, x2);
      String Rest = Command.substring(x1 + 1);

      if (CMD.equalsIgnoreCase("DIR")) {
        File_System.DirList_Print("/");
      } else if (CMD.equalsIgnoreCase("DUMP")) {
        File_System.Dump(ARG);
      } else if (CMD.equalsIgnoreCase("DEL")) {
        File_System.Delete(ARG);
      } else if (CMD.equalsIgnoreCase("CLEAR")) {
        RFLink_File.Clear_Devices();
      } else if (CMD.equalsIgnoreCase("PRINT")) {
        RFLink_File.Print_Devices();
      } else if (CMD.equalsIgnoreCase("REMOVE")) {
        RFLink_File.Remove_Device(Rest);
        RFLink_File.Print_Devices();
      }
      return true;
    }

    // *********************************************
    // 11; Echo the complete Line (without the preceeding "11;"
    // *********************************************
    else if (strncmp(InputBuffer_Serial, "11;", 3) == 0) {
      String Command = String(InputBuffer_Serial);
      Serial.print(Command.substring(3, -1) + "\r\n");
      return true;
    }

    // *********************************************
    // 13; Set CMD1 for CMDs in Learning Mode 8,9
    // 14; Set CMD1 for Transmit Predefined CMDs
    // *********************************************
    else if (LowerCase.startsWith("13;")) {
      if (LowerCase.length() > 4) _Learning_Mode_9_Cmd1 = Serial_Command.substring(3);
      else {
        Serial.println("LM-9, CMD1 = " + _Learning_Mode_9_Cmd1);
        Serial.println("LM-9, CMD2 = " + _Learning_Mode_9_Cmd2);
      }
      return true;
    } else if (LowerCase.startsWith("14;")) {
      if (LowerCase.length() > 4) _Learning_Mode_9_Cmd2 = Serial_Command.substring(3);
      else {
        Serial.println("LM-9, CMD1 = " + _Learning_Mode_9_Cmd1);
        Serial.println("LM-9, CMD2 = " + _Learning_Mode_9_Cmd2);
      }
      return true;
    }
    return false;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  int _Receive_Pin = -1;
  int _Transmit_Pin = -1;
  unsigned long _Heartbeat_Last_Time = 0;
  bool First_Time_After_Restart_Email = true;
  bool First_Time_After_Restart_MQTT = true;

  bool _Learning_Mode_9_State;
  unsigned long _Learning_Mode_9_LastTime = 0;
  // String _Learning_Mode_9_Cmd1 = "10;EV1527;005DF;01;ON;";
  // String _Learning_Mode_9_Cmd2 = "10;EV1527;005DF;02;ON;";
  // String _Learning_Mode_9_Cmd1 = "10;NEWKAKU;2508A7C;0A;ON;";
  // String _Learning_Mode_9_Cmd2 = "10;NEWKAKU;2508A7C;0A;OFF;";
  String _Learning_Mode_9_Cmd1 = "10;NEWKAKU;2508A7C;0B;ON;";
  String _Learning_Mode_9_Cmd2 = "10;NEWKAKU;2508A7C;0B;OFF;";
  // "10;KAKU;02508A7C;0A;ON;"
  // "10;KAKU;02508A7C;0A;OFF;"
  // NewKaku;ID=2508A7C;
  // NewKaku;ID=1C38A04;

  String Learning_Modes_Text = "\
-----  Learning_Modes  -----\r\n\
0 : Production Mode\r\n\
1 : Real Learning Mode\r\n\
2 : one detection  + statistics\r\n\
3 : all detections + statistics\r\n\
4 : display pulstime\r\n\
5 : display pulstime rounded at 30 usec\r\n\
6 : Statistics, Binairy and Hex Results\r\n\
7 : Statistics followed, by normal detection\r\n\
8 : Transmit a predefined sequence\r\n\
9 : same as 8, but indefinitly";

  String Commands_Text = "\n\
10;LIST;          // list all commands\r\n\
10;PING;          // return PONG\r\n\
10;REBOOT;        // reboot RFLink\r\n\
10;VERSION;       // displays version information\r\n\
10;RF_Command;    // Send an RF package\r\n\
10;DEBUG=x;       // Enter Learning/Debug Mode\r\n\
12;Name;ID;       // In Learning_Mode=1 add this device\r\n\
X                 // In Learning_Mode=1 add the last seen Device\r\n\
19;PRINT;         // list all Known Devices\r\n\
19;DIR;           // Directory of the file-system\r\n\
19;DUMP;Filename; // Print the content of the file\r\n\
19:DEL;Filename;  // Delete the file\r\n\
19:CLEAR;         // Delete all registered Devices\r\n\
11;<LINE>         // Complete <LINE> is echoed\r\n\
13;<LINE>         // Command-1 for Learning_Mode 8,9\r\n\
14;<LINE>         // Command-2 for Learning_Mode 8,9";
};

#endif
