// Version 0.2,  14-04-2010, SM
//    - Get_Ordered_DirList deleted
//    - HTML_File_CheckList only sends html if anay file exists
//
// Version 0.1,  29-08-2019, SM (based on FS_support 0.6)
//

#ifndef SD_MMC_support_h
#define SD_MMC_support_h 0.2

#include "FS.h"

#ifdef ESP32
  #include "SD_MMC.h"
#endif

// FILELIEST NO LONGER EXISTS

// ***********************************************************************************
// ***********************************************************************************
class _SD_MMC_class {
public:
  // **************************************************
  // **************************************************
  _SD_MMC_class() {}

  // **************************************************
  // **************************************************
  bool Begin(String Filename = "", long Max_File_Time = 24 * 60 * 60, int Max_NFile = 10) {
    bool Result;
    Result = SD_MMC.begin();

#ifdef ESP32
    if (Result) {
      if (_Parse_Filename(Filename)) {
        String Last_File_Pre = _Temp_Last_File_Pre;
        String Last_File_Post = _Temp_Last_File_Post;
        int Last_File_Nr = 0;
        String FileFound;
        fs::File dir = SD_MMC.open("/");
        fs::File file = dir.openNextFile();
        while (file) {
          FileFound = file.name();
          if (_Parse_Filename(FileFound)) {
            if (_Temp_Last_File_Nr > Last_File_Nr) {
              Last_File_Nr = _Temp_Last_File_Nr;
            }
          }
          file = dir.openNextFile();
        }
        _Last_Filename = Last_File_Pre + String(Last_File_Nr) + Last_File_Post;
        _Last_File_Pre = Last_File_Pre;
        _Last_File_Post = Last_File_Post;
        _Last_File_Nr = Last_File_Nr;
        Serial.println("Last File (MMC) = " + _Last_Filename);
      }
    }
#else
    if (Result) {
      if (_Parse_Filename(Filename)) {
        String Last_File_Pre = _Temp_Last_File_Pre;
        String Last_File_Post = _Temp_Last_File_Post;
        int Last_File_Nr = 0;
        String FileFound;

        Dir dir = SD_MMC.openDir(Last_File_Pre);
        while (dir.next()) {
          FileFound = dir.fileName();
          if (_Parse_Filename(FileFound)) {
            if (_Temp_Last_File_Nr > Last_File_Nr) {
              Last_File_Nr = _Temp_Last_File_Nr;
            }
          }
        }
        _Last_Filename = Last_File_Pre + String(Last_File_Nr) + Last_File_Post;
        _Last_File_Pre = Last_File_Pre;
        _Last_File_Post = Last_File_Post;
        _Last_File_Nr = Last_File_Nr;
        Serial.println("Last File = (MMC) " + _Last_Filename);
      }
    }
#endif

    _Max_FileSize = 0;
    _Max_FileTime = 0;
    if (Max_File_Time > 0) {
      _Max_FileTime = 1000 * Max_File_Time;
    } else {
      _Max_FileSize = -Max_File_Time;
    }
    _Max_NFile = Max_NFile - 2;
    if (_Max_NFile < 0) {
      _Max_NFile = 0;
    }

    return Result;
  }

  // **************************************************
  // **************************************************
  String DirList(String Path = "/") {
    String Result = "";
// **************************************************
// ESP32
// **************************************************
#ifdef ESP32
    fs::File dir = SD_MMC.open("/");
    fs::File file = dir.openNextFile();
    Serial.println(">>>>>");
    while (file) {
      Serial.println(file.name());
      Result += String(file.name()).substring(1) + "\n";
      file = dir.openNextFile();
    }

// **************************************************
// ESP8266
// **************************************************
#else
    Dir dir = SD_MMC.openDir("/");
    Serial.println(">>>>>");
    while (dir.next()) {
      Serial.println(dir.fileName());
      Result += dir.fileName().substring(1) + "\n";
    }
#endif
    return Result;
  }

