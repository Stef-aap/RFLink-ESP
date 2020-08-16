// Version 0.1  16-06-2019, SM

#ifndef My_DateTime_h
#define My_DateTime_h 0.1

#include <Time.h>

// ****************************************************************************
// ****************************************************************************
String UnixTime_2_String(time_t DateTime) { Serial.println("TODO: UnixTime_2_String"); }

// ****************************************************************************
//    String_2_UnixTime ( "1970-01-01 00:00:01" ) ;
// ****************************************************************************
time_t String_2_UnixTime(String calTimestamp) {
  struct tm tm;
  // Serial.println("Parsing " + calTimestamp);
  String year = calTimestamp.substring(0, 4);
  String month = calTimestamp.substring(5, 7);
  if (month.startsWith("0")) {
    month = month.substring(1);
  }
  String day = calTimestamp.substring(8, 10);
  if (day.startsWith("0")) {
    month = day.substring(1);
  }
  tm.tm_year = year.toInt() - 1900;
  tm.tm_mon = month.toInt() - 1;
  tm.tm_mday = day.toInt();
  tm.tm_hour = calTimestamp.substring(11, 13).toInt();
  tm.tm_min = calTimestamp.substring(14, 16).toInt();
  tm.tm_sec = calTimestamp.substring(17, 19).toInt();

  /*
char datetimeBuffer[20] = "";
sprintf(datetimeBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
                        tm.tm_year, tm.tm_mon, tm.tm_mday,
                        tm.tm_hour, tm.tm_min, tm.tm_sec ) ;
Serial.println(String(datetimeBuffer )) ;
Serial.println ( mktime ( &tm ) ) ;
//*/

  return mktime(&tm);
}

#endif