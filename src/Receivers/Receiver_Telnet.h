// Version 0.1    10-04-2020, SM

#ifndef Receiver_Telnet_h
#define Receiver_Telnet_h 0.1

#include "Receiver_Base.h"

#ifdef ESP8266
  #include <ESP8266WiFi.h>
#elif defined ESP32
  #include <WiFi.h>
#endif

// ***********************************************************************************
// ***********************************************************************************
// Define a new Serial port, which we will access explicitly with Serial_Device
// HardwareSerial Serial_Device ( 0 ) ;
// Define the Telnet Server and Client
WiFiServer Telnet_Server(23);
WiFiClient Telnet_Client;

// ***********************************************************************************
// ***********************************************************************************
class _Receiver_Telnet : public _Receiver_BaseClass {

public:
  // ***********************************************************************
  // Creator,
  // ***********************************************************************
  _Receiver_Telnet() {
    Version_Name = "V" + String(Receiver_Telnet_h) + "   Receiver_Telnet.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // ***********************************************************************
  // Allowed after a wifi connection is established
  // ***********************************************************************
  void setup() {
    Telnet_Server.begin();
    Telnet_Server.setNoDelay(true);
    delay(1000);
  }

  // ***********************************************************************
  // only one client is supported
  // ***********************************************************************
  void loop() {
    // check if there are any new clients
    if (Telnet_Server.hasClient()) {
      if (!Telnet_Client.connected()) {
        Telnet_Client = Telnet_Server.available();
        // first character is rubish
        delay(300);
        while (Telnet_Client.available()) {
          Telnet_Client.read();
        }
      }
    }
  }

  // ***********************************************************************
  // ***********************************************************************
  virtual bool Send_Data(String JSON_Message) { return true; }
};

// ***********************************************************************************
// The class that can dynamically switch between UART and TelNet
// and the flag that controls this
// and immediately create an instance
// and a define to replace "Serial" with this instance during compile time
// ***********************************************************************************
bool MySerial_2_Telnet = false;
class Class_MySerial : public Stream {
public:
  // **************************************
  Class_MySerial() { Serial_Device.begin(115200); }

  // **************************************
  // **************************************
  void begin(int Baudrate = 115200) { Serial_Device.begin(Baudrate); }

  // **************************************
  // needed for ESP32
  // **************************************
  void flush() {}

  // **************************************
  // by implemneting the "write" method all print and println variants will be supported
  // **************************************
  size_t write(uint8_t val) {
    Serial_Device.write(val);
    Telnet_Client.write(val);
    return 1;
  }
  // **************************************
  size_t write(const uint8_t *buffer, size_t size) {
    Serial_Device.write(buffer, size);
    Telnet_Client.write(buffer, size);
    return size;
  }
  // **************************************

  // **************************************
  // all other methods can be created by the 3 methods above
  // **************************************
  inline size_t write(const char *buffer, size_t size) { return write((uint8_t *)buffer, size); }
  inline size_t write(const char *s) { return write((uint8_t *)s, strlen(s)); }
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }

  // **************************************
  int available(void) {
    int N_Char = 0;
    _Read_From_Serial = false;
    N_Char = Telnet_Client.available();
    if (Serial_Device.available() >= N_Char) {
      N_Char = Serial_Device.available();
      this->_Read_From_Serial = true;
    }
    return N_Char;
  }

  // **************************************
  int read(void) {
    if (available()) {
      if (this->_Read_From_Serial) return Serial_Device.read();
      else
        return Telnet_Client.read();
    }
    return -1;
  }

  // **************************************
  // **************************************
  int peek(void) { return -1; }

  // ****************************************************************
private:
  bool _Read_From_Serial = true;
};
// **************************************
Class_MySerial My_Serial;
#define Serial My_Serial

#endif
