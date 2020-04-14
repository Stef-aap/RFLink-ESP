#ifndef Quirks_h
#define Quirks_h


// ***********************************************************************************
// it's not allowed to use both libraries
// The reason is that both libraries use
//    #ifndef __BME280_H__
//    #define __BME280_H__
// which is an ERROR in both libraries
// ***********************************************************************************
#ifndef NOT_INCLUDE_BME280
  #include "Sensor_BME280_AF.h"   
  //#include "Sensor_BME280_SF.h"
#endif

#endif