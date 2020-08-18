// ****************************************************************************
// Deze library
//   - De swapped serial hardware port kunnen gebruiken voor hardware devices
//   - en toch Serial (via software) kunnen gebruiken voor debugging
//
// Serial uses UART0, which is mapped to pins GPIO1 (TX) and GPIO3 (RX).
//   Serial may be remapped to GPIO15 (TX) and GPIO13 (RX)
//   by calling Serial.swap() after Serial.begin.
//   Callindg swap again maps UART0 back to GPIO1 and GPIO3.
//
// Version 0.1   08-03-2019, SM
//    - initial version
// ****************************************************************************

#ifndef Redirect_Serial_h
#define Redirect_Serial_h 0.1

// Serial uses UART0, which is mapped to pins GPIO1 (TX) and GPIO3 (RX).
//   Serial may be remapped to GPIO15 (TX) and GPIO13 (RX)
//   by calling Serial.swap() after Serial.begin.
//   Calling swap again maps UART0 back to GPIO1 and GPIO3.

// ***************************************************
// Hier wordt een software serial port gedefinieerd op
//   de oorspronkelijke hardware UART 0
// ***************************************************
#include <SoftwareSerial.h>
int _Redirected_RX = 03; // 13
int _Redirected_TX = 01; // 15
const bool _Redirected_Inverted = false;
SoftwareSerial Serial_Software(_Redirected_RX, _Redirected_TX, _Redirected_Inverted);

// ***************************************************
// Omdat we door de redirect "Serial" kwijt raken
// moeten we een nieuwe HardwareSerial aanmaken
// ***************************************************
HardwareSerial Serial_Device(UART0);

// ****************************************************************************
// ****************************************************************************
void SoftSerial_Setup(int Baudrate_Device, int Baudrate_Soft) {
  // ***************************************************
  // Start de orginele hardware port en swap deze naar RX=13 en TX=15
  // ***************************************************
  // Serial.begin ( 115200 ) ;
  // Serial.swap () ;

  // ***************************************************
  // Start een nieuwe SerialHardware en swap deze
  // ***************************************************
  Serial_Device.begin(Baudrate_Device);
  Serial_Device.swap();

  // ***************************************************
  // Het zetten van de pinmode is noodzakelijk !!
  // ***************************************************
  Serial_Software.begin(Baudrate_Soft);
  pinMode(_Redirected_RX, INPUT);
  pinMode(_Redirected_TX, OUTPUT);

// Serial_Soft.swap () ;  // Geeft compiler fout, DUS PRIMA !!

// ***************************************************
// Hier wordt "Serial" geredirect
// ***************************************************
#define Serial Serial_Software

  Serial.println("\nV" + String(Redirect_Serial_h) + "   Redirect_Serial.h");
  Serial.println("    Device   Baudrate = " + String(Baudrate_Device) + "    RX=GPIO13,  TX=GPIO15");
  Serial.println("    Software Baudrate = " + String(Baudrate_Soft) + "    RX=GPIO3 ,  TX=GPIO1");
}
#endif
