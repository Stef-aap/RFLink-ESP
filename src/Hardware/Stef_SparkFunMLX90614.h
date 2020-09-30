/******************************************************************************
SparkFunMLX90614.h
Header file for the SparkFun IR Thermometer Library

Jim Lindblom @ SparkFun Electronics
October 23, 2015
https://github.com/sparkfun/SparkFun_MLX90614_Arduino_Library

This file defines the SMBus hardware interface(s) for the MLX90614 IR thermometer
and abstracts temperature measurments and other features of the MLX90614

Development environment specifics:
Arduino 1.6.5
SparkFun IR Thermometer Evaluation Board - MLX90614
******************************************************************************/

#ifndef SPARKFUNMLX90164_H
#define SPARKFUNMLX90164_H

#include <Arduino.h>
#include <Wire.h>

//////////////////////////////////
// MLX90614 Default I2C Address //
//////////////////////////////////
#define MLX90614_DEFAULT_ADDRESS 0x5A

///////////////////////////////////////
// MLX90614 RAM and EEPROM Addresses //
///////////////////////////////////////
#define MLX90614_REGISTER_TA 0x06
#define MLX90614_REGISTER_TOBJ1 0x07
#define MLX90614_REGISTER_TOBJ2 0x08
#define MLX90614_REGISTER_TOMAX 0x20
#define MLX90614_REGISTER_TOMIN 0x21
#define MLX90614_REGISTER_PWMCTRL 0x22
#define MLX90614_REGISTER_TARANGE 0x23
#define MLX90614_REGISTER_KE 0x24
#define MLX90614_REGISTER_CONFIG 0x25
#define MLX90614_REGISTER_ADDRESS 0x2E
#define MLX90614_REGISTER_ID0 0x3C
#define MLX90614_REGISTER_ID1 0x3D
#define MLX90614_REGISTER_ID2 0x3E
#define MLX90614_REGISTER_ID3 0x3F
#define MLX90614_REGISTER_SLEEP 0xFF // Not really a register, but close enough

#define I2C_READ_TIMEOUT 1000

typedef enum { TEMP_RAW, TEMP_K, TEMP_C, TEMP_F } temperature_units;

class IRTherm {
public:
  // Default constructor, does very little besides setting class variable
  // initial values.
  IRTherm();

  // begin(<address>) initializes the Wire library, and prepares
  // communication with an MLX90614 device at the specified 7-bit I2C
  // address.
  // If no parameter is supplied, the default MLX90614 address is used.
  uint8_t begin(uint8_t address = MLX90614_DEFAULT_ADDRESS);

  // setUnit(<unit>) configures the units returned by the ambient(),
  // object(), minimum() and maximum() functions, and it determines what
  // units the setMin() and setMax() functions should expect.
  // <unit> can be either:
  //  - TEMP_RAW: No conversion, just the raw 12-bit ADC reading
  //  - TEMP_K: Kelvin
  //  - TEMP_C: Celsius
  //  - TEMP_F: Farenheit
  void setUnit(temperature_units unit);

  // read() pulls the latest ambient and object temperatures from the
  // MLX90614. It will return either 1 on success or 0 on failure. (Failure
  // can result from either a timed out I2C transmission, or an incorrect
  // checksum value).
  uint8_t read(void);

  // object() returns the MLX90614's most recently read object temperature
  // after the read() function has returned successfully. The float value
  // returned will be in the units specified by setUnit().
  float object(void);

  // ambient() returns the MLX90614's most recently read ambient temperature
  // after the read() function has returned successfully. The float value
  // returned will be in the units specified by setUnit().
  float ambient(void);

  // readEmissivity() reads the MLX90614's emissivity setting. It will
  // return a value between 0.1 and 1.0.
  float readEmissivity(void);

  // setEmissivity(<emis>) can set the MLX90614's configured emissivity
  // EEPROM value.
  // The <emis> parameter should be a value between 0.1 and 1.0.
  // The function will return either 1 on success or 0 on failure.
  uint8_t setEmissivity(float emis);

  int readFIR(void);
  int readIIR(void);
  int readConfig(void);
  int setFIR(uint8_t value);
  int setIIR(uint8_t value);
  int DumpEE(uint16_t *eeData);

  // readAddress() returns the MLX90614's configured 7-bit I2C bus address.
  // A value between 0x01 and 0x7F should be returned.
  uint8_t readAddress(void);

  // setAddress(<newAdd>) can set the MLX90614's 7-bit I2C bus address.
  // The <newAdd> parameter should be a value between 0x01 and 0x7F.
  // The function returns 1 on success and 0 on failure.
  // The new address won't take effect until the device is reset.
  uint8_t setAddress(uint8_t newAdd);

  // readID() reads the 64-bit ID of the MLX90614.
  // Return value is either 1 on success or 0 on failure.
  uint8_t readID(void);

  // After calling readID() getIDH() and getIDL() can be called to read
  // the upper 4 bytes and lower 4-bytes, respectively, of the MLX90614's
  // identification registers.
  uint32_t getIDH(void);
  uint32_t getIDL(void);

  // readRange() pulls the object maximum and minimum values stored in the
  // MLX90614's EEPROM.
  // It will return either 1 on success or 0 on failure.
  uint8_t readRange(void);

  // minimum() and maximum() return the MLX90614's minimum and maximum object
  // sensor readings.
  // The float values returned will be in the units specified by setUnit().
  float minimum(void);
  float maximum(void);

  // setMax(<maxTemp>) and setMin(<minTemp>) configure the MLX90614's
  // maximum and minimum object sensor temperatures.
  uint8_t setMax(float maxTemp);
  uint8_t setMin(float minTemp);

  // sleep() sets the MLX90614 into a low-power sleep mode.
  uint8_t sleep(void);

  // wake() should revive the MLX90614 from low-power sleep mode.
  void wake(void);

private:
  uint8_t _deviceAddress;         // MLX90614's 7-bit I2C address
  temperature_units _defaultUnit; // Keeps track of configured temperature unit

  // These keep track of the raw temperature values read from the sensor:
  int16_t _rawAmbient, _rawObject, _rawObject2, _rawMax, _rawMin;

  uint16_t id[4]; // Keeps track of the 64-bit ID value

  // These functions individually read the object, object2, and ambient
  // temperature values from the MLX90614's RAM:
  uint8_t readObject(void);
  uint8_t readObject2(void);
  uint8_t readAmbient(void);

  // These functions individually read the min and mx temperatures in
  // the MLX90614's EEPROM:
  uint8_t readMax(void);
  uint8_t readMin(void);

  // calcTemperature converts a raw ADC temperature reading to the
  // set unit.
  float calcTemperature(int16_t rawTemp);

  // calcRawTemperature converts a set unit temperature to a
  // raw ADC value:
  int16_t calcRawTemp(float calcTemp);

  // Abstract function to write 16-bits to an address in the MLX90614's
  // EEPROM
  uint8_t writeEEPROM(byte reg, int16_t data);

  // Abstract functions to read and write 16-bit values from a RAM
  // or EEPROM address in the MLX90614
  uint8_t I2CReadWord(byte reg, int16_t *dest);
  uint8_t I2CWriteWord(byte reg, int16_t data);

  // crc8 returns a calculated crc value given an initial value and
  // input data.
  // It's configured to calculate the CRC using a x^8+x^2+x^1+1 poly
  uint8_t crc8(uint8_t inCrc, uint8_t inData);
};

#endif
