
// Version 0.1,  28-08-2019, SM

#ifndef Receiver_SDMMC_h
#define Receiver_SDMMC_h 0.1

#include "SD_MMC_support.h"

//#include "ESP32FtpServer.h"
// FtpServer32 ftpSrv_SD_MMC ;

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_SDMMC : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_SDMMC(int FS) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Receiver_SDMMC(String Filename = "/Data.csv", long Max_File_Seconds = 24 * 60 * 60, int Max_NFile = 10) {
    _Filename = Filename;
    _Max_File_Seconds = Max_File_Seconds;
    _Max_NFile = Max_NFile;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    if (!_Filename.startsWith("/")) _Filename = "/aap.csv";

    Version_Name = "V" + String(Receiver_SDMMC_h) + "   Receiver_SDMMC.h";
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
    if (!SD_MMC.begin()) {
      Serial.println("    Card Mount Failed");
      return;
    }
    uint8_t cardType = SD_MMC.cardType();

    if (cardType == CARD_NONE) {
      Serial.println("No SD_MMC card attached");
      return;
    }

    Serial.print("SD_MMC Card Type: ");
    if (cardType == CARD_MMC)
      Serial.println("MMC");
    else if (cardType == CARD_SD)
      Serial.println("SDSC");
    else if (cardType == CARD_SDHC)
      Serial.println("SDHC");
    else
      Serial.println("UNKNOWN");

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

    int Result = SD_MMC_System.Begin(_Filename, _Max_File_Seconds, _Max_NFile);
    Serial.println("Mount SD_MMC-FileSytem (including FTP support NOT YET): " + String(Result));

    // set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
    // ftpSrv.begin ( "esp8266", "esp8266" );    //username, password for ftp.
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _Filename = Settings.Get_Set_Default_String("SD_MMC Filename", "/Data.csv", Force);
    _Max_File_Seconds = Settings.Get_Set_Default_Int("SD_MMC File Seconds", 24 * 60 * 60, Force);
    _Max_NFile = Settings.Get_Set_Default_Int("SD_MMC Max N Files", 24, Force);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;

    String New_Value;
    int New_Value_Int;
    String Filename = Settings.Read_String("SD_MMC Filename");
    int FileSeconds = Settings.Read_Int("SD_MMC File Seconds");
    int File_NMax = Settings.Read_Int("SD_MMC Max N Files");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value = My_Webserver.arg(i);
      New_Value_Int = New_Value.toInt();

      if (My_Webserver.argName(i) == "SD_MMC Filename") {
        if (New_Value != Filename) {
          _My_Settings_Buffer["SD_MMC Filename"] = New_Value;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "SD_MMC File Seconds") {
        if (New_Value_Int != FileSeconds) {
          _My_Settings_Buffer["SD_MMC File Seconds"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "SD_MMC Max N Files") {
        if (New_Value_Int != File_NMax) {
          _My_Settings_Buffer["SD_MMC Max N Files"] = New_Value_Int;
          Restart = true;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    //      ftpSrv_SD_MMC.handleFTP();
  }

  // ***********************************************************************
  //   {"FTP":"ON"}   {"FTP":"OFF"}
  // ***********************************************************************
  void MQTT_Callback(String Topic, String Payload, DynamicJsonDocument root) {
    // void MQTT_Callback ( String Payload, JsonObject &root ) {
    Serial.println("FTPFTPFTPFTPFTP:::::::::::::::" + Payload);
    Serial.println("Loop Priority was : " + String(Loop_Priority));
    String FTP = root["FTP"];
    if (FTP == "ON")
      Loop_Priority = 1;
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
      SD_MMC_System.DirList_Print(Filename);
      //        SD_MMC_System.DirList_Print ( SD_MMC, Filename ) ;
      return false;
    } else if (LowerCase.startsWith("copy ")) {
      Filename = Serial_Command.substring(5);
      SD_MMC_System.Copy(Filename);
    } else if (LowerCase.startsWith("move ")) {
      Filename = Serial_Command.substring(5);
      SD_MMC_System.Move(Filename);
    } else if (LowerCase.startsWith("dump ")) {
      Filename = Serial_Command.substring(5);
      SD_MMC_System.Dump(Filename);
    } else if (LowerCase.startsWith("dumpall")) {
      SD_MMC_System.DumpAll();
    } else if (LowerCase.startsWith("del ")) {
      Filename = Serial_Command.substring(4);
      SD_MMC_System.Delete(Filename);
    } else if (LowerCase == "delall yes") {
      SD_MMC_System.Delete_All();
    } else if (LowerCase.startsWith("create ")) {
      Filename = Serial_Command.substring(7);
      SD_MMC_System.Append_File(Filename, "This is the first line\n");
    } else {
      return false;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Send_Data(String JSON_Message) {
    Serial.println("SD_MMC: " + JSON_Short_Data);
    SD_MMC_System.Create_CSV_File_Nr(_Filename, JSON_Short_Header + "\n");
    SD_MMC_System.Append_File(_Filename, JSON_Short_Data + "\n");
    return true;
  }
  // ***********************************************************************************
  // ***********************************************************************************
private:
  long _Max_File_Seconds;
  int _Max_NFile;
  String _Filename;

  String _Serial_Commands_Text = "======  Receiver_SDMMC  ======\n\
dir [FILENAME]  // Display een lijst van alle bestanden die beginnen met Filename\n\
dump FILENAME   // Print de inhoude van een bestand\n\
dumpALL         // Print de inhoud van alle bestanden\n\
create FILENAME // Create bestand als nog niet bestaat , append regel text\n\
del    FILENAME // Delete bestand\n\
delALL YES      // Delete ALL bestanden";
};

#endif
