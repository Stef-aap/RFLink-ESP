//
// Version 0.1,  08-08-2019, SM

#ifndef My_Settings_h
#define My_Settings_h 0.1

#include <ArduinoJson.h>

#ifdef FileSystem_SPIFFS
  #include "Clients/FS_support.h"
#endif

#include "Web/html_templates.h"

String _My_Settings_Filename = "/Settings.json";

DynamicJsonDocument _My_Settings_Buffer(2000); // ArduinoJson V6

// ***********************************************************************************
// ***********************************************************************************
class _My_Settings_Class {
public:
  JsonObject DocumentRoot;

  // ***********************************************************************
  // ***********************************************************************
  _My_Settings_Class() {}

  void Setup() {
    // THIS SEEMS NONSENSE HERE IS REQUIRED !!!
#ifdef FileSystem_SPIFFS
  #ifdef ESP32
    SPIFFS.begin(true); // format if no filesystem yet
  #else
    if (!SPIFFS.begin()) {
      SPIFFS.format();
    }
    SPIFFS.begin();
  #endif

    Serial.println("===== Read_Settings from file = " + _My_Settings_Filename);
    fs::File file = SPIFFS.open(_My_Settings_Filename, "r");

    if (file) {
      DeserializationError error = deserializeJson(_My_Settings_Buffer, file);
      if (error) Serial.println("Failed to read file JJJJJaaaa, using default configuration" + _My_Settings_Filename);
      file.close();
    } else {
      Serial.println("ERROR: file not found = " + _My_Settings_Filename);
    }

    JsonObject DocumentRoot = _My_Settings_Buffer.as<JsonObject>();
#endif
  }

  // ***********************************************************************
  // ***********************************************************************
  void Remove(String Key) { DocumentRoot.remove(Key); }

  // ***********************************************************************
  // ***********************************************************************
  void Store_Settings(String Filename = _My_Settings_Filename) {
#ifdef FileSystem_SPIFFS
    fs::File file = SPIFFS.open(Filename, "w");
    if (serializeJsonPretty(_My_Settings_Buffer, file) == 0) {
      Serial.println("Failed to write to file " + Filename);
    } else
      UnStored_Changes = false;
    file.close();
#endif
  }

  // ***********************************************************************
  // ***********************************************************************
  void Set_Unstored_Changes() { UnStored_Changes = true; }

  // ***********************************************************************
  // ***********************************************************************
  void Create_WebPage(String Filename = "/Settings.html") {
#ifdef FileSystem_SPIFFS
    Serial.print("\n      Creating Settings Webpage  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ IP = ");
    Serial.println(WiFi.localIP());

    fs::File file = SPIFFS.open(Filename, "w");
    if (!file) {
      Serial.println("EEERRROOORRRRRR");
      return;
    }

    String Line = "";
    Line += String(FPSTR(HTML_Sensor_Settings_Begin));
    file.print(Line);

    // *************************************************
    // because My_Settings array may have changed,
    // must be made a JSON object again
    // *************************************************
    JsonObject DocumentRoot = _My_Settings_Buffer.as<JsonObject>();

    for (JsonPair KeyValue : DocumentRoot) {
      String Key = String(KeyValue.key().c_str());
      JsonVariant Value = KeyValue.value();

      // ************************************************
      // ************************************************
      if (Value.is<bool>()) {
        bool Value = (bool)_My_Settings_Buffer[Key];

        Line = "<label><input type=\"checkbox\" name=\"";
        Line += Key;
        Line += "\" ";
        if (Value) Line += "checked";
        Line += "> ";
        Line += Key;
        Line += "</label><br>";
        file.println(Line);
      }

      // ************************************************
      // ************************************************
      else {
        String Value = _My_Settings_Buffer[Key].as<String>();
        Line = "<input type=\"text\" name=\"";
        Line += Key;
        Line += "\" value=\"";
        if (Key.startsWith("$")) Line += "********";
        else
          Line += Value;
        Line += "\"> ";
        Line += Key;
        Line += "<br>";
        file.println(Line);
      }
    }

    Line = "<br><label><input type=\"checkbox\" name=\"All_Default_Values\">All Default Values</label><br>";
    file.print(Line);
    Line = "<br><input type=\"submit\" value=\"Submit\">\n\
</form>\n\
</div>\n\
</body>\n\
</html>";
    file.print(Line);
    file.close();

#endif
  }

  // ***********************************************************************
  // ***********************************************************************
  String Read_String(String Key) {
    String Line = _My_Settings_Buffer[Key].as<String>();
    if (Line == "null") Line = "";
    return Line;
  }

  // ***********************************************************************
  // ***********************************************************************
  String Get_Set_Default_String(String Key, String Default, bool Force = false) {
    String Value = _My_Settings_Buffer[Key].as<String>();
    if (Force || (Value == "null")) {
      _My_Settings_Buffer[Key] = Default;
      UnStored_Changes = true;
      return Default;
    }
    return Value;
  }

  // ***********************************************************************
  // ***********************************************************************
  int Read_Int(String Key) { return (int)_My_Settings_Buffer[Key]; }

  // ***********************************************************************
  // ***********************************************************************
  int Get_Set_Default_Int(String Key, int Default, bool Force = false) {
    int Value = _My_Settings_Buffer[Key] | Default;
    if (Force || (Value == Default)) {
      _My_Settings_Buffer[Key] = Default;
      UnStored_Changes = true;
      return Default;
    }
    return Value;
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Read_Bool(String Key) { return (bool)_My_Settings_Buffer[Key]; }

  // ***********************************************************************************
private:
  // ***********************************************************************************
  bool UnStored_Changes = false;
};

// ***********************************************************************************
// Create an instance
// ***********************************************************************************
_My_Settings_Class Settings;

#endif