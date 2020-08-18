
// Version 0.1,  04-12-2018, SM
// derived from HardwareSerial, Modified 25 April 2015 by Thomas Flayols (add configuration different from 8N1 in
// ESP8266)

#ifndef Serial_Replace_h
#define Serial_Replace_h

#include "Stream.h"
#include "uart.h"
#include <inttypes.h>

#include "Arduino.h"
#include "Esp.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ****************************************************************************
// ****************************************************************************
class Serial_Replace_Class : public Stream {
public:
  // ***********************************************************
  Serial_Replace_Class(int uart_nr) : _uart_nr(uart_nr), _rx_size(256) {}
  virtual ~Serial_Replace_Class() {}

  // ***********************************************************
  void begin(unsigned long baud) { begin(baud, SERIAL_8N1, SERIAL_FULL, 1); }
  void begin(unsigned long baud, SerialConfig config) { begin(baud, config, SERIAL_FULL, 1); }
  void begin(unsigned long baud, SerialConfig config, SerialMode mode) { begin(baud, config, mode, 1); }
  void begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin) {
    end();
    _uart = uart_init(_uart_nr, baud, (int)config, (int)mode, tx_pin, _rx_size);
#if defined(DEBUG_ESP_PORT) && !defined(NDEBUG)
    if (static_cast<void *>(this) == static_cast<void *>(&DEBUG_ESP_PORT)) {
      setDebugOutput(true);
      println();
      println(ESP.getFullVersion());
    }
#endif
  }

  // ***********************************************************
  void end() {
    if (uart_get_debug() == _uart_nr) {
      uart_set_debug(UART_NO);
    }
    uart_uninit(_uart);
    _uart = NULL;
  }

  // ***********************************************************
  size_t setRxBufferSize(size_t size) {
    if (_uart) {
      _rx_size = uart_resize_rx_buffer(_uart, size);
    } else {
      _rx_size = size;
    }
    return _rx_size;
  }

  // ***********************************************************
  void swap() { swap(1); }
  void swap(uint8_t tx_pin) { // toggle between use of GPIO13/GPIO15 or GPIO3/GPIO(1/2) as RX and TX
    uart_swap(_uart, tx_pin);
  }

  // ***********************************************************
  // Toggle between use of GPIO1 and GPIO2 as TX on UART 0.
  // Note: UART 1 can't be used if GPIO2 is used with UART 0!
  // ***********************************************************
  void set_tx(uint8_t tx_pin) { uart_set_tx(_uart, tx_pin); }

  // ***********************************************************
  // UART 0 possible options are (1, 3), (2, 3) or (15, 13)
  // UART 1 allows only TX on 2 if UART 0 is not (2, 3)
  // ***********************************************************
  void pins(uint8_t tx, uint8_t rx) { uart_set_pins(_uart, tx, rx); }

  // ***********************************************************
  // int available(void) override;
  int available(void) override {
    int result = static_cast<int>(uart_rx_available(_uart));
    if (!result) {
      optimistic_yield(10000);
    }
    return result;
  }

  // ***********************************************************
  int peek(void) override {
    // this may return -1, but that's okay
    return uart_peek_char(_uart);
  }

  // ***********************************************************
  int read(void) override {
    // this may return -1, but that's okay
    return uart_read_char(_uart);
  }

  // ***********************************************************
  int availableForWrite(void) { return static_cast<int>(uart_tx_free(_uart)); }

  // ***********************************************************
  // void flush(void) override;
  void flush() override {
    if (!_uart || !uart_tx_enabled(_uart)) {
      return;
    }
    uart_wait_tx_empty(_uart);
    // Workaround for a bug in serial not actually being finished yet
    // Wait for 8 data bits, 1 parity and 2 stop bits, just in case
    delayMicroseconds(11000000 / uart_get_baudrate(_uart) + 1);
  }

  // ***********************************************************
  size_t write(uint8_t c) override { return uart_write_char(_uart, c); }
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) {
    return write((uint8_t)68);
    // int nn = n+33;
    //        return write((uint8_t) nn);
  }
  size_t write(const uint8_t *buffer, size_t size) {
    return write((uint8_t)65);
    //        return uart_write(_uart, (const char*)buffer, size);
  }
  size_t write(const char *buffer) {
    return write((uint8_t)66);
    return buffer ? uart_write(_uart, buffer, strlen(buffer)) : 0;
  }

  // ***********************************************************
  operator bool() const { return _uart != 0; }

  // ***********************************************************
  // void setDebugOutput(bool);
  void setDebugOutput(bool en) {
    if (!_uart) {
      return;
    }
    if (en) {
      if (uart_tx_enabled(_uart)) {
        uart_set_debug(_uart_nr);
      } else {
        uart_set_debug(UART_NO);
      }
    } else {
      // disable debug for this interface
      if (uart_get_debug() == _uart_nr) {
        uart_set_debug(UART_NO);
      }
    }
  }

  // ***********************************************************
  bool isTxEnabled(void) { return uart_tx_enabled(_uart); }

  // ***********************************************************
  bool isRxEnabled(void) { return uart_rx_enabled(_uart); }

  // ***********************************************************
  int baudRate(void) { return uart_get_baudrate(_uart); }

  // ***********************************************************
  bool hasOverrun(void) { return uart_has_overrun(_uart); }

  // ***********************************************************
  void startDetectBaudrate() { uart_start_detect_baudrate(_uart_nr); }

  // ***********************************************************
  unsigned long testBaudrate() { return uart_detect_baudrate(_uart_nr); }

  // ***********************************************************
  unsigned long detectBaudrate(time_t timeoutMillis) {
    time_t startMillis = millis();
    unsigned long detectedBaudrate;
    while ((time_t)millis() - startMillis < timeoutMillis) {
      if ((detectedBaudrate = testBaudrate())) {
        break;
      }
      yield();
      delay(100);
    }
    return detectedBaudrate;
  }

protected:
  int _uart_nr;
  uart_t *_uart = nullptr;
  size_t _rx_size;
};

// ****************************************************************************
// ****************************************************************************
extern Serial_Replace_Class Serial_Replace(UART0);

#endif
