/*
* Project Name: TM1638 
* File: TM1638plus.cpp
* Description: source file arduino  library for TM1638 module(LED & KEY).
* Author: Gavin Lyons.
* Created May 2019
* URL: https://github.com/gavinlyonsrepo/TM1638plus
*/

#include "TM1638plus.h"


uint8_t shiftInX(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t i;

    for(i = 0; i < 8; ++i) {
        //digitalWrite(clockPin, HIGH);
        if(bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, HIGH);
delayMicroseconds(1);
        digitalWrite(clockPin, LOW);
delayMicroseconds(1);
    }
//Serial.println ( "*********************");
    return value;
}


TM1638plus::TM1638plus(uint8_t strobe, uint8_t clock, uint8_t data) {
  _STROBE_IO = strobe;
  _DATA_IO = data;
  _CLOCK_IO = clock;
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  sendCommand(ACTIVATE_TM);
  brightness(DEFAULT_BRIGHTNESS);
  reset();
}

void TM1638plus::sendCommand(uint8_t value)
{
  digitalWrite(_STROBE_IO, LOW);
  shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, value);
  digitalWrite(_STROBE_IO, HIGH);
}

void TM1638plus::reset() {
  sendCommand(WRITE_INC); // set auto increment mode
  digitalWrite(_STROBE_IO, LOW);
  shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, SEG_ADR);   // set starting address to 0
  for (uint8_t i = 0; i < 16; i++)
  {
    shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, 0x00);
  }
   digitalWrite(_STROBE_IO, HIGH);
}

void TM1638plus::setLED(uint8_t position, uint8_t value)
{
  pinMode(_DATA_IO, OUTPUT);
  sendCommand(WRITE_LOC);
  digitalWrite(_STROBE_IO, LOW);
  shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, LEDS_ADR + (position << 1));
  shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, value);
  digitalWrite(_STROBE_IO, HIGH);
}

void TM1638plus::displayText(const char *text) {
  char c, pos;

  pos = 0;
  while (c = (*text++)) {
    if (*text == '.') {
      displayASCIIwDot(pos++, c);

      text++;
    }  else {
      displayASCII(pos++, c);
    }
  }
}


void TM1638plus::displayASCIIwDot(uint8_t position, uint8_t ascii) { 
    // add 128 or 0x080 0b1000000 to turn on decimal point/dot in seven seg
  display7Seg(position, pgm_read_byte(&SevenSeg[ascii- ASCII_OFFSET]) + DOT_MASK_DEC);
}

void TM1638plus::display7Seg(uint8_t position, uint8_t value) { // call 7-segment
  sendCommand(WRITE_LOC);
  digitalWrite(_STROBE_IO, LOW);
  shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, SEG_ADR + (position << 1));
  shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, value);
  digitalWrite(_STROBE_IO, HIGH); 
}


void TM1638plus::displayASCII(uint8_t position, uint8_t ascii) {
  display7Seg(position, pgm_read_byte(&SevenSeg[ascii - ASCII_OFFSET]));
}
 
void TM1638plus::displayHex(uint8_t position, uint8_t hex) 
{
    uint8_t offset = 0;
    if ((hex >= 0) && (hex <= 9))
    {
        display7Seg(position, pgm_read_byte(&SevenSeg[hex + HEX_OFFSET]));
        // 16 is offset in reduced ASCII table for 0 
    }else if ((hex >= 10) && (hex <=15))
    {
        // Calculate offset in reduced ASCII table for AbCDeF
        switch(hex) 
        {
         case 10: offset = 'A'; break;
         case 11: offset = 'b'; break;
         case 12: offset = 'C'; break;
         case 13: offset = 'd'; break;
         case 14: offset = 'E'; break;
         case 15: offset = 'F'; break;
        }
        display7Seg(position, pgm_read_byte(&SevenSeg[offset-ASCII_OFFSET]));
    }
    
}




uint8_t TM1638plus::readButtons()
{
  uint8_t buttons = 0;
digitalWrite(_CLOCK_IO, LOW);  //SM
  digitalWrite(_STROBE_IO, LOW);
delayMicroseconds(1);
  shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, BUTTONS_MODE);
  pinMode(_DATA_IO, INPUT);
  
  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftInX(_DATA_IO, _CLOCK_IO, LSBFIRST) << i;
    buttons |= v;
  }

  pinMode(_DATA_IO, OUTPUT);
  digitalWrite(_STROBE_IO, HIGH); 
  return buttons;
}


void TM1638plus::brightness(uint8_t brightness)
{
    uint8_t  value = 0;
    value = BRIGHT_ADR + (BRIGHT_MASK & brightness);
    digitalWrite(_STROBE_IO, LOW); 
    shiftOut(_DATA_IO, _CLOCK_IO, LSBFIRST, value);
     digitalWrite(_STROBE_IO, HIGH); 
}