  // **************************************************
  // **************************************************
  void HTML_File_CheckList(String Path = "/", String Ends = "") {
    String Filename;
    String FileExt;
    String Line;
    String Result;

    fs::File dir = SD_MMC.open(Path);
    fs::File file = dir.openNextFile();
    if (file) {
      My_Webserver.sendContent("<h4>Data Files (unordered) on SD_MMC Card</h4>\n");
      My_Webserver.sendContent("<form action=\"/Files_Selected.php\" method=\"get\">\n");
      My_Webserver.sendContent("<table>\n");
      My_Webserver.sendContent(
          "<thead><tr><td>Graph</td><td>Filename</td><td>Size</td><td>Download</td></tr></thead><tbody>\n");

      while (file) {
        String Filename = String(file.name()).substring(1);
        if (Ends.length() == 0 || Filename.endsWith(Ends)) {
          Result = "";
          int x1 = Filename.indexOf('.');
          FileExt = Filename.substring(x1 + 1);
          FileExt.toLowerCase();
          Result = "<tr><td>";
          if (FileExt == "csv") {
            Result += F("<label><input type=\"checkbox\" name=\"");
            Result += Filename;
            Result += "\"></label>";
          }
          Result += "</td><td>" + Filename + "</td><td>" + String(file.size());

          Result += "</td><td><a href=\"";
          Result += Filename;
          Result += F("\" download>Download</a><td></tr>\n");

          My_Webserver.sendContent(Result.c_str());
          delay(40);
          yield();
        }
        file = dir.openNextFile();
      }

      My_Webserver.sendContent("</tbody></table><input type=\"submit\" value=\"Show Graph(s)\">\n</form>\n");
    }
  }

  // **************************************************
  // **************************************************
  void DirList_Print(String Path = "/") {
    Serial.println("──────  Files in SD_MMC  ──────  " + Path);
    int Count = 0;

// **************************************************
// ESP32
// **************************************************
#ifdef ESP32
    fs::File dir = SD_MMC.open("/");
    fs::File file = dir.openNextFile();
    while (file) {
      Serial.print("    ");
      Serial.print(file.name());
      Serial.print("\t[");
      Serial.print(file.size());
      Serial.println("]");
      file = dir.openNextFile();
      Count += 1;
    }

    Serial.printf("Files=%i  Total=%i  Used=%i  Free=%i  MaxOpen=%i\n", Count, SD_MMC.totalBytes(), SD_MMC.usedBytes(),
                  SD_MMC.totalBytes() - SD_MMC.usedBytes(), 10);
// **************************************************
// ESP8266
// **************************************************
#else
    Dir dir = Drive.openDir(Path);
    while (dir.next()) {
      Serial.print("    " + dir.fileName() + "\t[");
      fs::File file = dir.openFile("r");
      Serial.print(file.size());
      Serial.println("]");
      Count += 1;
    }

    FSInfo fs_info;
    SD_MMC.info(fs_info);
    Serial.printf("Files=%i  Total=%i  Used=%i  Free=%i  MaxOpen=%i  MaxPathLength=%i  BlockSize=%i  PageSize=%i\n",
                  Count, fs_info.totalBytes, fs_info.usedBytes, fs_info.totalBytes - fs_info.usedBytes,
                  fs_info.maxOpenFiles, fs_info.maxPathLength, fs_info.blockSize, fs_info.pageSize);
#endif
    Print_Heap();
  }

  // **************************************************
  // **************************************************
  String Get_Next_Filename() {
    if (_Last_Filename.length() == 0) {
      _Get_Last_Filename();
    }

    _Last_File_Nr += 1;
    if (_Last_File_Pre.length() == 0) {
      _Last_File_Pre = "/MLX_";
      _Last_File_Post = ".bin";
    }
    _Last_Filename = _Last_File_Pre + String(_Last_File_Nr) + _Last_File_Post;

    return _Last_Filename;
  }

  // **************************************************
  // **************************************************
  String _Get_Last_Filename() {
    if (_Last_Filename.length() > 0) {
      return _Last_Filename;
    }

// **************************************************
// ESP32
// **************************************************
#ifdef ESP32
    fs::File dir = SD_MMC.open("/");
    fs::File file = dir.openNextFile();
    while (file) {
      _Last_Filename = file.name();
      file = dir.openNextFile();
    }
// **************************************************
// ESP8266
// **************************************************
#else
    Dir dir = SD_MMC.openDir("/");
    while (dir.next()) {
      _Last_Filename = dir.fileName();
    }
#endif

    // **************************************************
    // Parse the filename and store the parts
    // **************************************************
    int x1 = _Last_Filename.indexOf('_');
    int x2 = _Last_Filename.indexOf('.');
    if (x1 > 0) {
      _Last_File_Pre = _Last_Filename.substring(0, x1 + 1);
      _Last_File_Nr = (_Last_Filename.substring(x1 + 1, x2)).toInt();
      _Last_File_Post = _Last_Filename.substring(x2);
    } else {
      _Last_File_Pre = _Last_Filename.substring(0, x2) + "_";
      _Last_File_Nr = 0;
      _Last_File_Post = _Last_Filename.substring(x2);
    }
    return _Last_Filename;
  }

