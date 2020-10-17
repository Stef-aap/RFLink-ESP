// ***********************************************************************************
// Version 1.4, 28-01-2020, SM, checked by ...
//    - _MHZ14_Servo_Count could go through zero, solved
//
// Version 1.3, 2019-09-05, SM, checked by ...
//    - Read servo via interrupt routine
//
// Version 1.2, 08/18/2019, SM, checked by ...
//    - made suitable for WebSettings
//
// Version 1.1, 06/13/2019, SM, checked by ...
//    - Get_Name_Version () added
//    - JSON_Short_Data added
//
// Version 1.0, 3/8/2019
//    - use of Redirect_Serial
//
// Version 0.9, 2019-03-07
//    - swapped pins, but no new instance
//
// Version 0.8, 2019-03-05
//    - PWM procedure with a timeout, so it no longer hangs there
//
// Version 0.8, 12/5/2018
//    - private Serial (swapped) added
//
// Version 0.7, 12/3/2018
//    - Software serial mode added
//
// Version 0.6, 12/2/2018
//    - added Serial communication via swapped pins
//        TX from 1 to 15 (blue wire)
//        RX from 3 to 13 (green wire)
//      necessary to use the following settings
//        _Debug_Over_Serial = false;
//        Serial_Setup (9600, true);
//    - Calibration: Stable for 20 minutes in outdoor air, then brown wire> 7 seconds to ground
//    - added disable calibration through serial (not sure this works for MHZ14, commands are taken from MHZ19)
//
// Version 0.5
//    - PWM formula corrected slightly, so 400 comes out nicely
//
// Version 0.4
//    - Domoticz output removed
//    - PWM measurement via PulseIn function
//
// Version 0.3
//    - Domoticz output added
//
// Version 0.2
//    - because software serial can ran out of sync
//    - changed to PWM detection
// The advantage of PWM detection is that it consumes only 1 IO-pin.
// The disadvantage is that timing in milliseconds might not be very precise.
//   with an Interrupt Service Routine (ISR) this problem can be bypassed.
//
// Version 0.1

#ifndef Sensor_MHZ14_h
#define Sensor_MHZ14_h 1.4

#include "Sensor_Base.h"

const int MHZ14_Servo = 0;
const int MHZ14_SOFT_SERIAL = 1; // software serial on swap pins
const int MHZ14_SWAP_SERIAL = 2; // hardware Serial on swapped pins

// ***********************************************************************************
// ***********************************************************************************
int _MHZ14_Servo_Pin = 13;

unsigned long _MHZ14_Servo_Start = 0;
int _MHZ14_Servo_Pulse = 0;
int _MHZ14_Servo_High = 0;
int _MHZ14_Servo_Low = 0;
int _MHZ14_Servo_Count = 0;

