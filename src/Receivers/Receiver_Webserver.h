// ***********************************************************************************
// * ESP8266 SPIFFS HTML Web Page with JPEG, PNG Image
// https://circuits4you.com/2018/01/31/upload-image-png-jpeg-to-esp8266-web-page/
//
// de volgende werken:
// http://192.168.4.1/index.html
// http://192.168.4.1
// http://192.168.4.1/7410.jpg
// http://192.168.4.1/index.h   (maar geeft pagina weer als textfile)
//
// goed overzicht :  https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/
//   https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
//
// op basis van example ESp8266Webserver/AdvancedWebServer.ino
// deze webserver geeft problemen bij grotere bestanden, je moet dan echt een asynchrone webserver gebruiken
// ***********************************************************************************
// Version 0.3   11-02-2020, SM, checked by ....
//    - opschoning
//
// Version 0.2   saved
// Version 0.1   24-07-2019, SM, checked by ....
//    - intial version
// ***********************************************************************************

#ifndef Receiver_Webserver_h
#define Receiver_Webserver_h 0.3

String WebServer_Root_Page = "/index.html";

#include "Clients/FS_support.h"
#ifdef ESP32
  #include "Hardware/SD_MMC_support.h"
#endif
#include "Web/html_templates.h"

// ***********************************************************************
// Stream de opgegeven file naar de webclient
// ***********************************************************************
bool Load_File(fs::FS &Drive, String path) {
  String dataType = "text/plain";
  Serial.println("load from Path = " + path);
  if (path.endsWith("/")) path += WebServer_Root_Page.substring(1);
  // else if ( path.endsWith ( "/index.html" ) ) path.replace ( "/index.html", WebServer_Root_Page ) ;

  // EXISTS TOEVOEGEN

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".h"))
    dataType = "text/html";
  else if (path.endsWith(".html"))
    dataType = "text/html";
  else if (path.endsWith(".htm"))
    dataType = "text/html";
  else if (path.endsWith(".css"))
    dataType = "text/css";
  else if (path.endsWith(".js"))
    dataType = "application/javascript";
  else if (path.endsWith(".png"))
    dataType = "image/png";
  else if (path.endsWith(".gif"))
    dataType = "image/gif";
  else if (path.endsWith(".jpg"))
    dataType = "image/jpeg";
  else if (path.endsWith(".ico"))
    dataType = "image/x-icon";
  else if (path.endsWith(".xml"))
    dataType = "text/xml";
  else if (path.endsWith(".pdf"))
    dataType = "application/pdf";
  else if (path.endsWith(".zip"))
    dataType = "application/zip";

  fs::File dataFile = Drive.open(path.c_str(), "r");
  if (My_Webserver.hasArg("download")) dataType = "application/octet-stream";
  if (My_Webserver.streamFile(dataFile, dataType) != dataFile.size()) {
    // error ???
  }
  dataFile.close();

  return true;
}

// ***********************************************************************
// ***********************************************************************
bool loadFrom_SD_MMC(String Path) {
#ifdef ESP32
  return Load_File(SD_MMC, Path);
#endif
  return false;
}

// ***********************************************************************
// ***********************************************************************
bool loadFromSpiffs(String Path) { return Load_File(SPIFFS, Path); }

// ***********************************************************************
// change Root "/"  naar  "/index.html"  (or whatever de startpagina is)
// ***********************************************************************
void handleRoot() {
  My_Webserver.sendHeader("Location", WebServer_Root_Page, true); // Redirect to our html web page
  My_Webserver.send(302, "text/plane", "");
}

