
#ifndef Sensor_OKE4_h
#define Sensor_OKE4_h 0.1

#include <SoftwareSerial.h>

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_OKE4 : public _Sensor_BaseClass {
public:
  SoftwareSerial *mySerial;

  // int Converter_IDs [2] = { 3, 7 } ;
  // int N_Converters      =  2 ;
  int Converter_IDs[2] = {0};
  int N_Converters = 1;

  // ***********************************************************************
  // Creators
  // ***********************************************************************
  _Sensor_OKE4() {
    mySerial = new SoftwareSerial(RX, TX, Inverted);
    Serial.println("V" + String(Sensor_OKE4_h) + "   Sensor_OKE4_h");
    Sensor_OKE4_Available = true;
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    mySerial->begin(300);
    mySerial->setTransmitEnablePin(DIR);
    Find_Converters();
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    switch (_State) {
      // ******************************
      // wait till next sample time
      // ******************************
      case 0:
        //          if ( ( millis () - _Time_Last_Sample ) > 10*1000 ) {
        if ((millis() - _Time_Wait) > _Message_Time) {
          _Time_Last_Sample = millis();
          _Converter = 0;
          _State += 1;
        }
        break;

      // ******************************
      // Send Energy command to next converter
      // ******************************
      case 1:
        _ID = Converter_IDs[_Converter];
        OKE4_Write_Command(_ID, "E?");
        _Time_Wait = millis();
        _State += 1;
        break;

      // ******************************
      // wait _Message_Time msec, then read the current converter
      // ******************************
      case 2:
        // delay ( 500 ) ;   // verlaagd van +1000 naar -100, -200, -300, -400, +500
        if ((millis() - _Time_Wait) > _Message_Time) {
          _Energy = OKE4_Read(); // _ID ) ;
          _Time_Wait = millis();
          _State += 1;
        }
        break;

      // ******************************
      // Send VOLTAGE command to current converter
      // ******************************
      case 3:
        OKE4_Write_Command(_ID, "W02?");
        _Time_Wait = millis();
        _State += 1;
        break;

      // ******************************
      // wait _Message_Time msec, then read the current converter
      // ******************************
      case 4:
        if ((millis() - _Time_Wait) > _Message_Time) {
          _Voltage = OKE4_Read(); // _ID ) ;
          _Time_Wait = millis();
          _State += 1;
        }
        break;

      // ******************************
      // Send CURRENT command to current converter
      // ******************************
      case 5:
        OKE4_Write_Command(_ID, "W04?");
        _Time_Wait = millis();
        _State += 1;
        break;

      // ******************************
      // wait _Message_Time msec, then read the current converter
      // ******************************
      case 6:
        if ((millis() - _Time_Wait) > _Message_Time) {
          _Current = OKE4_Read(); // _ID ) ;

          _Converter += 1;
          if (_Converter < N_Converters) {
            _State = 1;
          } else
            _State += 1;
        }
        break;

      // ******************************
      case 7:
        // Serial.printf ( "ID=%i data read", _ID ) ;
        Serial.printf("ID, N_Power, Energy, Voltage, Current = %i, %i, %i, %i, %i\n", _ID, _N_Power, _Energy, _Voltage,
                      _Current);
        _Sum_Power += _Voltage * _Current;
        _N_Power += 1;
        _State = 0;
        _Time_Wait = millis();
        break;
    }
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"Energy\":";
    JSON_Data += _Energy;
    JSON_Data += ",";
    JSON_Data += " \"Power\":";
    if (_N_Power > 0) {
      _Sum_Power = 10 * _Sum_Power / (_N_Power * mw10);
    }
    JSON_Data += _Sum_Power;
    JSON_Data += ",";

    _N_Power = 0;
    _Sum_Power = 0;
  }

  // ***********************************************************************
  // Change the ID of the selected OKE4
  // ***********************************************************************
  bool Change_ID(uint8_t ID_Old, uint8_t ID_New) {
    OKE4_Write_Command(ID_Old, "IDE=" + ID_New);
    delay(2000);
    OKE4_Write_Command(ID_Old, "RESET");
    Serial.printf("Changed ID %i -> %i", ID_Old, ID_New);
    delay(1000);
    // Clear Read buffer
    OKE4_Read();
    Serial.println("aaapapapappa");
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Read_SX(uint8_t ID) {
    for (int i = 0; i < 9; i++) {
      String Line = "S" + String(i) + "?";
      // Serial.println ( Line ) ;
      OKE4_Write_Command(ID, Line);
      delay(500);
      OKE4_Read_String();
    }
  }

private:
  const int TX = 14;  // 16 can't be used !!!
  const int RX = 15;  // Witty RED
  const int DIR = 13; // Witty BLUE
  const bool Inverted = false;
  const unsigned long _Message_Time = 500; // 400 is te kort

  int _Energy = 0;
  int _Voltage = 0;
  int _Current = 0;
  int _N_Power = 0;
  int _Sum_Power = 0;

  const int mw10 = 9182; // kapot=9254, hobby=9248,  schuur= 9182

  int _State = 0;
  unsigned long _Time_Wait;
  unsigned long _Time_Last_Sample = 0;
  int _Converter;
  int _ID;

  // ***********************************************************************
  // Sends Cmd to the OKE4-100
  // ***********************************************************************
  void OKE4_Write_Command(uint8_t ID, String Cmd) {
    uint8_t Checksum;

    // ************************************
    // set RS485 chip in transmit mode
    // ************************************
    digitalWrite(DIR, HIGH);
    delay(20);

    // ************************************
    // startbyte zero
    // ************************************
    mySerial->write((uint8_t)0x00);

    // ************************************
    // ID byte, to address the current OKE4
    // ************************************
    mySerial->write(ID);
    Checksum = ID;

    // ************************************
    // Send the command
    // ************************************
    for (int i = 0; i < Cmd.length(); i++) {
      mySerial->write(Cmd[i]);
      Checksum += Cmd[i];
      // Serial.print ( Cmd[i] ) ;
    }

    // ************************************
    // If command too short, fill with spaces
    // ************************************
    for (int i = Cmd.length(); i < 8; i++) {
      mySerial->write(0x20);
      Checksum += 0x20;
    }

    // ************************************
    // Send Checksum
    // ************************************
    Checksum = ~Checksum & 0x7F;
    mySerial->write(Checksum);

    // ************************************
    // Set RS485 driver in Receive Mode
    // ************************************
    mySerial->flush();
    delay(5);
    digitalWrite(DIR, LOW);

    mySerial->flush(); /// ADDDDED
    Serial.printf("CMD=%s", Cmd.c_str());
  }

  // ***********************************************************************
  // Reads response from the OKE4-100, stops when no new data available
  // ***********************************************************************
  int OKE4_Read() { // uint8_t ID ) {
    int Result = 0;
    int N = 0;
    int Checksum = 0;
    bool IsNumber = false;

    // Serial.printf ( " N=%i  ", mySerial->available() ) ;
    while (mySerial->available() && (N < 11)) {
      // Serial.printf ( " N=%i  ", mySerial.available() ) ;
      N += 1;
      byte Data = mySerial->read() & 0x7F;
      Checksum += Data;
      // Serial.print ( Data | 0x80, HEX ) ;

      if ((Data == 0x20) || (N >= 11)) {
        IsNumber = false;
      }

      if (IsNumber) {
        Result = 16 * Result + ASCII_2_Hex(Data);
        // Serial.print ( Data, HEX ) ;
        // Serial.print ( "==") ;
        // Serial.println ( Result ) ;
      }

      if (Data == '=') {
        IsNumber = true;
      }
    }

    Checksum = Checksum & 0xFF;
    if (Checksum != 0xFF) {
      // Serial.print ( "Check" ) ;
      // Serial.println ( Checksum ) ;
      Result = -1;
    }
    // printf ( "ppp=%i\n", Result ) ;

    while (mySerial->available()) {
      byte Data = mySerial->read();
    }
    return Result;
  }

  // ***********************************************************************
  // Reads response from the OKE4-100, stops when no new data available
  // ***********************************************************************
  String OKE4_Read_String() {
    int Result = 0;
    int N = 0;
    int Checksum = 0;
    String Response = "";

    // Serial.printf ( " N=%i  ", mySerial->available() ) ;
    while (mySerial->available() && (N < 11)) {
      // Serial.printf ( " N=%i  ", mySerial.available() ) ;
      N += 1;
      byte Data = mySerial->read() & 0x7F;
      Checksum += Data;
      // Serial.print ( Data | 0x80, HEX ) ;

      if (N > 4) {
        Response += (char)Data;
        // Serial.print ( Data, HEX ) ;
        // Serial.print ( "==") ;
        Serial.println(Response);
      }
    }

    Checksum = Checksum & 0xFF;
    if (Checksum != 0xFF) {
      // Serial.print ( "Check" ) ;
      // Serial.println ( Checksum ) ;
      // Result = -1 ;
    }
    // printf ( "ppp=%i\n", Result ) ;

    while (mySerial->available()) {
      byte Data = mySerial->read();
    }
    return Response;
  }

  // ***********************************************************************
  // ***********************************************************************
  int ASCII_2_Hex(uint8_t Data) {
    if (Data <= 0x39) {
      return Data - 0x30;
    } else {
      return Data - 'A' + 0x0A;
    }
  }

  // ***********************************************************************
  // find all converters
  // ***********************************************************************
  void Find_Converters() {
    int Result = 0;
    for (int ID = 0; ID < 3; ID++) {
      //        Result  = OKE4_Response ( ID, "E?" ) ;
      OKE4_Write_Command(ID, "E?");
      delay(_Message_Time); // verlaagd van +1000 naar -100, -200, -300, -400, +500
      Result = OKE4_Read(); // ID ) ;
      if (Result > 0) {
        Serial.printf("\nFound OKE4 with ID = %i\n", ID);
      } else
        Serial.print(".");
    }
  }
};
#endif