static void _MHZ14_Servo_Int() ICACHE_RAM_ATTR;
static void _MHZ14_Servo_Int() {
  if (digitalRead(_MHZ14_Servo_Pin) == HIGH) {
    _MHZ14_Servo_Low = millis() - _MHZ14_Servo_Start;
    _MHZ14_Servo_Start = millis();
    _MHZ14_Servo_Count += 1;
  } else {
    _MHZ14_Servo_High = millis() - _MHZ14_Servo_Start;
    _MHZ14_Servo_Start = millis();
  }
}

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_MHZ14 : public _Sensor_BaseClass {

public:
  HardwareSerial *Serial_CO2;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_MHZ14(char *Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_MHZ14(int Servo_Pin = 13, int Comm_Mode = MHZ14_Servo, int Reset_Pin = -1) {
    _MHZ14_Servo_Pin = Servo_Pin;
    this->_MHZ14_Reset_Pin = Reset_Pin;
    this->_Comm_Mode = Comm_Mode;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    if (_Comm_Mode == MHZ14_SWAP_SERIAL) {
      // ****************************
      // First way does not work
      // ****************************
      Serial_CO2 = &Serial_Device;
      // ****************************
    }
    Version_Name = "V" + String(Sensor_MHZ14_h) + "  ------  Sensor_MHZ14.h";
    Serial.println("\n------  CREATE  ------  " + Version_Name);

    _JSON_Short_Header = "CO2\t";
    _JSON_Long_Header = "MHZ14_CO2_ppm\t";
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    if (_MHZ14_Reset_Pin >= 0) {
      pinMode(_MHZ14_Reset_Pin, OUTPUT);
      digitalWrite(_MHZ14_Reset_Pin, HIGH);
    }

    if (_Comm_Mode == MHZ14_Servo) {
      pinMode(_MHZ14_Servo_Pin, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(_MHZ14_Servo_Pin), _MHZ14_Servo_Int, CHANGE);
    }

    //*****************************************
    // normally   UART0 : TX=GPIO1,   RX=GPIO3
    // after swap UART0 : TX=GPIO15,  RX=GPIO13
    //*****************************************
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    _Comm_Mode = Settings.Get_Set_Default_Int("MHZ14 Comm Mode", MHZ14_Servo, Force);
    _MHZ14_Servo_Pin = Settings.Get_Set_Default_Int("MHZ14 PWM Pin", 13, Force);
    _MHZ14_Reset_Pin = Settings.Get_Set_Default_Int("MHZ14 Reset Pin", -1, Force);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int Mode = Settings.Read_Int("MHZ14 Comm Mode");
    int PWM = Settings.Read_Int("MHZ14 PWM Pin");
    int Reset = Settings.Read_Int("MHZ14 Reset Pin");

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "MHZ14 Comm Mode") {
        if (New_Value_Int != Mode) {
          _My_Settings_Buffer["MHZ14 Comm Mode"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "MHZ14 PWM Pin") {
        if (New_Value_Int != PWM) {
          _My_Settings_Buffer["MHZ14 PWM Pin"] = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "MHZ14 Reset Pin") {
        if (New_Value_Int != Reset) {
          _My_Settings_Buffer["MHZ14 Reset Pin"] = New_Value_Int;
        }
      }
    }
    return Restart;
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    if (_Comm_Mode == MHZ14_Servo) {
      if (_MHZ14_Servo_Count > _MHZ14_Servo_Count_Prev) {
        _MHZ14_Servo_Count_Prev = _MHZ14_Servo_Count;

        int CO2 = 5000 * (_MHZ14_Servo_High - 2) / (_MHZ14_Servo_High + _MHZ14_Servo_Low - 4);

        _MHZ14_CO2 += CO2;
        _MHZ14_N += 1;
      } else if (_MHZ14_Servo_Count < 0) {
        _MHZ14_Servo_Count = 0;
        _MHZ14_Servo_Count_Prev = 0;
      }

      yield();

      // ***********************************************************
      // reset the MHZ14 sensor, to prevent a re-calibration
      // ***********************************************************
      if (_MHZ14_Reset_Pin >= 0) {
        if ((millis() - _Reset_23_Hour) > _Hour_23) {
          _Reset_23_Hour = millis();
          digitalWrite(_MHZ14_Reset_Pin, LOW);
          delay(300);
          digitalWrite(_MHZ14_Reset_Pin, HIGH);
        }
      }
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  void Print_Help() {
    Serial.print("Mode      = ");
    switch (this->_Comm_Mode) {
      case 0:
        Serial.println("Servo-Mode");
        break;
      case 1:
        Serial.println("Software Serial on Swapped Pins");
        break;
      case 2:
        Serial.println("Hardware Serial on Swapped Pins ( TX = 15,  RX = 13 )");
        break;
      default:
        Serial.println("Unknown");
        break;
    }
    Serial.print("Servo_Pin = ");
    Serial.println(_MHZ14_Servo_Pin);
    Serial.print("Reset Pin = ");
    Serial.println(this->_MHZ14_Reset_Pin);
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    if (_Comm_Mode == MHZ14_Servo) {
      //_MHZ14_PPM  = Get_ppm_Servo  () ;
      if (_MHZ14_N > 0) {
        _MHZ14_PPM = _MHZ14_CO2 / _MHZ14_N;
        _MHZ14_CO2 = 0;
        _MHZ14_N = 0;
      }
    } else {
      _MHZ14_PPM = Get_ppm_Uart();
    }
    JSON_Data += " \"MHZ14_CO2_ppm\":";
    JSON_Data += String(_MHZ14_PPM);
    JSON_Data += ",";

    JSON_Short_Data += String(_MHZ14_PPM);
    JSON_Short_Data += "\t";
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Hardware_Test(int Test_Nr = 1) {
    int ppm_Uart = Get_ppm_Uart();
    Debugf("CO2 UART/Servo [ppm] = %i / %i,  T[C]=%i", ppm_Uart, _Temperature_Uart);
    return true;
  }

  // ***********************************************************************
  // ***********************************************************************
  int Get_ppm_Uart() {

    // *****************************
    // ask for new measurement value
    // *****************************
    byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    byte response[9]; // for answer

    // *****************************
    // Clear input buffer (flush waits until output buffer is empty)
    // *****************************
    while (Serial_CO2->available() > 0) {
      char kar = Serial_CO2->read();
    }

    Serial_CO2->write(cmd, 9); // request PPM CO2
    memset(response, 0, 9);

    int waited = 0;
    while (Serial_CO2->available() == 0) {
      delay(100);
      if (waited++ > 10) {
        return false;
      }
    }

    int count = Serial_CO2->readBytes(response, 9);
    if (count < 9) {
      return false;
    }

    byte check = getCheckSum(response);
    if (response[8] != check) {
      return false;
    }
    int ppm_uart = 256 * (int)response[2] + response[3];
    _Temperature_Uart = response[4] - 40;
    return ppm_uart;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  int _MHZ14_CO2 = 0;
  int _MHZ14_N = 0;
  int _MHZ14_PPM = 0;
  int _MHZ14_Servo_Count_Prev = 0;

  int _MHZ14_Reset_Pin;
  int _Comm_Mode = MHZ14_Servo;
  uint8_t _Temperature_Uart;
  unsigned long _Reset_23_Hour = 0;
  unsigned long _Hour_23 = 23 * 60 * 60 * 1000;

  // ***********************************************************************
  // ***********************************************************************
  byte getCheckSum(byte *packet) {
    byte i;
    unsigned char checksum = 0;
    for (i = 1; i < 8; i++) {
      checksum += packet[i];
    }
    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
  }
};

#endif
