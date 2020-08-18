
#ifndef Sensor_SlimmeMeter_h
#define Sensor_SlimmeMeter_h 0.1

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_SlimmeMeter : public _Sensor_BaseClass {
public:
  // ***********************************************************************
  // Creators
  // ***********************************************************************
  _Sensor_SlimmeMeter() { Serial.println("V" + String(Sensor_SlimmeMeter_h) + "   Sensor_SlimmeMeter_h"); }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    _Time = millis();
    _Time_Delta = _Time - _Time_Old;
    _Value = _Read_Watermeter();
    // *******************************************
    // in State = 0 wachten we op een verandering,
    //   om vervolgens (met onthouden van de starttijd)
    //   naar State = 1 te springen
    // *******************************************
    if (_State == 0) {
      if (_Value != _Value_Old) {
        _State = 1;
        _Time_Old = _Time;
      }
    }
    // *******************************************
    // In Steta =1, vallen we (zonder iets te doen) terug
    //    als het nivo weer het oude was (korte stoorpiek)
    // Zodra de verandering langer duurt dan de debounce tijd,
    //    hebben we een geldige verandering, dus count ophogen
    //    en ook terugvallen, met registratie van huidige stand
    // *******************************************
    else {
      if (_Value == _Value_Old) {
        _State = 0;
      } else if (_Time_Delta > _Time_Debounce) {
        _Count += 1;
        _State = 0;
        _Value_Old = _Value;
      }
    }
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"Water\":";
    JSON_Data += _Count;
    JSON_Data += ",";
    _Count = 0;
  }

private:
  int _State = 0;
  int _Count = 0;
  unsigned long _Time_Debounce = 50;
  unsigned long _Time;
  unsigned long _Time_Delta;
  unsigned long _Time_Old;
  bool _Value;
  bool _Value_Old = false;

  // ***********************************************************************
  // ***********************************************************************
  bool _Read_Watermeter() {
    delay(5); // anders crashed het programma !!!
    return analogRead(A0) > 150;
  }
};
#endif
