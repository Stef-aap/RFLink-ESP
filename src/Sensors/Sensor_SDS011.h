// ***********************************************************************************
// Version 0.7
//    - support for Webserver Settings
//
// Version 0.6
//    - Redirect_Serial library used
//    - Measurement time reduced from 15 sec to 5 sec
//
// Version 0.5
//    - Missed sample improved
//    - Measurement time increased from 5 sec to 15 sec
//
// Version 0.4
//    - new lib definitions + much simpler
//    - Comm used is defined as a pointer but still connected to the real swapped Serial
// ***********************************************************************************

#ifndef Sensor_SDS011_h
#define Sensor_SDS011_h 0.7

#include "Sensor_Base.h"

const int SDS011_UART0 = 0;
const int SDS011_UART1 = 1;
const int SDS011_UART2 = 2;
const int SDS011_Swapped = 3;

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_SDS011 : public _Sensor_BaseClass {

public:
  HardwareSerial *_Serial_SDS011;

  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Sensor_SDS011() {
    Serial.println("V" + String(Sensor_SDS011_h) + "   Sensor_SDS011.h");
    Serial.println("\n──────  CREATE  ──────  " + Version_Name);
    _JSON_Short_Header = "PM_2_5\tPM_10\t";
  }

  // ***********************************************************************
  // Problem is that the sensor sometimes (especially after programming) enters a mode,
  // in which the sensor does not work and the value of the sensor cannot be extracted,
  // other than by powerup
  // The _SDS_Continuous_cmd therefore does not work.
  // ***********************************************************************
  void setup() {
    _Serial_SDS011 = &Serial_Device;
    _Serial_SDS011->begin(9600);
    _Serial_SDS011->write(_SDS_Stop_cmd, sizeof(_SDS_Stop_cmd));
    _Serial_SDS011->flush();
  }

  // ***********************************************************************
  // ***********************************************************************
  void loop() {
    unsigned long Now = millis();

    // *******************************
    // _State = 0 sleep mode
    // *******************************
    if (_State == 0) {
      if (Now - _Last_Loop_Time > 120000) {
        // ***************************************************************************
        // this command starts the sensor (i.e. the laserdiode and the ventilator) and
        // while the sensor is working, it will send continuously the measured data
        // ***************************************************************************
        _Serial_SDS011->write(_SDS_Start_cmd, sizeof(_SDS_Start_cmd));
        _State = 1;
        _Last_Loop_Time = Now;
      }
    }

    // *******************************
    // _State = 1 warm up time
    // *******************************
    else if (_State == 1) {
      if (Now - _Last_Loop_Time > 15000) {
        _State = 2;
        _Last_Loop_Time = Now;
        _NSample = 0;
        _Missed_Sample = 0;
        _Sum_PM_2_5 = 0;
        _Sum_PM_10 = 0;
        _Min_PM_2_5 = 2000;
        _Min_PM_10 = 2000;
        _Max_PM_2_5 = 0;
        _Max_PM_10 = 0;
        // ******************************************************************
        // because the sensor (if it is on) a message send every second
        // flush the incoming buffer
        // ******************************************************************
        while (_Serial_SDS011->available() > 0) {
          char kar = _Serial_SDS011->read();
        }
      }
    }

    // *******************************
    // _State = 2 Measuring time
    // *******************************
    else {
      if (Now - _Last_Loop_Time > 1000) {
        _Last_Loop_Time = Now;
        // *************
        // take a sample
        // *************
        if (_Get_Response(0x04)) {
          int PM_2_5 = 256 * _Data[1] + _Data[0];
          int PM_10 = 256 * _Data[3] + _Data[2];

          _Sum_PM_2_5 += PM_2_5;
          _Sum_PM_10 += PM_10;
          _NSample += 1;

          _Max_PM_2_5 = std::max(_Max_PM_2_5, PM_2_5);
          _Min_PM_2_5 = std::min(_Min_PM_2_5, PM_2_5);
          _Max_PM_10 = std::max(_Max_PM_10, PM_10);
          _Min_PM_10 = std::min(_Min_PM_10, PM_10);
        } else {
          _Missed_Sample += 1;
          if (_Missed_Sample > 10) {
            _Last_Loop_Time = Now;
            _State = 0;
            _fPM_2_5_Hist = 0;
            _fPM_10_Hist = 0;
          }
        }

        // ***************************************
        // Measurement is ready
        // ***************************************
        if (_NSample > 15) {
          // **********************
          // stop the SDS011 sensor
          // **********************
          _Serial_SDS011->write(_SDS_Stop_cmd, sizeof(_SDS_Stop_cmd));

          _NSample -= 2;
          _fPM_2_5 = 0.1 * (_Sum_PM_2_5 - _Min_PM_2_5 - _Max_PM_2_5) / _NSample;
          _fPM_10 = 0.1 * (_Sum_PM_10 - _Min_PM_10 - _Max_PM_10) / _NSample;

          _fPM_2_5_Hist = _fPM_2_5;
          _fPM_10_Hist = _fPM_10;

          // *************************************************
          // and here the JSON string to send to all web api's
          // *************************************************
          sprintf(msg, "{\"value_type\":\"SDS_P1\",\"value\":\"%.2f\"},{\"value_type\":\"SDS_P2\",\"value\":\"%.2f\"},",
                  _fPM_10, _fPM_2_5);
          _Luftdaten_String = String(msg);

          _Last_Loop_Time = Now;
          _State = 0;
        }
      }
    }
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    JSON_Data += " \"PM_2_5\":";
    JSON_Data += String(_fPM_2_5_Hist, 1);
    JSON_Data += ",";
    JSON_Data += " \"PM_10\":";
    JSON_Data += String(_fPM_10_Hist, 1);
    JSON_Data += ",";

    JSON_Short_Data += String(_fPM_2_5_Hist, 1);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(_fPM_10_Hist, 1);
    JSON_Short_Data += "\t";
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  String Get_JSON_LuftData() { return _Luftdaten_String; };

  // ***********************************************************************
private:
  // ***********************************************************************
  unsigned long _Last_Loop_Time = -120000;
  int _NSample = 0;
  int _State = 0;
  int _Sum_PM_2_5;
  int _Sum_PM_10;
  int _Min_PM_2_5;
  int _Min_PM_10;
  int _Max_PM_2_5;
  int _Max_PM_10;
  int _Missed_Sample;
  float _fPM_2_5;
  float _fPM_10;
  float _fPM_2_5_Hist = 0;
  float _fPM_10_Hist = 0;
  String _Luftdaten_String;
  uint8_t _Data[20];
  int _Data_Len = 0;
  int _Soft_RX = -1;
  int _Soft_TX = -1;

  uint8_t _SDS_Stop_cmd[19] = {0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB};
  uint8_t _SDS_Start_cmd[19] = {0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0xAB};
  uint8_t _SDS_Continuous_cmd[19] = {0xAA, 0xB4, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x07, 0xAB};

  // ***********************************************************************
  // All responses have the same length
  //   and are roughly build in the same way.
  //   So we can use the same function for alle responses.
  //
  // Code     = comand-ID, which is reflected in third byte of the response
  //            (except when quering the data)
  // Response = the second and third byte of the response
  //
  // Code   Respons   Meaning
  //  02    C5, 02    Set the work-mode Passive = query / active
  //  04    C0        Query the sensor data
  //  05    C5, 05    Set Device-ID
  //  06    C5, 06    Start / Stop the sensor ( laser + ventilator)
  //  07    C5, 07    Get Firmware and Device-ID
  //  08    C5, 08    Set working period
  // ***********************************************************************
  boolean _Get_Response(uint8_t Cmd2) {
    int CheckSum = 0;
    int Answer_Len = 0;

    while (_Serial_SDS011->available() > 0) {
      char kar = _Serial_SDS011->read();

      switch (Answer_Len) {
        case 0:
          if (kar == 0xAA) {
            Answer_Len += 1;
          } else {
            Debugf(">>>>>>>>>>>>>>>>>> NNNN  %i    %i", kar, _Serial_SDS011->available());
            while (_Serial_SDS011->available() > 0) {
              char kar = _Serial_SDS011->read();
            }
            return false;
          }
          break;

        case 1:
          if (((Cmd2 == 0x04) && (kar == 0xC0)) || ((Cmd2 != 0x04) && (kar == 0xC5))) {
            Answer_Len += 1;
          } else {
            Answer_Len = 0;
          }
          break;

        case 2:
          // *******************************************
          // for all commands, except the data query,
          //   the command ID is reflected in the answer
          //   so check it
          // Because the data is send autonomuously by the sensor
          //    this will happen often ( so not realy an error)
          //    thus we only give a message if MAX info
          // *******************************************
          if ((Cmd2 != 0x04) && (Cmd2 != kar)) {
            Answer_Len = 0;
            break;
          }
          // ****************************************
          // checksum is calculated over bytes 2 .. 8
          // ****************************************
          Answer_Len += 1;
          CheckSum = kar;
          break;

        case 8:
          if (kar == (CheckSum % 256)) {
            Answer_Len += 1;
          }
          // ********************************************
          // If Checksum is wrong, start all over
          // ********************************************
          else {
            Answer_Len = 0;
          }
          break;

        case 9:
          if (kar == 0xAB) {
            _Data_Len = Answer_Len - 2;
            while (_Serial_SDS011->available() > 0) {
              char kar = _Serial_SDS011->read();
            }
            return true;
          } else {
            Answer_Len = 0;
          }
          break;

        default:
          Answer_Len += 1;
          CheckSum += kar;
      }
      // ***********************************************
      // if too much, flush buffer and retur false
      // ***********************************************
      if (Answer_Len >= 10) {
        while (_Serial_SDS011->available() > 0) {
          char kar = _Serial_SDS011->read();
        }
        return false;
      }

      if (Answer_Len >= 3) {
        _Data[Answer_Len - 3] = kar;
      }
    }
    // *****************************************************
    // No more data from sensor, and no good answer received
    // *****************************************************
    _Data_Len = 0;
    return false;
  }
};

#endif
