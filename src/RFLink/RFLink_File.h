// Version 0.2,  25-04-2020,  SM
//    - includes removed (problems distributing on Github)
//
// Version 0.1

#ifndef RFLink_File_h
#define RFLink_File_h 0.2

// ***********************************************************************************
// This class is explicitly NOT derived from _FS_Class,
// because an instance of _FS_Class will always exist already and therfor can be used.
// ***********************************************************************************
class _RFLink_File {

public:
  String Known_Devices = "";

  // **************************************************
  // **************************************************
  _RFLink_File(String Filename = "/RFLink.txt") { _Filename = Filename; }

  // **************************************************
  // **************************************************
  void Begin() {
    // for logging use max 5 files of length 100000
    File_System.Begin(this->_Log_Filename, -100000, 5);
    _Read_Device_File();
  }

  // **************************************************
  // **************************************************
  void Log_Line(String Line) {
    File_System.Create_CSV_File_Nr(this->_Log_Filename, "");
    File_System.Append_File(this->_Log_Filename, String(millis() / 1000) + "\t" + Line + "\r\n");
    Serial.println("LOG: " + Line);
  }

  // **************************************************
  // **************************************************
  bool Add_Device(String Line) {
    if (Known_Devices.indexOf(Line) < 0) {
      Known_Devices += Line;
      _Write_Device_File(Known_Devices);
      return true;
    }
    return false;
  }

  // **************************************************
  // **************************************************
  bool Remove_Device(String Line) {
    int pos = Known_Devices.indexOf(Line);
    if (pos >= 0) {
      Known_Devices = Known_Devices.substring(0, pos) + Known_Devices.substring(pos + Line.length());
      _Write_Device_File(Known_Devices);
      return true;
    }
    return false;
  }

  // **************************************************
  // **************************************************
  bool Clear_Devices() {
    bool removed = SPIFFS.remove(_Filename);
    Known_Devices = "";
    return removed;
  }

  // **************************************************
  // **************************************************
  void Print_Devices() {
    String Line = "------  Known Devices ------";
    this->Log_Line(Line);
    int x0 = 0;
    while (x0 < Known_Devices.length()) {
      int x1 = Known_Devices.indexOf(";", x0 + 1);
      int x2 = Known_Devices.indexOf(";", x1 + 1);
      if ((x2 < 0) || (x2 < 0)) return;
      Line = "    " + Known_Devices.substring(x0, x2 + 1);
      this->Log_Line(Line);
      x0 = x2 + 1;
    }
  }

  // **************************************************
  // **************************************************
private:
  String _Filename;
  String _Log_Filename = "/RFLog.txt";

  // **************************************************
  // **************************************************
  bool _Write_Device_File(String Line) {
    fs::File file = SPIFFS.open(_Filename, "w");
    file.print(Line);
    file.close();
    return true;
  }

  // **************************************************
  // **************************************************
  void _Read_Device_File() {
    fs::File file = SPIFFS.open(_Filename, "r");
    Known_Devices = "";
    if (file) {
      while (file.available()) {
        Known_Devices += file.readString();
      }
      file.close();
    }
  }
};

#endif
