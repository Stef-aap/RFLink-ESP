
// Version 0.2   22-02-2020,  SM
//    - cleanup Send_Data
//
// Version 0.1   05-02-2020, SM
//    - initial release
//    - works pretty well
//    - homepage only works as non-ceel files on sdcard
//    - download + graph + ftp don't work on SDcard yet
//
// ***********************************************************************************
// ***********************************************************************************
//  <root>\Arduino\libraries\SdFat\src\FatLib
//    ArduinoFiles.h
//    FatFileSystem.h
//      Replace "File" with "Arduino_File"
//      otherwise conflicts with FS.h
//
// In the meantime switched to the Adafruit forl to SDfat
// and resolved the conflight in the following way

// ***********************************************************************************
// ***********************************************************************************

#ifndef Receiver_SDfat_h
#define Receiver_SDfat_h 0.2

#include "SdFat.h"
#include "sdios.h"

#include "Clients/FS_support.h" // is dat nodig ???
#include "Utilities/My_File_Support.h"

// MUST BE REMOVERS OF FILELIST NO LONGER EXISTS
// ***********************************************************************************
// ***********************************************************************************
void MySort(String String_List[], int Len, bool Reversed) {
  String Temp;
  for (int Start = 1; Start < Len; Start++) {
    for (int i = Start; i < Len; i++) {
      if ((!Reversed && (String_List[i] < String_List[Start - 1])) ||
          (Reversed && (String_List[i] > String_List[Start - 1]))) {
        Temp = String_List[i];
        String_List[i] = String_List[Start - 1];
        String_List[Start - 1] = Temp;
      }
    }
  }
}

// Receiver_SDfat ********************************************************************
// ***********************************************************************************
class _Receiver_SDfat : public _Receiver_BaseClass {

public:
  SdFat SDcard;
  SdFile SDpath;
  SdFile SDfile;

  String Help_Text = "#define Receiver_SDfat  <Filename>\n\
#define Receiver_SDfat  <Filename>  <CS>\n\
    <CS> Chip Select pin, defaults to GPIO4\n\
    <Filename>            defaults to \"temp.txt\"";

  // Receiver_SDfat ********************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_SDfat(int SD_Card_CS = 14, String Filename = "Data.csv", int Max_File_Seconds = 600, int Max_NFile = 10) {
    this->_SD_Card_CS = SD_Card_CS;
    this->_Data_Filename = Filename;
    this->_Max_File_Seconds = Max_File_Seconds;
    this->_Max_NFile = Max_NFile;
    this->Constructor_Finish();
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    _Is_Receiver_SDFat = true;
    Version_Name = "V" + String(Receiver_SDfat_h) + "  ──────  Receiver_SDfat.h";
    Serial.println("\n──────  CREATE  ──────  " + Version_Name);