// ***********************************************************************
// Direct verzenden werkt beter dan eerst in een file opslaan
//
// check/Uncheck ALL signal CBs:
// https://www.includehelp.com/code-snippets/javascript-select-unselect-check-unckecck-all-checkboxes.aspx
// ***********************************************************************
void Handle_Files_Selected() {
  My_Webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  My_Webserver.send_P(200, "text/html", HTML_Graph_Page_Begin);

  String Temp;
  String CBXs;
  for (uint8_t i = 0; i < My_Webserver.args(); i++) {
    // yield();
    int pos = My_Webserver.argName(i).indexOf('&');
    String Filename = My_Webserver.argName(i).substring(0, pos);
    Serial.println("======++++" + My_Webserver.argName(i) + "pp " + Filename);
    CBXs = "";

    if (i == 0) {
      fs::File file = SPIFFS.open("/" + Filename, "r");
      if (file) {
        String FirstLine = file.readStringUntil('\n');
        file.close();
        My_StringSplitter *Splitter = new My_StringSplitter(FirstLine, '\t');
        int ItemCount = Splitter->getItemCount();
        if (ItemCount <= 1) {
          Splitter->newString(FirstLine, ',');
          ItemCount = Splitter->getItemCount();
        }
        int DT_i;
        for (DT_i = 1; DT_i < ItemCount; DT_i++) {
          // yield();
          String Item = Splitter->getItemAtIndex(DT_i);
          Temp = String(FPSTR(HTML_Dygraph_Signal_Select));
          Temp.replace("XXX_Label", Item);
          Temp.replace("XXX", String(DT_i - 1));
          CBXs += Temp;
        }
      }
    }
    Temp = String(FPSTR(HTML_Dygraph));
    Temp.replace("XXX", String(i));
    Temp.replace("FFF", Filename);
    Temp.replace("CBXs", CBXs);
    My_Webserver.sendContent(Temp);
    Serial.println("Dygraph of : " + Filename);
  }
  My_Webserver.sendContent("</body></html>");
}

// ***********************************************************************
// ***********************************************************************
void Handle_Files_Selected_SD() {
  String Temp;
  String CBXs;
  String Line = FPSTR(HTML_Graph_Page_Begin);
  for (uint8_t i = 0; i < My_Webserver.args(); i++) {
    int pos = My_Webserver.argName(i).indexOf('&');
    String Filename = My_Webserver.argName(i).substring(0, pos);
    Serial.println("======++++" + My_Webserver.argName(i) + "pp " + Filename);
    CBXs = "";
#ifdef ESP32
    if (i == 0) {
      fs::File file = SD_MMC.open("/" + Filename, "r");
      if (file) {
        String FirstLine = file.readStringUntil('\n');
        file.close();
        My_StringSplitter *Splitter = new My_StringSplitter(FirstLine, '\t');
        int ItemCount = Splitter->getItemCount();
        if (ItemCount <= 1) {
          Splitter->newString(FirstLine, ',');
          ItemCount = Splitter->getItemCount();
        }
        int DT_i;
        for (DT_i = 1; DT_i < ItemCount; DT_i++) {
          String Item = Splitter->getItemAtIndex(DT_i);
          Temp = String(FPSTR(HTML_Dygraph_Signal_Select));
          Temp.replace("XXX_Label", Item);
          Temp.replace("XXX", String(DT_i - 1));
          CBXs += Temp;
          // Serial.println ( "UUUUUUUUUUUUUUUU : " + String(DT_i) + Temp ) ;
        }
      }
    }
#endif
    Temp = String(FPSTR(HTML_Dygraph));
    Temp.replace("XXX", String(i));
    Temp.replace("FFF", Filename);
    Temp.replace("CBXs", CBXs);
    Line += Temp;
  }
  Line += "</body></html>";
  // Serial.println ( Line ) ;
#ifdef ESP32
  SD_MMC_System.Store_File("/Select_Signals.html", Line);
#endif
  My_Webserver.send(404, "text/html", Line);
}

// ***********************************************************************
// ***********************************************************************
void Handle_Modifed_Settings() {
  String message;
  message += (My_Webserver.method() == HTTP_GET) ? "GET" : "POST";
  message += "\r\nArguments: ";
  message += My_Webserver.args();
  message += "\r\n";
  for (uint8_t i = 0; i < My_Webserver.args(); i++) {
    message += " NAME:" + My_Webserver.argName(i) + "\r\n VALUE:" + My_Webserver.arg(i) + "\r\n";
  }

  // My_Webserver.send ( 404, "text/html", "<html><H2>Gewijzigde Instellingen zijn opgeslagen.</H2></html>" ) ;
  My_Webserver.send(404, "text/html", FPSTR(HTML_Settings_Done));
  // loadFromSpiffs ( "/Settings.html" ) ;
  Serial.println(message);

  // **************************************************************
  // Het antwoord op de webpagina bevat allee items die aangechecked zijn
  // vandaar dat we op een lastige manier doorheen moeten lopen
  // **************************************************************
  bool Restart_Needed = false;

  JsonObject DocumentRoot = _My_Settings_Buffer.as<JsonObject>();
  for (JsonPair KeyValue : DocumentRoot) {
    String Key = String(KeyValue.key().c_str());
    if (Key.startsWith("Sensor_") || Key.startsWith("Receiver_")) {
      //_My_Settings_Buffer [ Key ] = false ;
      JsonVariant Value = KeyValue.value();
      // Serial.println ( "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ-" + Key + "-ZZ-" + My_Webserver.arg ( Key ) +
      // "-Z-" +Value.as<String>() + "-" ) ;
      if ((My_Webserver.arg(Key) == "on") && (!Value.as<bool>())) {
        // Serial.println ( "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR " + Key + "  //  " + My_Webserver.arg ( Key ) )
        // ;
        _My_Settings_Buffer[Key] = true;
        Restart_Needed = true;
      } else if ((My_Webserver.arg(Key) == "") && (Value.as<bool>())) {
        // Serial.println ( "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR " + Key + "  //  " + My_Webserver.arg ( Key ) )
        // ;
        _My_Settings_Buffer[Key] = false;
        Restart_Needed = true;
      }
    }
  }

  // **************************************************************
  Store_Modified_Settings_From_Webserver(Restart_Needed);
}

