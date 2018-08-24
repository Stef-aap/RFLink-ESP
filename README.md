# Domoticz-RFLink-ESP
Domoticz tested with a RFLink, modified for ESP8266 and ESP32

This is a fork of RFLink, and because we couldn't get it working reliable, we ended up in a complete rewrite of RFLink.
Problem is that the latest version of RFLink is R48. We couldn't only find sources of version R29 and R35. Both sources didn't work correctly, R29 was the best. We tried to contact "the stuntteam" which owns the orginal sources but no response.

This version of RFLink has the following features
- just a few protocols are translated and tested
- Protocols are more generic, so you need less protocols
- Protocols are written as classes and all derived from a common class
- Protocols can easily be selected and the order can be determined
- Debugmode replaced with a more flexible Learning_Mode
- Removed a lot of redundancies
- Device must be registered before they will be recognized ( (almost) no more false postives)
- Dynamically determine long/short puls, by measuring Min,Max,Mean
- Runs on ESP32 and even on a ESP8266 
- Fully open source, see GitHub: https://github.com/Stef-aap/Domoticz-RFLink-ESP
- Codesize is strongly reduced

Some ideas for the future
- Implementing rolling code SomFy / Own
- Cleanup global constants and variables
- Using SI4432 as Receiver / Transmitter (Transmitter can be used for all frequencies, Receiver might be able to fetch a complete sequence, so listening at more frequencies at the same time might even be possible)
- Logging of false positives, including time (Neighbours ??)  NTP: https://www.instructables.com/id/Arduino-Internet-Time-Client/