  // **************************************************
  // **************************************************
  void Dump(String Filename) {
    Serial.println("────── Dump " + Filename);
    fs::File file = SD_MMC.open(Filename, "r");
    if (file) {
      while (file.available()) {
        Serial.print(file.readString());
      }
      file.close();
      Serial.println("\n──────");
    } else {
      Serial.println("ERROR: file not found");
    }
  }

  // **************************************************
  // **************************************************
  int Get_Time_In_File(String Filename) {
    int Delta_T = 0;
    fs::File file = SD_MMC.open(Filename, "r");
    String Line_0;
    String Line_1;
    String Line_2;
    String Line_x;
    if (!file) {
      return 0;
    }
    if (!file.available()) {
      return 0;
    }

    Line_0 = file.readStringUntil('\n');
    My_StringSplitter *Splitter = new My_StringSplitter(Line_0, '\t');
    int ItemCount = Splitter->getItemCount();
    int DT_i;
    for (DT_i = 0; DT_i < ItemCount; DT_i++) {
      String Item = Splitter->getItemAtIndex(DT_i);
      if (Item == "Seconds") {
        break;
      }
    }
    if (DT_i >= ItemCount) {
      return 0;
    }
    if (!file.available()) {
      return 0;
    }

    Line_1 = file.readStringUntil('\n');
    Splitter->newString(Line_1, '\t');
    String Date1 = Splitter->getItemAtIndex(DT_i);
    int DT1 = Date1.toInt();
    if (!file.available()) {
      return 0;
    }

    Line_2 = file.readStringUntil('\n');
    Splitter->newString(Line_2, '\t');
    String Date2 = Splitter->getItemAtIndex(DT_i);
    int DT2 = Date2.toInt();
    if (!file.available()) {
      return 0;
    }

    Line_x = "###";
    while (file.available() && (Line_x.length() > 1)) {
      Line_x = file.readStringUntil('\n');
      Splitter->newString(Line_x, '\t');
      String Datex = Splitter->getItemAtIndex(DT_i);
      int DTx = Datex.toInt();

      if (DTx > DT2) {
        Delta_T += DTx - DT2;
      }
      DT2 = DTx;
    }
    file.close();
    return Delta_T;
  }

  // **************************************************
  // **************************************************
  bool Exists(String Filename) {
    fs::File file = SD_MMC.open(Filename, "r");
    if (file) {
      file.close();
      return true;
    }
    return false;
  }

  // **************************************************
  // **************************************************
  void DumpAll() {
// **************************************************
// ESP32
// **************************************************
#ifdef ESP32
    fs::File dir = SD_MMC.open("");
    fs::File file = dir.openNextFile();
    while (file) {
      String Filename = file.name();
      Serial.println(Filename);
      Serial.println(file.size());
      if (file) {
        while (file.available()) {
          Serial.print(file.readString());
        }
        file.close();
      }
      file = dir.openNextFile();
    }

// **************************************************
// ESP8266
// **************************************************
#else
    // String Result ;
    Dir dir = SD_MMC.openDir("");
    while (dir.next()) {
      // Result += dir.fileName() + "\n" ;
      fs::File file = dir.openFile("r");
      if (file) {
        Serial.print("────── Contents of ");
        Serial.print(dir.fileName());
        Serial.print("    ");
        Serial.println(file.size());
        while (file.available()) {
          Serial.print(file.readString());
        }
        file.close();
        Serial.println("\n──────");
      }
    }
#endif
    return;
  }

  // **************************************************
  // **************************************************
  bool Delete(String Filename) {
    Serial.println("────── Delete " + Filename);
    bool Result = SD_MMC.remove(Filename);
    if (Result) {
      Serial.println("\n──────");
    } else {
      Serial.println("ERROR: file not found");
    }
  }

