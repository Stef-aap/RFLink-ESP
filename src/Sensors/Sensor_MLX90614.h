
// Version 0.2, 20-12-2019, SM
//
// Version 0.1, Initial_Release, 06-12-2019, SM
// ATTENTION THE EMISSIVITY IS CURRENTLY AT 1

#ifndef Sensor_MLX90614_h
#define Sensor_MLX90614_h 0.2

#include "Hardware/Stef_SparkFunMLX90614.h"
#include "Sensor_Base.h"
#include <SimpleKalmanFilter.h>
#include <Wire.h>

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_MLX90614 : public _Sensor_BaseClass {
public:
  // ***********************************************************************
  // Creators
  // ***********************************************************************
  _Sensor_MLX90614(int I2C_Address = 0x5A) {
    this->_I2C_Address = I2C_Address;
    this->MLX = new IRTherm();
    Muur_Kalman = new SimpleKalmanFilter(1, 1, 0.01);
    Muur_Kalman2 = new SimpleKalmanFilter(2, 2, 0.01);
    // Muur_Kalman2  = new SimpleKalmanFilter ( 5, 5, 0.01  ) ;
    Ruimte_Kalman = new SimpleKalmanFilter(1, 1, 0.01);
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {
    this->MLX->begin(this->_I2C_Address);
    MLX->setUnit(TEMP_C);

    // FIRST BUT MAKE SOME DUMPEE GOOD

    float Emissivity = MLX->readEmissivity();
    Serial.println("MLX90614 at " + String(this->_I2C_Address) + "     Emissivity = " + String(Emissivity, 2));

    int Config = MLX->readConfig();
    int FIR = MLX->readFIR();
    int IIR = MLX->readIIR();
    Serial.println("Config : " + String(Config) + "     FILTERS:    FIR = " + String(FIR) +
                   "       IIR = " + String(IIR));
  }

  // ***********************************************************************
  // The sensor cannot be addressed during the first 20 msec
  // ***********************************************************************
  void loop() {
    if ((millis() - _Last_Time) > 100) {
      if (MLX->read()) {
        float TMuur = MLX->object();
        float TRuimte = MLX->ambient();

        this->_TRuimte_Filt = (1 - this->_alfa_Ruimte) * this->_TRuimte_Filt + this->_alfa_Ruimte * TRuimte;

        this->_TMuur_Kalman = Muur_Kalman->updateEstimate(TMuur);
        this->_TMuur_Filt = Muur_Kalman2->updateEstimate(TMuur);
        this->_TRuimte_Kalman = Ruimte_Kalman->updateEstimate(TRuimte);

        this->_T_Object += TMuur;
        this->_T_Ambient += TRuimte;
        this->_N_Measure += 1;
        _Last_Time = millis();
      }
    }
  }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    if (this->_N_Measure > 10) {
      float TRuimte = this->_T_Ambient / this->_N_Measure;
      float TMuur = this->_T_Object / this->_N_Measure;

      JSON_Data += " \"T_Lucht_" + String(this->_I2C_Address) + "\":";
      JSON_Data += TRuimte;
      JSON_Data += ", \"T_Muur_" + String(this->_I2C_Address) + "\":";
      JSON_Data += TMuur;

      JSON_Data += ", \"T_Muur2_" + String(this->_I2C_Address) + "\":";
      JSON_Data += this->_TMuur_Filt;
      JSON_Data += ", \"T_Lucht2_" + String(this->_I2C_Address) + "\":";
      JSON_Data += this->_TRuimte_Filt;

      JSON_Data += ", \"T_Muur_Kalman_" + String(this->_I2C_Address) + "\":";
      JSON_Data += this->_TMuur_Kalman;
      JSON_Data += ", \"T_Lucht_Kalman_" + String(this->_I2C_Address) + "\":";
      JSON_Data += this->_TRuimte_Kalman;

      JSON_Data += ",";

      Serial.print("I2C = ");
      Serial.print(this->_I2C_Address);
      Serial.print(this->_TMuur_Filt);
      Serial.println();
      Serial.println();

      this->_T_Ambient = 0;
      this->_T_Object = 0;
      this->_N_Measure = 0;
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  bool Hardware_Test(int Test_Nr = 1) {
    Serial.println("MLX90614 at " + String(this->_I2C_Address));
    Serial.println(MLX->readEmissivity());
    return true;
  }

  // ***********************************************************************
  // ***********************************************************************
  float Read_T_Ambient() {
    if (this->_N_Measure > 0) {
      this->_Read_T_Ambient = this->_T_Ambient / this->_N_Measure;
    }
    return this->_Read_T_Ambient;
  }
  // ***********************************************************************
  // ***********************************************************************
  float Read_T_Object() {
    if (this->_N_Measure > 0) {
      this->_Read_T_Object = this->_T_Object / this->_N_Measure;
    }
    return this->_Read_T_Object;
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  unsigned long _Last_Time = millis();
  int _I2C_Address;
  float _T_Ambient = 0;
  float _T_Object = 0;
  int _N_Measure = 0;
  float _Largest[5] = {0, 0, 0, 0, 0};
  float _Smallest[5] = {1000, 1000, 1000, 1000, 1000};
  float _TMuur_Filt = 15;
  float _alfa = 0.001;
  float _TRuimte_Filt = 15;
  float _alfa_Ruimte = 0.001;

  float _Read_T_Ambient = -99;
  float _Read_T_Object = -99;

  float _TMuur_Kalman = 15;
  float _TRuimte_Kalman = 15;

  IRTherm *MLX;
  SimpleKalmanFilter *Muur_Kalman;
  SimpleKalmanFilter *Ruimte_Kalman;

  SimpleKalmanFilter *Muur_Kalman2;
};
#endif
