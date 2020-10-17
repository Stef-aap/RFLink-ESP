// Version 0.1, 03-02-2020, SM
//   - initial version
// ***********************************************************************************
// uses <root>\Arduino\libraries\Grove_-_RTC_DS1307
// according to the datasheet: https://datasheets.maximintegrated.com/en/ds/DS1307.pdf
// the ic only works with an I2C clock of 100 kHz
// experiments however show the following
// the chip works well with a clock of 200kHz ... 1MHz
// tested with ESP8266 in 80 / 160MHz mode, with and without pull-up resistors
// ***********************************************************************************

// ***********************************************************************************
// Use define Set_RTC_YMD_HMS_DoW, before importing Sensor_Receiver
// ***********************************************************************************
//#define Set_RTC_YMD_HMS_DoW  "2020,1,10,13,11,13,MON"
//#include "Sensor_Receiver.h"
// ***********************************************************************************

#ifndef Sensor_RTC_h
#define Sensor_RTC_h 0.1

#include "DS1307.h"
DS1307 Rclock; // preferably local

// Specifies that the clock can be a maximum of 100 kHz
// According to my measurements, the clock should be at least 200 kHz
// ***********************************************************************
#define _RTC_My_I2C_Clock 400000
// ***********************************************************************

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_RTC : public _Sensor_BaseClass {

public:
  // _Sensor_RTC ***********************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_RTC() {
    Version_Name = "V" + String(Sensor_RTC_h) + "  ------  Sensor_RTC.h";
    Serial.println("\n------  CREATE  ------  " + Version_Name);
    this->_JSON_Long_Header = "Date\tTime\t";
    this->_JSON_Short_Header = "Date\tTime\t";
    Help_Text = "    >>>>>>> ToDO Help text";
  }

  // _Sensor_RTC ***********************************************************
  // ***********************************************************************
  void setup() {
    Wire.setClock(400000);

#ifdef Set_RTC_YMD_HMS_DoW
    Serial.println("==================> Set RTC");
    String Line = Set_RTC_YMD_HMS_DoW;
    My_StringSplitter *Splitter = new My_StringSplitter(Line, ',');

    int Year = (Splitter->getItemAtIndex(0)).toInt();
    int Month = (Splitter->getItemAtIndex(1)).toInt();
    int Day = (Splitter->getItemAtIndex(2)).toInt();
    int Hour = (Splitter->getItemAtIndex(3)).toInt();
    int Minute = (Splitter->getItemAtIndex(4)).toInt();
    int Second = (Splitter->getItemAtIndex(5)).toInt();

    int DoW;
    Line = Splitter->getItemAtIndex(6);
    if (Line == "MON") DoW = MON;
    else if (Line == "TUE")
      DoW = TUE;
    else if (Line == "WED")
      DoW = WED;
    else if (Line == "THU")
      DoW = THU;
    else if (Line == "FRI")
      DoW = FRI;
    else if (Line == "SAT")
      DoW = SAT;
    else
      DoW = SUN;

    Rclock.begin();
    Rclock.fillByYMD(Year, Month, Day);
    Rclock.fillByHMS(Hour, Minute, Second);
    Rclock.fillDayOfWeek(DoW);
    Rclock.setTime();

    // trying to get the first real read also correct
    delay(1000);
    Rclock.getTime();
    delay(1000);
    Rclock.getTime();
    Serial.println("done");
#endif
  }

  // _Sensor_RTC ***********************************************************
  // ***********************************************************************

  String GetDateTime() {
    Wire.setClock(400000); //_RTC_My_I2C_Clock ) ;
    Rclock.getTime();

    char DateTimeBuffer[35] = "";

    String DayOfWeek;
    switch (Rclock.dayOfWeek) { // Friendly printout the weekday
      case MON:
        DayOfWeek = "Monday";
        break;
      case TUE:
        DayOfWeek = "Tuesday";
        break;
      case WED:
        DayOfWeek = "Wednesday";
        break;
      case THU:
        DayOfWeek = "Thursday";
        break;
      case FRI:
        DayOfWeek = "Friday";
        break;
      case SAT:
        DayOfWeek = "Saterday";
        break;
      case SUN:
        DayOfWeek = "Sunday";
        break;
    }

    Global_DateTime.Year = Rclock.year + 2000;
    Global_DateTime.Month = Rclock.month;
    Global_DateTime.Day = Rclock.dayOfMonth;
    Global_DateTime.Hour = Rclock.hour;
    Global_DateTime.Minute = Rclock.minute;
    Global_DateTime.Second = Rclock.second;

    sprintf(DateTimeBuffer, "%04d-%02d-%02d %02d:%02d:%02d  ", Rclock.year + 2000, Rclock.month, Rclock.dayOfMonth,
            Rclock.hour, Rclock.minute, Rclock.second);
    Serial.print(DateTimeBuffer);
    Serial.print(DayOfWeek);
    Serial.println();
    return String(DateTimeBuffer);
  }

  // _Sensor_RTC ***********************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    String DateTime = this->GetDateTime();
    JSON_Data += " \"Date\":\"";
    JSON_Data += DateTime.substring(0, 10);
    JSON_Data += "\", \"Time\":\"";
    JSON_Data += DateTime.substring(11, 19);
    JSON_Data += "\",";

    JSON_Short_Data += DateTime.substring(0, 10);
    JSON_Short_Data += "\t";
    JSON_Short_Data += DateTime.substring(11, 19);
    JSON_Short_Data += "\t";
  }

  // _Sensor_RTC ***********************************************************
  // ***********************************************************************
  bool Handle_Serial_Command(String Serial_Command) {
    String LowerCase = Serial_Command;
    LowerCase.toLowerCase();

    if (LowerCase.startsWith("help")) {
      Serial.println(_Serial_Commands_Text);
      this->Print_Current_Time();
      return false;
    } else if (LowerCase.startsWith("?")) {
      Serial.println(_Serial_Commands_Text);
      this->Print_Current_Time();
      return false;
    } else if (LowerCase.startsWith("rtc")) {
      Serial.println(Serial_Command + "##");
      int Delta_Second = (Serial_Command.substring(4)).toInt();
      Serial.println("Correcting RTC by Seconds = " + String(Delta_Second));

      int Delta_Hour = Delta_Second / 3600;
      Delta_Second -= 3600 * Delta_Hour;
      int Delta_Minute = Delta_Second / 60;
      Delta_Second -= 60 * Delta_Minute;
      Serial.println("dHour = " + String(Delta_Hour) + "   dMinute = " + String(Delta_Minute) +
                     "   dSec = " + String(Delta_Second));

      Wire.setClock(_RTC_My_I2C_Clock);
      Rclock.getTime();
      int Hour = Rclock.hour;
      int Minute = Rclock.minute;
      int Second = Rclock.second;
      Rclock.begin();
      Rclock.fillByHMS(Hour + Delta_Hour, Minute + Delta_Minute, Second + Delta_Second);
      Rclock.setTime();
      delay(1000);

      this->Print_Current_Time();
      return true;
    }
    return false;
  }

  // ***********************************************************************
private:
  // ***********************************************************************

  String _Serial_Commands_Text = "------  Sensor_RTC  ------\n\
RTC +[N]  // TODO Advances the time [N] seconds\n\
RTC -[N]  // TODO Set the time back to [N] seconds";

  // _Sensor_RTC ***********************************************************
  // ***********************************************************************
  void Print_Current_Time() {
    Serial.print("Current DateTime = ");
    this->GetDateTime();
  }
};

#endif
