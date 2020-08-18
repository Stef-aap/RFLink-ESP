
// Version 0.2, Initial_Release, 20-12-2019, SM
//
// Version 0.1, Initial_Release, 06-12-2019, SM
//    This relies on the driver written by Melexis and can be found at:
//    https://github.com/melexis/mlx90640-library

#ifndef Sensor_MLX90640_h
#define Sensor_MLX90640_h 0.2

#include "Hardware/MLX90640_API.h"
#include "Hardware/MLX90640_I2C_Driver.h"
#include "driver/adc.h"
#include <SimpleKalmanFilter.h>
#include <Wire.h>

/*
SimpleKalmanFilter ( e_mea, e_est, q ) ;
 e_mea: Measurement Uncertainty
 e_est: Estimation Uncertainty
 q: Process Noise

 e_mea: Measurement Uncertainty - How much do we expect to our measurement vary
e_est: Estimation Uncertainty - Can be initilized with the same value as e_mea since the kalman filter will adjust its
value. q: Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended
0.01. Should be tunned to your needs.
*/

// 0.01 oorspronkelijk
// 0.1  verergert de ruis
// 1.0  verergert de ruis
// 0.01 en 0.001 exact gelijk
float Uncertainty = 0.3;

// 90 is veel te hoog (zeer traag)
// 10 is veel te hoog (zeer traag)
float Process_Variance = 0.0003;

SimpleKalmanFilter simpleKalmanFilter_T1(Uncertainty, Uncertainty, Process_Variance);
SimpleKalmanFilter simpleKalmanFilter_T2(Uncertainty, Uncertainty, Process_Variance);
SimpleKalmanFilter simpleKalmanFilter_Td(1, 1, 0.0003);
SimpleKalmanFilter simpleKalmanFilter_Td2(0.5, 0.5, 0.0003);

