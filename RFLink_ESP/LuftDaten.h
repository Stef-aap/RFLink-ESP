
// Version 0.2 

#ifndef LuftDaten_h
#define LuftDaten_h   0.2

#define SOFTWARE_VERSION "NRZ-2017-100"

#define SDS_API_PIN  1

String LuftDaten_First_Part = "{\"software_version\": \"" + String(SOFTWARE_VERSION) + "\", \"sensordatavalues\":[";
String JSON_LuftData ;

const char TXT_CONTENT_TYPE_JSON[] PROGMEM = "application/json";

const char*   host_dusti     = "api.luftdaten.info";
const char*   url_dusti      = "/v1/push-sensor-data/";
int           httpPort_dusti = 443;

const char* host_madavi     = "api-rrd.madavi.de";
const char* url_madavi      = "/data.php";
const int   httpPort_madavi = 443;


// ***********************************************
// espid is needed as login name for the web-api's
// ***********************************************
#ifdef ESP32
  String  esp_chipid = String ( GetChipID() ) ;
#else
  String  esp_chipid = String ( ESP.getChipId() ) ;
#endif

// *****************************************************************
//LUFTDATTTT "15218064" ) ) ;  
// *****************************************************************
void Luftdaten_Setup ( String ChipID_2_Use ) {
  Serial.println ( "V" + String ( LuftDaten_h ) + "   LuftDSaten_h" ) ;
  #ifdef ESP32
     Serial.println ( "Chip-ID      = " + String ( GetChipID() ) ) ; 
  #else
     Serial.println ( "Chip-ID      = " + String ( ESP.getChipId() ) ) ; 
  #endif
  if ( ChipID_2_Use != "" ) {
    esp_chipid = ChipID_2_Use ;
    Serial.println ( "Chip-ID used = " + esp_chipid ) ; 
  }
}

/*****************************************************************
/* convert value to json string                                  *
/*****************************************************************/
String Value2Json(const String& type, const String& value) {
  String s = F("{\"value_type\":\"{t}\",\"value\":\"{v}\"},");
  s.replace("{t}", type); s.replace("{v}", value);
  return s;
}

// ***********************************************************************
// ugly, but it works
// ***********************************************************************
void Complete_JSON_String ( signed long Loop_Sample_Count ) {
  String data_sample_times ;
  
  int    signal_strength = WiFi.RSSI();
  data_sample_times  = Value2Json("samples", String(  long ( Loop_Sample_Count )));
  data_sample_times += Value2Json ( "signal", String(signal_strength) );

  JSON_LuftData += data_sample_times ;

  // **********************
  // remove the final comma
  // **********************
  //if ( JSON_Data [ JSON_Data.length() - 1 ] == "," ) {   //ERROR: ISO C++ forbids comparison between pointer and integer [-fpermissive]
  if ( JSON_LuftData.lastIndexOf (',') == ( JSON_LuftData.length() - 1 ) ) {
    JSON_LuftData.remove(JSON_LuftData.length() - 1);
  }

  //JSON_LuftData += ",{\"value_type\":\"SSID\",\"value\":\"" ;    
  //JSON_LuftData += WiFi.SSID() ;
  //JSON_LuftData += "\"}" ;
  //
  //JSON_LuftData += "]}";

  JSON_LuftData += "],\"SSID\":\"" ;    
  JSON_LuftData += WiFi.SSID() ;
  JSON_LuftData += "\"}" ;

  return ;
}


