// Version 0.3  20-12-07-2019, SM
//
// Version 0.2  07-12-07-2019, SM
//    - After I2C scan clock reset to original value
//
// Version 0.1  26-07-2019, SM, original release

#ifndef Sensor_I2C_Scan_h
#define Sensor_I2C_Scan_h 0.3

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_I2C_Scan : public _Sensor_BaseClass {
public:
  // **********************************************************************************************
  // **********************************************************************************************
  _Sensor_I2C_Scan(String Dummy) {
    this->Default_Settings();
    this->Constructor_Finish();
  }
  _Sensor_I2C_Scan() { this->Constructor_Finish(); }
  _Sensor_I2C_Scan(int SDA, int SCL) {
    Wire_SDA = SDA;
    Wire_SCL = SCL;
    this->Constructor_Finish();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Constructor_Finish() {
    Version_Name = "V" + String(Sensor_I2C_Scan_h) + "  ──────  Sensor_I2C_Scan.h";
    Serial.println("\n──────  CREATE  ──────  " + Version_Name);
    Help_Text = "";
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void setup() { Wire_Begin(Wire_SDA, Wire_SCL); }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) {
    Wire_SDA = Settings.Get_Set_Default_Int("I2C Data  GPIO", 17, Force);
    Wire_SCL = Settings.Get_Set_Default_Int("I2C Clock GPIO", 5, Force);
    Wire_Begin(Wire_SDA, Wire_SCL);
  }

  // **********************************************************************************************
  bool Check_Modified_Settings() {
    bool Restart = false;
    int New_Value_Int;

    int I2C_Data = Settings.Read_Int("I2C Data  GPIO");
    int I2C_Clock = Settings.Read_Int("I2C Clock GPIO");

    Serial.printf("2222222222222222222222    I2C settings   %i  // %i \n", I2C_Data, I2C_Clock);

    for (int i = 0; i < My_Webserver.args(); i++) {
      New_Value_Int = My_Webserver.arg(i).toInt();

      if (My_Webserver.argName(i) == "I2C Data  GPIO") {
        if (New_Value_Int != I2C_Data) {
          _My_Settings_Buffer["I2C Data  GPIO"] = New_Value_Int;
          Wire_SDA = New_Value_Int;
          Restart = true;
        }
      } else if (My_Webserver.argName(i) == "I2C Clock GPIO") {
        if (New_Value_Int != I2C_Clock) {
          _My_Settings_Buffer["I2C Clock GPIO"] = New_Value_Int;
          Wire_SCL = New_Value_Int;
          Restart = true;
        }
      }
    }
    return Restart;
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void loop() {
    if (_Scan_Done) return;
    _Scan_Done = true;

    Serial.print("Data = " + String(Wire_SDA) + "    Clock = " + String(Wire_SCL));
    Serial.println("   I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--I2C--");
    I2Cscan();
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  bool _Scan_Done = false;

  // **********************************************************************************************
  // !!!!!!!!!!!!!!!!!!
  // HERE THE OLD ENTRY FROM _My_Settings_Buffer MUST BE REMOVED FIRST !!!!!!
  // !!!!!!!!!!!!!!!!!!
  // **********************************************************************************************
  void I2Cscan() {
#ifdef ESP32
    int Wire_Clock_Old = Wire.getClock();
#else
    int Wire_Clock_Old = 100000;
#endif

    // ********************************************
    // remove previous found devices from json tree
    // ********************************************
    JsonObject DocumentRoot = _My_Settings_Buffer.as<JsonObject>();
    for (JsonPair KeyValue : DocumentRoot) {
      String Key = String(KeyValue.key().c_str());
      if (Key.startsWith("I2C Device")) DocumentRoot.remove(Key);
    }

    const long allSpeed[] = {50, 100, 200, 300, 400, 500, 600, 700, 800};
    long speed[sizeof(allSpeed) / sizeof(allSpeed[0])];
    int addressStart = 0;
    int addressEnd = 127;

#define RESTORE_LATENCY 5 // for delay between tests of found devices.
    uint8_t count = 0;
    int speeds = 5;

    speeds = sizeof(allSpeed) / sizeof(allSpeed[0]);
    for (int i = 0; i < speeds; i++) {
      speed[i] = allSpeed[i];
    }

    TwoWire *wi;
    wi = &Wire;

    // print header
    Serial.print("TIME\tHEX\t");
    for (uint8_t s = 0; s < speeds; s++) {
      Serial.print("\t");
      Serial.print(speed[s]);
    }
    Serial.println("\t[KHz]");
    for (uint8_t s = 0; s < speeds + 4; s++) {
      Serial.print("--------");
    }
    Serial.println();

    int Count = 0;

    for (uint8_t address = addressStart; address <= addressEnd; address++) {
      bool printLine = true;
      bool found[speeds];
      bool fnd = false;

      for (uint8_t s = 0; s < speeds; s++) {
#if ARDUINO >= 158
        wi->setClock(speed[s] * 1000);
#else
        TWBR = (F_CPU / (speed[s] * 1000) - 16) / 2;
#endif
        wi->beginTransmission(address);
        found[s] = (wi->endTransmission() == 0);
        fnd |= found[s];
        // give device 5msec
        if (fnd) delay(RESTORE_LATENCY);
      }
      if (fnd) count++;
      printLine |= fnd;

      bool Found_Something = false;
      for (uint8_t s = 0; s < speeds; s++) {
        if (found[s]) {
          Found_Something = true;
          break;
        }
      }
      if (Found_Something) {

        Serial.print(millis());

        String Line = "\t0x";
        if (address < 0x10) Line += "0";
        Line += String(address, HEX);
        Count += 1;
        _My_Settings_Buffer["I2C Device " + String(Count)] = Line;
        Serial.print(Line);

        Serial.print(F("\t"));

        for (uint8_t s = 0; s < speeds; s++) {
          Serial.print("\t");
          Serial.print(found[s] ? "V" : ".");
        }
        Serial.println();
      }
    }
    Serial.println();

    Wire.setClock(Wire_Clock_Old);
    Settings.Set_Unstored_Changes();
  }
};
#endif