  // **************************************************
  // **************************************************
  void Delete_All() {
    Serial.println("────── Delete All Files");
    String Filename;

#ifdef ESP32
    Serial.println("!!!!!!!! ToDo _FS_class.Delete_All");
#else
    Dir dir = SD_MMC.openDir("");
    while (dir.next()) {
      Filename = dir.fileName();
      Serial.println("Delete : " + Filename);
      SD_MMC.remove(Filename);
    }
#endif
  }

  // **************************************************
  // **************************************************
  bool Store_File(String Filename, String Line) {
    fs::File file = SD_MMC.open(Filename, "w");
    file.print(Line);
    file.close();
    return true;
  }

  // **************************************************
  // **************************************************
  bool Append_File(String Filename, String Line) {
    fs::File file = SD_MMC.open(Filename, "a");
    file.print(Line);
    file.close();
    return true;
  }

  // **************************************************
  // **************************************************
  bool Create_CSV_File(String Filename, String Header) {
    if (not SD_MMC.exists(Filename)) {
      Append_File(Filename, Header);
    }
  }

  // **************************************************
  // **************************************************
  bool Create_CSV_File_Nr(String Filename, String Header) {
    // ************************************************************
    // If recorded about a day, skip to a new file
    // ************************************************************
    bool NewFile = false;
    if (_Max_FileSize > 0) {
      fs::File file = SD_MMC.open(Filename, "r");
      Serial.print("fileszie/Max");
      Serial.print(file.size());
      Serial.println(_Max_FileSize);
      if (file.size() > _Max_FileSize) {
        NewFile = true;
      }
      file.close();
    } else if (_Max_FileTime > 0) {
      // ************************************************************
      // After a reset check how many seconds are already in the file
      // ************************************************************
      if (!_Initialized) {
        _Offset_Millis = 1000 * Get_Time_In_File(Filename);
        if (_Offset_Millis > _Max_FileTime) {
          _Offset_Millis = _Max_FileTime;
        }
        _Initialized = true;
      }
      if ((millis() - _File_Millis) > (_Max_FileTime - _Offset_Millis)) {
        NewFile = true;
      }
    }

    if (NewFile) {
      _File_Millis = millis();
      if (_Last_File_Nr > _Max_NFile) {
        String File_To_Remove = _Last_File_Pre + String(_Last_File_Nr - _Max_NFile) + _Last_File_Post;
        SD_MMC.remove(File_To_Remove);
        Serial.println("rrrrrrrrrrrrrrrrrr " + File_To_Remove);
      }
      SD_MMC.rename(Filename, Get_Next_Filename());
      Serial.println("???????????????????" + _Get_Last_Filename());
    }

    if (not SD_MMC.exists(Filename)) {
      Append_File(Filename, Header);
      _Offset_Millis = 0;
    }
  }

  // *************************************************
private:
  // *************************************************
  String _Last_Filename = "";
  String _Last_File_Pre = "";
  String _Last_File_Post = "";
  int _Last_File_Nr = 0;

  String _Temp_Last_File_Pre = "";
  String _Temp_Last_File_Post = "";
  int _Temp_Last_File_Nr = 0;

  unsigned long _File_Millis = 0;
  bool _Initialized = false;
  int _Offset_Millis = 0;

  unsigned long _Max_FileTime;
  long _Max_FileSize;
  int _Max_NFile;

  // **************************************************
  // Parse the filename and store the parts
  // **************************************************
  bool _Parse_Filename(String Filename) {
    if (Filename.length() == 0) {
      return false;
    }
    int x1 = Filename.indexOf('_');
    int x2 = Filename.indexOf('.');
    if (x1 > 0) {
      _Temp_Last_File_Pre = Filename.substring(0, x1 + 1);
      _Temp_Last_File_Nr = (Filename.substring(x1 + 1, x2)).toInt();
      _Temp_Last_File_Post = Filename.substring(x2);
    } else {
      _Temp_Last_File_Pre = Filename.substring(0, x2) + "_";
      _Temp_Last_File_Nr = 0;
      _Temp_Last_File_Post = Filename.substring(x2);
    }
    return true;
  }
};

// ***********************************************************************************
// ***********************************************************************************
_SD_MMC_class SD_MMC_System;

#endif
