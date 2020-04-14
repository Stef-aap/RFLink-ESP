# RFLink-ESP V3.0

This is a fork of RFLink.
The previous versions (2.1 ... 2.4) have been proven to be stable, running 24/7 for over 2 years without any problem.
Version V3.0 is a complete rebuild, based on my standard Sensor/Receiver libraries, which is easier to maintain and opens the possibility to easy add new features.
I believe that one or more of the forks has a better RF detection.
V3.0 of RFLink-ESP has the following new or improved features
- complete rebuild on the base of Sensors/Receivers-concept (more features with less effort)
- simultanuous support of serial and telnet debug and control
- OTA suport
- Webserver for settings and download of files
- received and transmitted messages are stored in a file
- flag Home_Automation removed
- command added:  19;PRINT;    //lists the known devices
- Learning_Mode=9 is made asynchronuous, so you can do other things including stopping LM-9
- command added:  13; and 14;   // shows/sets 2 commands for Learning_Mode 8/9

# Schmurtz
forked V2.1 and made it more compatible with other Bridge-libraries. https://github.com/schmurtzm/RFLink-ESP

# RFLink-ESP V2.1
Home Assistant / Domoticz tested with a RFLink, modified for ESP8266 and ESP32

This is a fork of RFLink, and because we couldn't get it working reliable, we ended up in a complete rewrite of RFLink.
Problem is that the latest version of RFLink is R48. We couldn't only find sources of version R29 and R35. Both sources didn't work correctly, R29 was the best. We tried to contact "the stuntteam" which owns the orginal sources but no response.

This version of RFLink-ESP has the following features
- just a few protocols are translated and tested
- Protocols are more generic, so you need less protocols
- Protocols are written as classes and all derived from a common class
- Protocols can easily be selected and the order can be determined
- Debugmode replaced with a more flexible Learning_Mode
- Removed a lot of redundancies
- Device must be registered before they will be recognized ( (almost) no more false postives)
- Dynamically determine long/short puls, by measuring Min,Max,Mean
- Runs on ESP32 and even on a ESP8266 
- Fully open source
- Codesize is strongly reduced
- Can communicate over USB/RS232 or MQTT

Some ideas for the future
- Implementing rolling code SomFy / Own
- Cleanup global constants and variables
- Using SI4432 as Receiver / Transmitter (Transmitter can be used for all frequencies, Receiver might be able to fetch a complete sequence, so listening at more frequencies at the same time might even be possible)
- Logging of false positives, including time (Neighbours ??)  NTP: https://www.instructables.com/id/Arduino-Internet-Time-Client/
