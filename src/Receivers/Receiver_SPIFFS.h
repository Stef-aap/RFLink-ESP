// Version 0.4,  09-04-2020, SM
//    - if filename == "" no recordig in Send_Data
//
// Version 0.3,  14_06-2019, SM
//    - "delall yes" werkte niet
// Version 0.2,  23-05-2019, SM
//    - 1 file per tijdseenheid of grootte, en als vol begint te raken oudste file weggooien
//
// Version 0.1,  23-05-2019, SM

#ifndef Receiver_SPIFFS_h
#define Receiver_SPIFFS_h 0.3

#include "Clients/FS_support.h"

//#define FTP_DEBUG
//#include <ESP8266FtpServer.h>
#ifdef YES_INCLUDE_FTPSERVER
  #include "Clients/My_FtpServer.h"
FtpServer ftpSrv; // set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
#endif
/*
...................
Connected to MiRa_Home_Router
IP address: 192.168.0.4
SPIFFS opened!
Ftp server waiting for connection on port 21
Client connected!
USER esp8266
PASS esp8266
OK. Waiting for commands.
SYST
FEAT
PWD
TYPE A
PASV
Connection management set to passive
Data port set to 50009
LIST -a
ftpdataserver client....
client disconnected
Ftp server waiting for connection on port 21
*/

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_SPIFFS : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  //   if Filename == "" then no recodring in Send_Data
  // ***********************************************************************
  _Receiver_SPIFFS(int FS) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Receiver_SPIFFS(String Filename = "/Data.csv", long Max_File_Seconds = 24 * 60 * 60, int Max_NFile = 10) {
    this->_Filename = Filename;
    this->_Max_File_Seconds = Max_File_Seconds;
    this->_Max_NFile = Max_NFile;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    if (!_Filename.startsWith("/")) _Filename = "/aap.csv";

    Version_Name = "V" + String(Receiver_SPIFFS_h) + "  ======  Receiver_SPIFFS.h";
    Serial.println("CREATE    " + Version_Name);
    Help_Text = "    FTP support: Username = esp8266 / Password = esp8266";
    Help_Text += _Serial_Commands_Text;
    Help_Text += "\n==========  Filename = " + _Filename;
    Help_Text += "    File_Seconds = " + String(_Max_File_Seconds);
    Help_Text += "    Max_N_Files = " + String(_Max_NFile) + "\n";

    MQTT_Callback_Topic = MQTT_Topic;
    High_Priority_Loop = 5;
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void setup() {
    bool Result = File_System.Begin(_Filename, _Max_File_Seconds, _Max_NFile);
    Serial.println("Mount SPI-FileSytem (including FTP support): " + String(Result));

#ifdef YES_INCLUDE_FTPSERVER
    ftpSrv.begin("esp8266", "esp8266"); // username, password for ftp.
#endif
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _Filename = Settings.Get_Set_Default_String("SPIFFS Filename", "/Data.csv", Force);
    _Max_File_Seconds = Settings.Get_Set_Default_Int("SPIFFS File Seconds", 3600, Force);
    _Max_NFile = Settings.Get_Set_Default_Int("SPIFFS Max N Files", 24, Force);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;

    String New_Value;
    int New_Value_Int;
    String Filename = Settings.Read_String("SPIFFS Filename");
    int FileSeconds = Settings.Read_Int("SPIFFS File Seconds");
    int File_NMax = Settings.Read_Int("SPIFFS Max N Files");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value = My_Webserver.arg(i);
      New_Value_Int = New_Value.toInt();

      if (My_Webserver.argName(i) == "SPIFFS Filename") {
        if (New_Value != Filename) {
          _My_Settings_Buffer["SPIFFS Filename"] = New_Value;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "SPIFFS File Seconds") {
        if (New_Value_Int != FileSeconds) {
          _My_Settings_Buffer["SPIFFS File Seconds"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "SPIFFS Max N Files") {
        if (New_Value_Int != File_NMax) {
          _My_Settings_Buffer["SPIFFS Max N Files"] = New_Value_Int;
          Restart = true;
        }
      }
    }
    return Restart;
  }

// ***********************************************************************
// ***********************************************************************
#ifdef YES_INCLUDE_FTPSERVER
  void loop() { ftpSrv.handleFTP(); }
#endif

  // ***********************************************************************
  //   {"FTP":"ON"}   {"FTP":"OFF"}
  // ***********************************************************************
  void MQTT_Callback(String Payload, DynamicJsonDocument root) {
    Serial.println("FTPFTPFTPFTPFTP:::::::::::::::" + Payload);
    Serial.println("Loop Priority was : " + String(Loop_Priority));
    String FTP = root["FTP"];
    if (FTP == "ON") Loop_Priority = 1;
    else if (FTP == "OFF") {
      Loop_Priority = 0;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Handle_Serial_Command(String Serial_Command) {
    String Filename;
    String LowerCase = Serial_Command;
    LowerCase.toLowerCase();

    if (LowerCase.startsWith("help")) {
      Serial.println(_Serial_Commands_Text);
      return false;
    } else if (LowerCase.startsWith("?")) {
      Serial.println(_Serial_Commands_Text);
      return false;
    } else if (LowerCase.startsWith("dir")) {
      Filename = Serial_Command.substring(4);
      File_System.DirList_Print(Filename);
      return false;
    } else if (LowerCase.startsWith("dump ")) {
      Filename = Serial_Command.substring(5);
      File_System.Dump(Filename);
    } else if (LowerCase.startsWith("dumpall")) {
      File_System.DumpAll();
    } else if (LowerCase.startsWith("del ")) {
      Filename = Serial_Command.substring(4);
      File_System.Delete(Filename);
    } else if (LowerCase == "delall yes") {
      File_System.Delete_All();
    } else if (LowerCase.startsWith("create ")) {
      Filename = Serial_Command.substring(7);
      File_System.Append_File(Filename, "This is the first line\n");
    } else {
      return false;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) {
    if (this->_Filename.length() > 0) {
      File_System.Create_CSV_File_Nr(_Filename, JSON_Short_Header + "\n");
      File_System.Append_File(_Filename,
                              String(millis() / 1000 + File_System._Offset_Seconds) + "\t" + JSON_Short_Data + "\n");
    }
    return true;
  }

  // ***********************************************************************************
  // ***********************************************************************************
private:
  long _Max_File_Seconds;
  int _Max_NFile;
  String _Filename;

  String _Serial_Commands_Text = "======  Receiver_SPIFFS  ======\r\n\
dir [FILENAME]  // Display een lijst van alle bestanden die beginnen met Filename\r\n\
copy FILENAME   // Kopieer bestand naar de USB poort\r\n\
move FILENAME   // Kopieer bestand naar de USB poort en delete bestand\r\n\
dump FILENAME   // Print de inhoude van een bestand\r\n\
dumpALL         // Print de inhoud van alle bestanden\r\n\
create FILENAME // Create bestand als nog niet bestaat , append regel text\r\n\
del    FILENAME // Delete bestand\r\n\
delALL YES      // Delete ALL bestanden";
};

#endif
