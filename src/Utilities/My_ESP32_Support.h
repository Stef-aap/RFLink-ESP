

#ifndef My_ESP32_Support_h
#define My_ESP32_Support_h

uint32_t GetChipID() {
  uint64_t ChipID;
  ChipID = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
                              // Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
                              // Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  return (uint32_t)ChipID;
}

#endif