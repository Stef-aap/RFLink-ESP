
// Version 0.3
//   - writing in small chunks instead of the whole buffer
//
// Version 0.2

#ifndef Receiver_SDcard_h
#define Receiver_SDcard_h 0.3

#include <SD.h>

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_SDcard : public _Receiver_BaseClass {

public:
  String Help_Text = "#define Receiver_SDcard  <Filename>\n\
#define Receiver_SDcard  <Filename>  <CS>\n\
    <CS> Chip Select pin, defaults to GPIO4\n\
    <Filename>            defaults to \"temp.txt\"";

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_SDcard() {
    _Data_Filename = "temp.txt";
    _SD_Card_CS = 14;
    Serial.println("V" + String(Receiver_SDcard_h) + "   Receiver_SDcard_h");
  }
  _Receiver_SDcard(String Filename) {
    _Data_Filename = Filename;
    _SD_Card_CS = 14;
    Serial.println("V" + String(Receiver_SDcard_h) + "   Receiver_SDcard_h");
  }
  _Receiver_SDcard(String Filename, int SD_Card_CS) {
    _Data_Filename = Filename;
    _SD_Card_CS = SD_Card_CS;
    Serial.println("V" + String(Receiver_SDcard_h) + "   Receiver_SDcard_h");
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    Serial.print("\n#define Receiver_SDcard  ");
    Serial.print(_Data_Filename + "  ");
    Serial.print(_SD_Card_CS);
    Serial.print("\n    Initializing SD card ... ");
    // see if the card is present and can be initialized:
    if (!SD.begin(_SD_Card_CS)) {
      Serial.println("Card failed, or not present");
    } else {
      _Card_Present = true;
      Serial.println("SD-Card initialized.");
    }
    Serial.println(Help_Text);
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) {
    if (!_Card_Present) {
      return false;
    }

    //******************************************************
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    // FILE_WRITE = read / append
    //******************************************************
    File DataFile = SD.open(_Data_Filename, FILE_WRITE);

    //******************************************************
    // if the file is available, write to it:
    // neemt zo'n 18 msec in beslag
    //******************************************************
    if (DataFile) {

#define BUFFER_SIZE 200
      const char *a = JSON_Message.c_str();
      int i;
      for (i = 0; (i + BUFFER_SIZE) < JSON_Message.length(); i += BUFFER_SIZE) {
        DataFile.write(a + i, BUFFER_SIZE);
      }
      DataFile.write(a + i, JSON_Message.length() - i);
      DataFile.write("\n");

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
    } else if (LowerCase.startsWith("copy ")) {
      Filename = Serial_Command.substring(5);
      File_System.Copy(Filename);
    } else if (LowerCase.startsWith("move ")) {
      Filename = Serial_Command.substring(5);
      File_System.Move(Filename);
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
private:
  // ***********************************************************************
  String _Data_Filename;
  int _SD_Card_CS;
  bool _Card_Present = false;

  String _Serial_Commands_Text = "------  Receiver_SDMMC Commands  ------\n\
dir [FILENAME]  // Display a list of all files starting with Filename\n\
dump FILENAME   // Print the contents of a file\n\
dumpALL         // Print the contents of all files\n\
create FILENAME // Create file if not already exists, append line text\n\
del    FILENAME // Delete file\n\
delALL YES      // Delete ALL files";
};

#endif