/*****************************************************************
/* send data to rest api                                         *
/*****************************************************************/
void sendData(const String& data, const int pin, const char* host, const int httpPort, const char* url, const char* basic_auth_string, const String& contentType) {
Debug ( String(host) + data ) ;
//return ;

  #if defined(ESP8266)
  
    //debug_out(F("Start connecting to "), DEBUG_MIN_INFO, 0);
    //debug_out(host, DEBUG_MIN_INFO, 1);
  
    String request_head = F("POST "); request_head += String(url); request_head += F(" HTTP/1.1\r\n");
    request_head += F("Host: "); request_head += String(host) + "\r\n";
    request_head += F("Content-Type: "); request_head += contentType + "\r\n";
    if (basic_auth_string != "") { request_head += F("Authorization: Basic "); request_head += String(basic_auth_string) + "\r\n";}
    request_head += F("X-PIN: "); request_head += String(pin) + "\r\n";
    request_head += F("X-Sensor: esp8266-"); request_head += esp_chipid + "\r\n";
    request_head += F("Content-Length: "); request_head += String(data.length(), DEC) + "\r\n";
    request_head += F("Connection: close\r\n\r\n");
  
    // Use WiFiClient class to create TCP connections
  
    if (httpPort == 443) {
  
      WiFiClientSecure client_s;
  
      client_s.setNoDelay(true);
      client_s.setTimeout(20000);
  
      if (!client_s.connect(host, httpPort)) {
//        debug_out(F("connection 1 failed"), DEBUG_ERROR, 1);
Debug ( "connection 1 failed" );
        return;
      }
  
      //debug_out(F("Requesting URL: "), DEBUG_MIN_INFO, 0);
      //debug_out(url, DEBUG_MIN_INFO, 1);
      //debug_out(esp_chipid, DEBUG_MIN_INFO, 1);
      //debug_out(data, DEBUG_MIN_INFO, 1);
  
      // send request to the server
  
      client_s.print(request_head);
  
      client_s.println(data);
  
      delay(10);
  
      // Read reply from server and print them
      String Line = "" ;
      while(client_s.available()) {
        char c = client_s.read();
        Line += String ( c ) ;
//        debug_out(String(c), DEBUG_MAX_INFO, 0);
Debug ( Line ) ;
      }
  
//      debug_out(F("\nclosing connection\n------\n\n"), DEBUG_MIN_INFO, 1);
  
    } else {
  
      WiFiClient client;
  
      client.setNoDelay(true);
      client.setTimeout(20000);
  
      if (!client.connect(host, httpPort)) {
//        debug_out(F("connection 2 failed"), DEBUG_ERROR, 1);
Debug ( "connection 2 failed" ) ;
        return;
      }
  
//      debug_out(F("Requesting URL: "), DEBUG_MIN_INFO, 0);
//      debug_out(url, DEBUG_MIN_INFO, 1);
//      debug_out(esp_chipid, DEBUG_MIN_INFO, 1);
//      debug_out(data, DEBUG_MIN_INFO, 1);
  
      client.print(request_head);
  
      client.println(data);
  
      delay(10);
  
      // Read reply from server and print them
      while(client.available()) {
        char c = client.read();
//        debug_out(String(c), DEBUG_MAX_INFO, 0);
      }
  
//      debug_out(F("\nclosing connection\n------\n\n"), DEBUG_MIN_INFO, 1);
  
    }
  
//    debug_out(F("End connecting to "), DEBUG_MIN_INFO, 0);
//    debug_out(host, DEBUG_MIN_INFO, 1);
  
    wdt_reset(); // nodemcu is alive
    yield();
  #endif
}

// ***********************************************************************
// ***********************************************************************
bool _Send_LuftData () {

  if ( JSON_LuftData.indexOf ( "SDS_P1" ) < 0 ) {
    Debug ( "NO DATA for Luftfaten available" ) ;
    return false ;
  }

  String Temp = JSON_LuftData ;
  int x1 = Temp.indexOf ( '}' ) ;
  int x2 = Temp.indexOf ( '}', x1+1 ) ;
  Temp.remove ( x2+1 );
  Temp += "]}" ;
  Temp.replace ( "SDS_", "" );
  
  sendData ( Temp, SDS_API_PIN, host_dusti, httpPort_dusti, url_dusti, "", FPSTR(TXT_CONTENT_TYPE_JSON)) ;
  return true ;
}


// ***********************************************************************
// ***********************************************************************
bool _Send_MadaviData () {

  if ( JSON_LuftData.indexOf ( "SDS_P1" ) < 0 ) {
    Debug ( "NO DATA for Madavi available" ) ;
    return false ;
  }

  sendData ( JSON_LuftData, 0, host_madavi, httpPort_madavi, url_madavi, "", FPSTR(TXT_CONTENT_TYPE_JSON)) ;
  return true ;
      
}



#endif
