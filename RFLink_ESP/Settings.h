{
  "RS232 Baudrate" : 115200, "Sample Period[ms]" : 10000, "Signal LED" : -1,
                                                                         "Sensor_Wifi" : true,
                                                                                         "Wifi-Netwerk"
      : "<HomeNetwork SSID>",
        "$Wifi-Password" : "<Wifi Password>",
                           "Receiver_Telnet" : true,
                                               "Receiver_SPIFFS" : true,
                                                                   "SPIFFS Filename" : "/Data.csv",
                                                                                       "SPIFFS File Seconds" : 3600,
                                                                                       "SPIFFS Max N Files" : 24,
                                                                                       "Receiver_Webserver"
      : true,
        "Sensor_Watchdog" : true,
                            "Sensor_Dummy" : false,
                                             "Sensor_I2C_Scan" : false,
                                                                 "Sensor_Noise_100" : false,
                                                                                      "Sensor_System" : true,
                                                                                                        "Receiver_MQTT"
      : true,
        "Receiver_Serial" : false,
                            "Receiver_OTA" : true,
                                             "Watchdog GPIO" : 23,
                                             "MQTT Topic" : "hb/from_HA/RFLink_ESP",
                                                            "MQTT Broker-IP" : "192.168.0.123",
                                                                               "OTA Hostname" : "RFLink_ESP",
                                                                                                "$OTA Password"
      : "RFLink_ESP",
        "Sensor_RFLink" : true,
                          "RFLink Receive  GPIO" : 12,
                          "RFLink Transmit GPIO" : 14
}