    int x1 = this->_Data_Filename.indexOf('.');
    _Data_Filename_Prefix = this->_Data_Filename.substring(0, x1) + '_';
    _Data_Filename_Ext = this->_Data_Filename.substring(x1);
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void setup() {
    Serial.print("\n#define Receiver_SDfat  ");
    Serial.print(_Data_Filename + "  ");
    Serial.print(_SD_Card_CS);
    Serial.print("\n    Initializing SD card ... ");

    if (SDcard.begin(_SD_Card_CS, SD_SCK_MHZ(50))) {
      Serial.println("SD-Card initialized.");
      _Card_Present = true;
    } else {
      Serial.println("SD-Card failed, or not present");
      _Card_Present = false;
    }

    Serial.println(Help_Text);
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) {
    if (!_Card_Present) return false;
    SdFile DataFile;

    // *****************************************************
    // create the file if it doesn't exists and filestamp it
    // *****************************************************
    if (!_DataFile_Initialized) {
      this->_Seconds_In_File = Get_LastTime_In_File(_Data_Filename);
      if (this->_Seconds_In_File < 0) {
        DataFile.open(_Data_Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT);
        DataFile.println(JSON_Short_Header);
        // *****************************************************
        // Cancel callback so sync will not change access/modify timestamp
        // *****************************************************
        SdFile::dateTimeCallbackCancel();
        DataFile.timestamp(T_CREATE, Global_DateTime.Year, Global_DateTime.Month, Global_DateTime.Day,
                           Global_DateTime.Hour, Global_DateTime.Minute, Global_DateTime.Second);
        DataFile.timestamp(T_WRITE, Global_DateTime.Year, Global_DateTime.Month, Global_DateTime.Day,
                           Global_DateTime.Hour, Global_DateTime.Minute, Global_DateTime.Second);
        DataFile.timestamp(T_ACCESS, Global_DateTime.Year, Global_DateTime.Month, Global_DateTime.Day,
                           Global_DateTime.Hour, Global_DateTime.Minute, Global_DateTime.Second);
        DataFile.close();
        this->_Seconds_In_File = -(millis() / 1000);
      }
      _DataFile_Initialized = true;
    }

    //******************************************************
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    // FILE_WRITE = read / append
    //******************************************************
    DataFile.open(_Data_Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT);
    if (!DataFile.isOpen()) return false;

    int Seconds = millis() / 1000 + this->_Seconds_In_File;
    int FileSize = DataFile.fileSize();

    bool New_File_Needed = false;
    New_File_Needed = ((this->_Max_File_Seconds > 0) && (Seconds > this->_Max_File_Seconds)) ||
                      ((this->_Max_File_Seconds < 0) && (FileSize > -this->_Max_File_Seconds));

    //******************************************************
    // create new file if needed,
    //    remove the oldest file, if too many
    //******************************************************
    if (New_File_Needed) {
      int Last_FileNr = Get_Last_File_Number();
      String New_Filename;
      if (Last_FileNr >= this->_Max_NFile) {
        New_Filename =
            this->_Data_Filename_Prefix + String(Last_FileNr - this->_Max_NFile + 2) + this->_Data_Filename_Ext;
        this->Delete(New_Filename);
      }

      // *****************
      // create a new file
      // *****************
      New_Filename = this->_Data_Filename_Prefix + String(Get_Last_File_Number() + 1) + this->_Data_Filename_Ext;
      DataFile.rename(New_Filename.c_str());
    }
    DataFile.close();
    DataFile.open(_Data_Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT);

    FileSize = DataFile.fileSize();
    if (FileSize == 0) {
      DataFile.println(JSON_Short_Header);
      this->_Seconds_In_File = -(millis() / 1000);

      // *****************************************************
      // Cancel callback so sync will not change access/modify timestamp
      // *****************************************************
      SdFile::dateTimeCallbackCancel();
      DataFile.timestamp(T_CREATE, Global_DateTime.Year, Global_DateTime.Month, Global_DateTime.Day,
                         Global_DateTime.Hour, Global_DateTime.Minute, Global_DateTime.Second);
      DataFile.timestamp(T_WRITE, Global_DateTime.Year, Global_DateTime.Month, Global_DateTime.Day,
                         Global_DateTime.Hour, Global_DateTime.Minute, Global_DateTime.Second);
      DataFile.timestamp(T_ACCESS, Global_DateTime.Year, Global_DateTime.Month, Global_DateTime.Day,
                         Global_DateTime.Hour, Global_DateTime.Minute, Global_DateTime.Second);
    }

    //******************************************************
    // if the file is available, write to it:
    // takes about 18 msec
    //******************************************************
    if (DataFile.isOpen()) {
      DataFile.println(String(millis() / 1000 + this->_Seconds_In_File) + "\t" + JSON_Short_Data);
      DataFile.close();
      return true;
    }

    //******************************************************
    // if the file isn't open, pop up and return false
    //******************************************************
    else {
      Serial.print("ERROR opening : ");
      Serial.println(_Data_Filename);
      return false;
    }
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  bool Handle_Serial_Command(String Serial_Command) {
    if (!_Card_Present) return false;

    String Filename;
    String LowerCase = Serial_Command;
    LowerCase.toLowerCase();

    //******************************************
    if (LowerCase.startsWith("help")) {
      //******************************************
      Serial.println(_Serial_Commands_Text);
      return false;
    }

    //******************************************
    else if (LowerCase.startsWith("?")) {
      //******************************************
      Serial.println(_Serial_Commands_Text);
      return false;
    }

    //******************************************
    else if (LowerCase.startsWith("dirr")) {
      //******************************************
      this->Get_CWD();
      Filename = Serial_Command.substring(5);
      if (Filename.length() == 0) Filename = "/";
      Serial.println("──────  Recursed Files in :  \"" + Filename + "\"  ──────");
      SdFile SDfile(Filename.c_str(), O_RDONLY);
      SDfile.ls(&Serial, LS_R);
      SDfile.close();
      Serial.println("──────────────────");
      return false;
    }

    //******************************************
    else if (LowerCase.startsWith("dir")) {
      //******************************************
      Filename = Serial_Command.substring(4);
      if (Filename.length() == 0) Filename = "/";

      this->Dir2();
      return false;
    }

    //******************************************
    else if (LowerCase.startsWith("cd")) {
      //******************************************
      String Path = Serial_Command.substring(3);
      Serial.println(Path);

      SDcard.chdir(Path.c_str(), true);
      SDcard.vwd()->getName(_SDcard_Line, sizeof(_SDcard_Line));
      Serial.println(_SDcard_Line);

      Serial.print("============================= CD --> ");
      this->Get_CWD();
      return true;
    }

    //******************************************
    else if (LowerCase.startsWith("md")) {
      //******************************************
      String Path = Serial_Command.substring(3);
      this->MakeDir(Path);
      Serial.print("============================= MD --> " + Path);
      return true;
    }

    //******************************************
    else if (LowerCase.startsWith("dump ")) {
      //******************************************
      Filename = Serial_Command.substring(5);
      this->Dump(Filename);
    }

    //******************************************
    else if (LowerCase.startsWith("dumpall")) {
      //******************************************
      String Path = Serial_Command.substring(8);
      if (Path.length() == 0) Path = "/";
      SDcard.chdir(Path.c_str());
      while (SDfile.openNext(SDcard.vwd(), O_READ)) {
        if (SDfile.isFile()) {
          SDfile.getName(_SDcard_Line, sizeof(_SDcard_Line));
          String Filename = _SDcard_Line;
          SDfile.close();
          this->Dump(Filename);
        } else
          SDfile.close();
      }
    }

    //******************************************
    else if (LowerCase.startsWith("del ")) {
      //******************************************
      Filename = Serial_Command.substring(4);
      SdFile SDfile(Filename.c_str(), O_RDWR);
      if (SDfile.isOpen()) {
        bool Result = SDfile.remove();
        if (Result) {
          Serial.print("============================= Deleted : " + Filename);
        } else {
          Serial.println("ERROR: file not found.");
        }
      }
    }

    //******************************************
    else if (LowerCase == "delall yes") {
      //******************************************
      Serial.println("DELALL YES, not yet implemented");
    }

    //******************************************
    else if (LowerCase.startsWith("create ")) {
      //******************************************
      Filename = Serial_Command.substring(7);
      SdFile SDfile(Filename.c_str(), FILE_WRITE);
      SDfile.write("This is the first line\n");
      SDfile.close();
    }

    //******************************************
    else {
      //******************************************
      return false;
    }
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void Append_2_CSV(String Filename, String Header, String Data) {
    if (!_Card_Present) return;

    SdFile SDfile(Filename.c_str(), O_WRONLY | O_APPEND | O_CREAT);
    if (SDfile.isOpen()) {
      SDfile.println(Data);
      SDfile.close();
    }
  }

  // Receiver_SDfat ********************************************************
private:
  // ***********************************************************************
  String _Data_Filename;
  String _Data_Filename_Prefix;
  String _Data_Filename_Ext;
  int _SD_Card_CS;
  bool _Card_Present = false;
  bool _DataFile_Initialized = false;
  int _Seconds_In_File;
  int _Max_File_Seconds;
  int _Max_NFile;

  char _SDcard_Line[250];

  String _Serial_Commands_Text = "──────  Receiver_SDfat  ──────\n\
dir [FILENAME]  // Display All files in this (or current) directory \ n \
dirr [FILENAME] // Display all files recursively \ n \
cd FILENAME     // change directory \ n \
md FILENAME     // make Dierectory \ n \
dump FILENAME   // Print the contents of a file \ n \
dumpALL         // Print the contents of all files \ n \
create FILENAME // Create file if not already exist, append line text \ n \
del FILENAME    // Delete file \ n \
delALL YES      // TODO Delete ALL files";

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void Get_CWD() {
    if (!_Card_Present) return;

    SDcard.vwd()->getName(_SDcard_Line, sizeof(_SDcard_Line));
    Serial.println("CWD = " + String(_SDcard_Line));
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void Dir(String Path, String Filename = "") {
    if (!_Card_Present) return;

    if (Filename.length() == 0) {
      SDcard.vwd()->getName(_SDcard_Line, sizeof(_SDcard_Line));
      String Path = _SDcard_Line;
    } else
      String Path = Filename;

    Serial.println("──────  Files in :  \"" + Path + "\"  ──────");
    SdFile SDfile(Filename.c_str(), O_RDONLY);
    SDfile.ls(&Serial);
    SDfile.close();
    Serial.println("──────────────────");
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void Dir2(String Path = "/") {
    if (!_Card_Present) return;

    SDcard.chdir(Path.c_str());
    Serial.println("──────  Files in :  \"" + Path + "\"  ──────");
    while (SDfile.openNext(SDcard.vwd(), O_READ)) {
      if (SDfile.isFile()) {
        SDfile.printName(&Serial);
        Serial.print('\t');
        SDfile.printFileSize(&Serial);
        Serial.print('\t');
        SDfile.printModifyDateTime(&Serial);
        Serial.println();
        SDfile.close();
      } else
        SDfile.close();
    }
    Serial.println("──────────────────");
  }

  // **************************************************
  // **************************************************
  bool Delete(String Filename) {
    SDfile.open(Filename.c_str(), O_RDWR);
    if (SDfile.isOpen()) {
      bool Result = SDfile.remove();
      SDfile.close();

      if (Result) {
        Serial.println("────── File removed " + Filename);
      } else {
        Serial.println("ERROR: file not found " + Filename);
      }
    }
    Serial.println("Removing File, doesn't exists " + Filename);
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void Dump(String Filename) {
    if (!_Card_Present) return;

    Serial.println("────── Dump :  " + Filename + "  ──────");
    SdFile SDfile(Filename.c_str(), O_RDONLY);
    if (SDfile.isOpen()) {
      int n;
      while ((n = SDfile.fgets(_SDcard_Line, sizeof(_SDcard_Line))) > 0) {
        Serial.print(_SDcard_Line);
      }
      SDfile.close();
      Serial.println("\n──────────────────");
    } else {
      Serial.println("ERROR: File not found.");
    }
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  void MakeDir(String NewPath) {
    if (!_Card_Present) return;
    SDfile.mkdir(SDcard.vwd(), NewPath.c_str(), true);
  }

  // Receiver_SDfat ********************************************************
  // The file is supposed to be a tab-delimited csv file
  // The first column contains the number of seconds since the start of this file
  // returns the last noted second counter -1 if file does not exist
  // ***********************************************************************
  int Get_LastTime_In_File(String Filename) {
    ifstream sdin(Filename.c_str());
    String Last_Line;

    while (sdin.getline(_SDcard_Line, sizeof(_SDcard_Line), '\n') || sdin.gcount()) {
      if (sdin.fail()) sdin.clear(sdin.rdstate() & ~ios_base::failbit);
      else {
        if (_SDcard_Line[0] != 0) {
          Last_Line = _SDcard_Line;
        }
      }
    }
    int x1 = Last_Line.indexOf('\t');
    if (x1 >= 0) {
      Last_Line = Last_Line.substring(0, x1);
      x1 = Last_Line.toInt();
    }
    return x1;
  }

  // Receiver_SDfat ********************************************************
  // ***********************************************************************
  int Get_Last_File_Number(String Path = "/") {
    int Last_FileNr = 0;
    String Filename;

    SDcard.chdir(Path.c_str());
    while (SDfile.openNext(SDcard.vwd(), O_READ)) {
      if (SDfile.isFile()) {
        SDfile.getName(_SDcard_Line, sizeof(_SDcard_Line));
        Filename = _SDcard_Line;
        if (Filename.startsWith(this->_Data_Filename_Prefix)) {
          int FileNr = Parse_FileNr(Filename);
          if (FileNr > Last_FileNr) Last_FileNr = FileNr;
        }
      }
      SDfile.close();
    }

    return Last_FileNr;
  }
};

#endif
