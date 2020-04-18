
// Version 0.2, 20-12-2019, SM
//
// Version 0.1, 12-12-2019, SM

#ifndef Sensor_MPU9250_h
#define Sensor_MPU9250_h    0.2

#include "Sensor_Base.h"
#include "MPU9250.h"

/*
Geen super bibliotheek, ook niet alles werkt.

https://www.invensense.com/products/motion-tracking/9-axis/

MPU92/65 (=GY87?) bestaat uit
- mpu6500 (ongeveer zoiets als een mpu6050)
- mpu9255 (ongeveer zoiets als een mpu6055)
- mpu9250 = mpu 6500 + AK8963
- mpu6555

I2C address = 0x68
*/


// ***********************************************************************************
// ***********************************************************************************
class _Sensor_MPU9250 : public _Sensor_BaseClass {

  public:
    float  MPU9250_Acc_X ;
    float  MPU9250_Acc_Y ;
    float  MPU9250_Acc_Z ;
   
    // ***********************************************************************
    // Creator, 
    // ***********************************************************************
    _Sensor_MPU9250 ( int Display_X = -1, int Display_Y = -1 ) {
      this -> _Display_X = Display_X ;
      this -> _Display_Y = Display_Y ;
      this -> Default_Settings () ;
      this -> Constructor_Finish () ;
    }  
      

    // ***********************************************************************
    // ***********************************************************************
    void Constructor_Finish () {
      Version_Name = "V" + String ( Sensor_MPU9250_h ) + "   Sensor_MPU9250.h" ;
      Serial.println ( "CREATE    " + Version_Name ) ;
      _JSON_Short_Header = "Acc_X\tAcc_Y\tAcc_Z\t" ;
    }

    // ***********************************************************************
    // ***********************************************************************
    void setup () {
      //delay ( 2000 ) ;
      _MPU.setup () ;
      Print_Help () ;      
    }

    // **********************************************************************************************
    // **********************************************************************************************
    void Default_Settings ( bool Force = false  ) {
      //_I2CAddress      = Settings.Get_Set_Default_Int  ( "BME280 I2C Address"      , 0x76, Force ) ;
    }

    // **********************************************************************************************
    bool Check_Modified_Settings () {
      return false ;
    }

    // ***********************************************************************
    // Name  = The Name that will be printed first
    //     if Value >= 0, an extra space will be printed after the Name 
    // Value = The floating point value to be printed  
    // X, Y  = Starting position on teh screen (in character coordinates)
    // ***********************************************************************
    void Disp_Number ( String Name, float Value, int Decimal, int X, int Y ) {
      _Character_Display->Show ( Name, X, Y  ) ;
      
      if ( Value < 0 ) {
        _Character_Display->Show ( String ( Value, Decimal ) ) ;
      } else {
        _Character_Display->Show ( " " ) ;
        _Character_Display->Show ( String ( Value, Decimal ) ) ;
      }
    }
    

    // ***********************************************************************
    // ***********************************************************************
    void loop () {
      if ( ( this->_Display_X >= 0 ) && ( this-> _Display_Y>=0 ) ) {
        if ( millis() - _Sample_Time_Last >= 200 ) {
          _MPU.update () ;
          Disp_Number ( "X", _MPU.getAcc(0), 1, this->_Display_X, this->_Display_Y     ) ;
          Disp_Number ( "Y", _MPU.getAcc(1), 1, this->_Display_X, this->_Display_Y + 1 ) ;
          Disp_Number ( "Z", _MPU.getAcc(2), 1, this->_Display_X, this->_Display_Y + 2 ) ;
          
        }
      }
    }
    
    // ***********************************************************************
    // ***********************************************************************
    void Print_Help () {
      Serial.println ( "_Sensor_MPU9250 ( 0x" ) ;
    }

    // ***********************************************************************
    // Get all the sampled data as a JSON string
    // ***********************************************************************
    void Get_JSON_Data () {
      this -> Get_Accelerations () ;

      JSON_Data += " \"Acc_X\":" ;
      JSON_Data += String ( MPU9250_Acc_X, 1 ) ;
      JSON_Data += ", \"Acc_Y\":" ;
      JSON_Data += String ( MPU9250_Acc_Y, 1 ) ;
      JSON_Data += ", \"Acc_Z\":" ;
      JSON_Data += String ( MPU9250_Acc_Z, 1 ) ;
      JSON_Data += "," ;

      JSON_Short_Data += String ( MPU9250_Acc_X, 1 ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( MPU9250_Acc_Y, 1 ) ;
      JSON_Short_Data += "\t" ;
      JSON_Short_Data += String ( MPU9250_Acc_Z, 1 ) ;
      JSON_Short_Data += "\t" ;
    } 


    // ***********************************************************************
    // ***********************************************************************
    void Get_Accelerations () {
      _MPU.update () ;
      MPU9250_Acc_X = _MPU.getAcc (0) ;
      MPU9250_Acc_Y = _MPU.getAcc (1) ;
      MPU9250_Acc_Z = _MPU.getAcc (2) ;
    }

  // ***********************************************************************
  private:
  // ***********************************************************************
    MPU9250 _MPU ;
    //int _I2CAddress ;
    unsigned long _Sample_Time_Last = 0 ;
    int           _Display_X ;
    int           _Display_Y ;

};

#endif
