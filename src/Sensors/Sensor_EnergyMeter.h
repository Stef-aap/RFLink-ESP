// Version 0.3, 25-06-2019, SM
//    - better timing of serial receiver
//    - check on 18 bytes received
//    - extra check values of Voltage and LineFrequency
//
// Version 0.2, 29-05-2019, SM
//    - back to software serial
//      (I really wonder if the hardware serial can handle that weird Baudrate)
//
// Version 0.1, 13-04-2019, SM
//   - initial version

#ifndef Sensor_EnergyMeter_h
#define Sensor_EnergyMeter_h 0.2

#include <SoftwareSerial.h>

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_EnergyMeter : public _Sensor_BaseClass {

public:
  SoftwareSerial *_Serial_EnergyMeter;

  // ***********************************************************************
  // Creator,
  // only Rx on pin 0 to 5 and 12 to 15 are interrupt driven !!
  // ***********************************************************************
  _Sensor_EnergyMeter(int RX = 13, int TX = -1) {
    bool Inverted = true;
    _Serial_EnergyMeter = new SoftwareSerial(RX, TX, Inverted);
    Serial.println("V" + String(Sensor_EnergyMeter_h) + "   Sensor_EnergyMeter.h");
    if ((RX >= 6) && (RX <= 11)) {
      Serial.println("=====  WARNING, software RX has no interrupt !!!!!");
    }
    JSON_Short_Header = "V\tI\tPavg\tPact\tPf\tFreq\t";
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() { _Serial_EnergyMeter->begin(60000); }

  // ***********************************************************************
  // every second one sequence is sent by the energymeter
  // ***********************************************************************
  void loop() {
    if (_Serial_EnergyMeter->available()) {
      delay(15); // at 60000 bits/sec, 18 bytes take 3 msec, yet 5 msec is too little
      if (_Serial_EnergyMeter->available() == 18) {
        int Data[6];
        for (int i = 0; i < 6; i++) {
          Data[i] = 0;
          Data[i] += (int)_Serial_EnergyMeter->read();
          Data[i] += 256 * (int)_Serial_EnergyMeter->read();
          Data[i] += 256 * 256 * (int)_Serial_EnergyMeter->read();
        }

        double __Vrms = ((double)Data[0]) / 0x1000000 / _Vgain * 400;
        double __LineFrequency = ((double)Data[5]) / 0x01999A * 50;

        if ((__Vrms > 210) && (__Vrms < 250) && (__LineFrequency > 48) && (__LineFrequency < 52)) {
          _Vrms += __Vrms;
          _Irms += ((double)Data[1]) / 0x1000000 / _Igain * 120;
          _Pavg += ((double)Data[2]) / 0x800000 / _Igain / _Vgain * 120 * 400;
          _Pactive += ((double)Data[4]) / 0x800000 / _Igain / _Vgain * 120 * 400;
          _Pf += ((double)Data[3]) / 0x800000;
          _LineFrequency += __LineFrequency;
          _N_Measurement += 1;
          Debug("N_Meas = " + String(_N_Measurement));
        }
      } else {
        Debug("SoftSerial Available = " + String(_Serial_EnergyMeter->available()));
        while (_Serial_EnergyMeter->available()) {
          _Serial_EnergyMeter->read();
        }
      }
    }
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    if (_N_Measurement >= 0) {
      JSON_Data += "\"V\":";
      JSON_Data += String(_Vrms / _N_Measurement, 1);
      JSON_Data += ", \"I\":";
      JSON_Data += String(_Irms / _N_Measurement, 1);
      JSON_Data += ", \"Pavg\":";
      JSON_Data += String(_Pavg / _N_Measurement, 1);
      JSON_Data += ", \"Pact\":";
      JSON_Data += String(_Pactive / _N_Measurement, 1);
      JSON_Data += ", \"Pf\":";
      JSON_Data += String(_Pf / _N_Measurement, 1);
      JSON_Data += ",\"Freq\":";
      JSON_Data += String(_LineFrequency / _N_Measurement, 1);
      JSON_Data += ",";

      JSON_Short_Data += String(_Vrms / _N_Measurement, 1);
      JSON_Short_Data += "\t";
      JSON_Short_Data += String(_Irms / _N_Measurement, 1);
      JSON_Short_Data += "\t";
      JSON_Short_Data += String(_Pavg / _N_Measurement, 1);
      JSON_Short_Data += "\t";
      JSON_Short_Data += String(_Pactive / _N_Measurement, 1);
      JSON_Short_Data += "\t";
      JSON_Short_Data += String(_Pf / _N_Measurement, 1);
      JSON_Short_Data += "\t";
      JSON_Short_Data += String(_LineFrequency / _N_Measurement, 1);
      JSON_Short_Data += "\t";

      _Vrms = 0;
      _Irms = 0;
      _Pavg = 0;
      _Pactive = 0;
      _Pf = 0;
      _LineFrequency = 0;
      _N_Measurement = 0;
    }
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  double _Vrms = 0;
  double _Irms = 0;
  double _Pavg = 0;
  double _Pactive = 0;
  double _Pf = 0;
  double _LineFrequency = 0;
  int _N_Measurement = 0;
  double _Igain = (double)0xE4AEDF / 0x400000;
  double _Vgain = (double)0x37D026 / 0x400000;
};
#endif
