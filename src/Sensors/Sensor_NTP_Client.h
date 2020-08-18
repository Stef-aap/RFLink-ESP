// Version 0.3, 23-02-2020, SM
//   - DateTime gesplits in 2 kolommen
//
// Version 0.2, 23-06-2019, SM
//   - Datetime string in JSON had geen doulblequotes
//
// Version 0.1, 11-06-2019, SM
//   - initial version

#ifndef Sensor_NTP_Client_h
#define Sensor_NTP_Client_h 0.3

#include <ezTime.h>

// ***********************************************************************************
// uses ezTime:   https://github.com/ropg/ezTime
// ***********************************************************************************
class _Sensor_NTP_Client : public _Sensor_BaseClass {

public:
  Timezone _myTZ;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_NTP_Client() {
    Version_Name = "V" + String(Sensor_NTP_Client_h) + "  ======  Sensor_NTP_Client.h";
    Serial.println("CREATE    " + Version_Name);
    _JSON_Short_Header = "Date\tTime\t";
    Help_Text = "    >>>>>>> PAS OP,  HET LIJKT ER OP DAT DEZE SENSOR DE MQTT COMMUNICATIE AANZIENLIJK VERSLECHTERD";

    this->_JSON_Short_Header = "Date\tTime\t";
    this->_JSON_Long_Header = this->_JSON_Short_Header;
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    // setServer ( "time.nist.gov" )   ;  // WERKT NIET
    // setServer ( "pool.ntp.org" )    ;  // OK
    setServer("nl.pool.ntp.org"); // OK

    waitForSync(30); // timeout [sec]

    Serial.print("NTP server nl.pool.ntp.org, last synced : zou niet weten hoe ik dat formateren ");
    Serial.println(lastNtpUpdateTime());

    if (!_myTZ.setCache(0)) {
      _myTZ.setLocation("Europe/Amsterdam");
      Serial.println("==========  Timezone SetLocation was not yet found in Cache  ==========");
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() { events(); }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    String Date = _myTZ.dateTime("Y-m-d");
    String Time = _myTZ.dateTime("H:i:s");

    JSON_Data += " \"Date\":\"" + Date;
    JSON_Data += "\", \"Time\":\"" + Time;
    JSON_Data += "\",";

    JSON_Short_Data += Date + '\t' + Time + '\t';
  }

  // ***********************************************************************
private:
  // ***********************************************************************
};

#endif