// ***********************************************************************
// ***********************************************************************
void Handle_Build_Settings() {
  String Build_Info = "00000000";
  for (uint8_t i = 0; i < My_Webserver.args(); i++) {
    int Item = (My_Webserver.argName(i).substring(5)).toInt();
    Build_Info[Item] = '1';
  }
  Serial.println("BUILD INFO :" + Build_Info);
  My_Webserver.send(404, "text/html", FPSTR(HTML_Settings_Done));
  File_System.Store_File(BI, Build_Info);
  delay(3000);
  ESP.restart();
}

// ***********************************************************************
// ***********************************************************************
void Send_index_html() {
  String Line;
  My_Webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  My_Webserver.send_P(200, "text/html", HTML_Sensor_StartPagina_Begin);

  if (Settings_By_WebInterface) My_Webserver.sendContent_P(HTML_Sensor_StartPagina_Begin_Settings);
  if (Build_Info.length() > 0) My_Webserver.sendContent_P(HTML_Sensor_StartPagina_Begin_BuildInfo);
  My_Webserver.sendContent_P(HTML_Sensor_StartPagina_Begin_Values);

  File_System.HTML_File_CheckList("/"); //, ".csv" ) ;

#ifdef Receiver_SDfat_h
  ((_Receiver_SDfat *)_p_Receiver_SDfat)->HTML_File_CheckList("/", ".csv", true);
#endif

#ifdef ESP32
  SD_MMC_System.HTML_File_CheckList("/", ".csv");
#endif

  My_Webserver.sendContent_P(HTML_Sensor_StartPagina_End);
}

// ***********************************************************************
// ***********************************************************************
void Show_Build_Info() {
  String Build_Info = File_System.Read_File(BI);
  if (Build_Info.length() == 0) {
    Build_Info = "00000000";
    File_System.Store_File(BI, Build_Info);
  }

  My_Webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  My_Webserver.send_P(200, "text/html", HTML_Build_Info_Pagina_Begin);
  if (Build_Info_Help.length() > 0) My_Webserver.sendContent(Build_Info_Help);

  String Result;
  for (int i = 0; i < 8; i++) {
    String Line = "Item_" + String(i);
    Result = F("<label><input type=\"checkbox\" name=\"");
    Result += Line;
    Result += "\" ";
    if (Build_Info[i] == '1') Result += "checked";
    Result += "> ";
    Result += Line;
    Result += "</label><br>\r\n";
    My_Webserver.sendContent(Result.c_str());
    delay(40);
    yield();
  }
  My_Webserver.sendContent("<br><input type=\"submit\" value=\"Save Build Info\">\r\n\
</form>\r\n");
}

// ***********************************************************************
// ***********************************************************************
void Handle_Values() {
  fs::File dataFile = SPIFFS.open("/Values.html", "r");
  My_Webserver.streamFile(dataFile, "text/html");
  dataFile.close();
}

// ***********************************************************************
// this callback will be invoked when user request "/Xsensors" */
// het zou kunnen dat een request midden in de opbouw van JSON_Data komt
//   maar dat vinden we helemaal niet erg.
// LET OP: ER VERSCHIJNT NIET ALS ER EEN FOUT IN HET JSON BERICHT ZIT
// ***********************************************************************
void Update_Values() {
  // Serial.print ( ":::::::::::::::::");
  // Serial.println ( JSON_Data ) ;
  My_Webserver.send(200, "application/json", JSON_Data);
}