// undefined reference to `_Sensor_MLX90640::mlx90640To'
// undefined reference to `_Sensor_MLX90640::mlx90640_OUT'
static float mlx90640To[768];
static uint8_t mlx90640_OUT[768 + 20];

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_MLX90640 : public _Sensor_BaseClass {
public:
  // IRTherm  *MLX ;
// const byte MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640
#define TA_SHIFT 8 // Default shift for MLX90640 in open air

  paramsMLX90640 mlx90640;

#define ADC_VBatt_Pin 33
  adc1_channel_t ADC_VBatt = ADC1_CHANNEL_5; // GPIO33

#define Knop_Pin 15
#define Led_Pin 13
#define Spot_N 10

  int Count = 0;
  float emissivity = 0.95;
  uint16_t mlx90640Frame[834];
  int status = -1;
  // uint8_t  Gain          = 6 ;
  int RefreshRate = 2;
  // bool     FTP_Transport = false ;

  // ***********************************************************************
  // Creators
  // ***********************************************************************
  _Sensor_MLX90640(int I2C_Address = 0x33) {
    _Debug_Over_Serial = false;
    this->_I2C_Address = I2C_Address;
    Version_Name = "V" + String(Sensor_MLX90640_h) + "   Sensor_MLX90640.h";
    Serial.println("CREATE    " + Version_Name);
    _JSON_Short_Header = "T1\tT2\tTd\tV_Batt\tT1_Raw\tT2_Raw";
  }

  // ***********************************************************************
  // ***********************************************************************
  void setup() {

    // *****************************************
    // *****************************************
    pinMode(ADC_VBatt_Pin, INPUT);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_VBatt, ADC_ATTEN_DB_6);

    // ************************************************************************
    // Get device parameters - We only have to do this once
    // ************************************************************************
    int status;

    status = MLX90640_SetRefreshRate(this->_I2C_Address, RefreshRate);
    status = MLX90640_GetRefreshRate(this->_I2C_Address);
    Serial.println("RefreshRate: " + String(status));

    uint16_t eeMLX90640[832];
    status = MLX90640_DumpEE(this->_I2C_Address, eeMLX90640);
    if (status != 0) {
      Serial.println("Failed to load system parameters");
    }
    status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    if (status != 0) {
      Serial.println("Parameter extraction failed");
    }

    // Once params are extracted, we can release eeMLX90640 array
    Serial.println("Setup Done");
  }

  // ***********************************************************************
  // De eerste 20 msec is de sensor niet aanspreekbaar
  // ***********************************************************************
  void loop() {
    // ***************************************************
    // Get data and calculate the picture
    // ***************************************************
    bool Frame_Available = MLX90640_GetFrameData_Status(this->_I2C_Address, mlx90640Frame);

    // ***************************************************
    // if data ok, write data to USB-port
    // als nodig bewaar Photo and zend deze over MQTT
    // ***************************************************
    if (Frame_Available) {
      // unsigned long _Single_Frame = millis();
      for (byte x = 0; x < 1; x++) { // Read SINGLE subpage
        status = MLX90640_GetFrameData(this->_I2C_Address, mlx90640Frame);
        if (status >= 0) {
          float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
          float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
          float tr = Ta - TA_SHIFT; // Reflected temperature based on the sensor ambient temperature
          MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
        }
        // Debugf ( "Single Frame %i\n", (millis() - _Single_Frame) ) ;
      }
      if (status >= 0) {

        float Max = 0;
        for (int x = 0; x < 768; x++) {
          if (mlx90640To[x] > Max) {
            Max = mlx90640To[x];
          }
        }
        int Gain = 254.0 / Max;
        if (Gain < 1) {
          Gain = 1;
        }
        for (int x = 0; x < 768; x++) {
          mlx90640_OUT[x] = 1 + (uint8_t)round(Gain * mlx90640To[x]);
        }
        int N_2 = (int)floor(Spot_N / 2);
        int T_Divide = Spot_N * Spot_N / 5;
        int Opp = Spot_N * Spot_N;

        Spot_T1 = 0;
        // int x1 = 9 * 32 + 6 ;
        int x1 = (12 - N_2) * 32 + 8 - N_2;
        for (int row = 0; row < Spot_N; row++) {
          for (int col = 0; col < Spot_N; col++) {
            Spot_T1 += mlx90640To[x1 + row * 32 + col];
          }
        }

        Spot_T2 = 0;
        // int x2 = 10 * 32 - 11 ;
        int x2 = (12 - N_2) * 32 + 24 - N_2;
        for (int row = 0; row < Spot_N; row++) {
          for (int col = 0; col < Spot_N; col++) {
            Spot_T2 += mlx90640To[x2 + row * 32 + col];
          }
        }

        Spot_Td = 128 + (Spot_T1 - Spot_T2);

        this->_Kalman_T1 = simpleKalmanFilter_T1.updateEstimate(Spot_T1);
        this->_Kalman_T2 = simpleKalmanFilter_T2.updateEstimate(Spot_T2);
        this->_Kalman_Td = simpleKalmanFilter_Td.updateEstimate(Spot_T1 - Spot_T2);
        this->_Kalman_Td2 = simpleKalmanFilter_Td2.updateEstimate(Spot_T1 - Spot_T2);

        int V_Battery = adc1_get_raw((adc1_channel_t)ADC_VBatt);
        //          byte Start_Bytes [12] = { 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ;
        Start_Bytes[2] = Gain;
        Start_Bytes[3] = (byte)round(V_Battery / 20);

        int Temp;
        Temp = round(_Kalman_T1);
        Start_Bytes[4] = (byte)(Temp / 100);
        Start_Bytes[5] = (byte)(Temp % 100);
        Temp = round(_Kalman_T2);
        Start_Bytes[6] = (byte)(Temp / 100);
        Start_Bytes[7] = (byte)(Temp % 100);

        if (false) {
          Serial.write(Start_Bytes, 12);
          Serial.write(mlx90640_OUT, 768);
        }
      }
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  int Save_and_MQTT() { //} String Filename ) {
    if (this->_Last_File_Nr < 0) {
      this->_Last_File_Nr = File_System.Get_Last_File_Number();
    }

    Start_Bytes[11] = 0x00; // string terminator
    for (int i = 0; i < 10; i++) {
      mlx90640_OUT[768 + i] = Start_Bytes[i + 2];
    }

    String Filename = "/MLX_" + String(_Last_File_Nr) + ".bin";
    Serial.println("New File = " + Filename);
    File_System.Store_File(Filename, (char *)mlx90640_OUT);

    My_MQTT_Client->Publish((MQTT_Topic + '_').c_str(), (char *)mlx90640_OUT);

    _Last_File_Nr += 1;
    return _Last_File_Nr - 1;
  }

  // ***********************************************************************
  // ***********************************************************************
  float Read_T1() {
    int Opp = Spot_N * Spot_N;
    _T2_at_Read_T1 = this->_Kalman_T2 / Opp;
    return this->_Kalman_T1 / Opp;
  }

  // ***********************************************************************
  // ***********************************************************************
  float Read_T2() { return _T2_at_Read_T1; }

  // ***********************************************************************
  // Get all the sampled data as a JSON string
  // ***********************************************************************
  void Get_JSON_Data() {
    int Opp = Spot_N * Spot_N;
    int V_Battery = adc1_get_raw((adc1_channel_t)ADC_VBatt);

    JSON_Data += "  \"T1\":";
    JSON_Data += _Kalman_T1 / Opp;
    JSON_Data += ",  \"T2\":";
    JSON_Data += _Kalman_T2 / Opp;
    JSON_Data += ",  \"Td\":";
    JSON_Data += _Kalman_Td / Opp;

    JSON_Data += ",  \"T1_Raw\":";
    JSON_Data += Spot_T1 / Opp;
    JSON_Data += ",  \"T2_Raw\":";
    JSON_Data += Spot_T2 / Opp;

    JSON_Data += ",  \"VBatt\":";
    JSON_Data += V_Battery;
    JSON_Data += ",";

    JSON_Short_Data += String(_Kalman_T1 / Opp, 2);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(_Kalman_T2 / Opp, 2);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(_Kalman_Td / Opp, 2);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(V_Battery);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(Spot_T1 / Opp, 2);
    JSON_Short_Data += "\t";
    JSON_Short_Data += String(Spot_T2 / Opp, 2);
    JSON_Short_Data += "\t";
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  float _Kalman_T1;
  float _Kalman_T2;
  float _Kalman_Td;
  float _Kalman_Td2;
  float _T2_at_Read_T1;
  int _Last_File_Nr = -1;
  int _I2C_Address;

  float Spot_T1 = 0;
  float Spot_T2 = 0;
  float Spot_Td = 0;

  unsigned long _Temp_Timer = 0;

  byte Start_Bytes[12] = {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};
#endif