// ***********************************************************************
// ***********************************************************************
void handleWebRequests() {
  Serial.println("Web Request : " + My_Webserver.uri());
  Serial.println(String(SPIFFS.exists(My_Webserver.uri())));

  String Page = My_Webserver.uri();

  if (Page == "/index.html") {
    Send_index_html();
    return;
  }

  String Page_h = Page;
  Page_h.replace(".html", ".h");
  if (!SPIFFS.exists(Page) && Page.endsWith(".html") && SPIFFS.exists(Page_h)) {
    Page = Page_h;
  }

  // *******************************************************
  // *******************************************************
  // if ( Page == "/Values_Page.php" ) {
  //  Handle_Values () ;
  //  return ;
  //}
  if (Page == "/Settings_Page.php") {
    Handle_Modifed_Settings();
    return;
  }
  if (Page == "/Build_Page.php") {
    Handle_Build_Settings();
    return;
  }
  if (Page == "/Files_Selected.php") {
    Handle_Files_Selected();
    return;
  }
  if (Page == "/Files_Selected_SD.php") {
    Handle_Files_Selected_SD();
    return;
  }

  if (Page == "/Stop_Recording.html") {
    My_Webserver.send_P(404, "text/html", HTML_Stop_Recording);
    Recording = false;
    return;
  }

  if (Page == "/Factory_Settings.html") {
    My_Webserver.send_P(404, "text/html", "Device will be restarted.");
    Settings_Factory();
    return;
  }

  if (Page == "/Format_SPIFFS.html") {
    Serial.println("Format SPIFFS ");
    return;
  }
// *******************************************************

// *******************************************************
// loadFromSpiffs(server.uri()) GEEFT ALTIJD TRUE TERUG !!
// *******************************************************
#ifdef ESP32
  // SD_MMC files always start with $, which must be removed
  if (Page.startsWith("/$")) {
    Page = "/" + Page.substring(2);
    loadFrom_SD_MMC(Page);
    return;
  }
  if (SD_MMC.exists(Page) && loadFrom_SD_MMC(Page)) return;

#endif

  if (SPIFFS.exists(Page) && loadFromSpiffs(Page)) return;

  // *******************************************************

  Serial.println("    File not found");
  String message = "File Not Detected\r\n";
  message += "URI: ";
  message += Page;
  message += "\r\nMethod: ";
  message += (My_Webserver.method() == HTTP_GET) ? "GET" : "POST";
  message += "\r\nArguments: ";
  message += My_Webserver.args();
  message += "\r\n";
  for (uint8_t i = 0; i < My_Webserver.args(); i++) {
    message += " NAME:" + My_Webserver.argName(i) + "\r\n VALUE:" + My_Webserver.arg(i) + "\r\n";
  }
  My_Webserver.send(404, "text/plain", message);
  Serial.println(message);
}

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_Webserver : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_Webserver() {
    Version_Name = "V" + String(Receiver_Webserver_h) + "   Receiver_Webserver.h";
    Serial.println("CREATE    " + Version_Name);
    Help_Text = "    PAS OP, deze Receiver werkt aanzienlijk slechter als Receiver_OTA wordt gebruikt !!!!\r\n\
Starts a WebServer on IP address assigned by the home network\r\n\
All pages should be stored in SPIFFS, with their names starting with a forward slash \"/\".\r\n\
Files with the extension .h (which can be edited in Arduino) will be sent as type \"text/html\".\r\n\
If \"index.html\" doesn't exists, but \"index.h\" does the latter will be used instead.\r\n\
   more to come";
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void setup() {
    if (!File_System.Exists("/index.html") && File_System.Exists("/index.h")) {
      WebServer_Root_Page = "/index.h";
    }
    My_Webserver.on("/", handleRoot);
    // My_Webserver.on         ( "/Values_Page.php", Handle_Values ) ;
    My_Webserver.on("/Values.html", Handle_Values);
    My_Webserver.on("/Build_Info.html", Show_Build_Info);
    My_Webserver.on("/Xsensors", Update_Values);
    My_Webserver.onNotFound(handleWebRequests); // Set setver all paths are not found so we can handle as per URI
    My_Webserver.begin();
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {}

  // ***********************************************************************
  // ***********************************************************************
  void loop() { My_Webserver.handleClient(); }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) { return true; }

  // ***********************************************************************
private:
  // ***********************************************************************
};

#endif
