// WAAROM SENSOR EN NIET RECEIVER ?

// Version 0.1, 05-08-2019, SM, checked by ..
//    - initial version
// ***********************************************************************************

#ifndef Sensor_MQTTBroker_h
#define Sensor_MQTTBroker_h 0.1

#include "uMQTTBroker.h"
#ifndef NOT_INCLUDE_RECEIVER_SDFAT
  #include "Receivers/Receiver_SDfat.h"
#endif

#define _MQTTBroker_Max_N 10

// ****************************************************************************
// Custom broker class with overwritten callback functions
// ****************************************************************************
class _myMQTTBroker : public uMQTTBroker {
public:
  String _MQTTBroker_List[_MQTTBroker_Max_N];
  String _MQTTBroker_Value[_MQTTBroker_Max_N];
  int _MQTTBroker_List_Len = 0;

  // class _myMQTTBroker ******************************************
  // **************************************************************
  virtual bool onConnect(IPAddress addr, uint16_t client_count) {
    Serial.println(addr.toString() + " connected");
    return true;
  }

  // class _myMQTTBroker ******************************************
  // **************************************************************
  virtual bool onAuth(String username, String password) {
    Serial.println("Username/Password: " + username + "/" + password);
    return true;
  }

  // class _myMQTTBroker ******************************************
  // **************************************************************
  bool New_Items_Found() {
    bool Result = this->_New_Items;
    this->_New_Items = false;
    return Result;
  }

  // class _myMQTTBroker ******************************************
  // **************************************************************
  virtual void onData(String topic, const char *data, uint32_t length) {
    char data_str[length + 1];
    os_memcpy(data_str, data, length);
    data_str[length] = '\0';

    Set_Signal_LED(1, 100, 200);
    Serial.println("received topic '" + topic + "' with data '" + (String)data_str + "'");

    DynamicJsonDocument JSON_Buffer(200); // Dynamic / Static ???  Heap / Stack of omgekeerd
    DeserializationError Error = deserializeJson(JSON_Buffer, data_str);
    if (Error) return;
    JsonObject Root = JSON_Buffer.as<JsonObject>();

    String Header;
    String Data;
    for (JsonPair KeyValue : Root) {
      String Key = String(KeyValue.key().c_str());
      // Serial.println ( "Recived key" + Key ) ;
      // JsonVariant Value = KeyValue.value () ;
      String Value = JSON_Buffer[Key].as<String>();

      // Test if Key already known
      bool Found = false;
      for (int i = 0; i < this->_MQTTBroker_List_Len; i++) {
        if (Key == this->_MQTTBroker_List[i]) {
          this->_MQTTBroker_Value[i] = Value;
          Found = true;
          break;
        }
      }
      if (!Found && (this->_MQTTBroker_List_Len < _MQTTBroker_Max_N)) {
        this->_MQTTBroker_List[this->_MQTTBroker_List_Len] = Key;
        this->_MQTTBroker_Value[this->_MQTTBroker_List_Len] = Value;
        this->_MQTTBroker_List_Len += 1;
        this->_New_Items = true;
      }
    }

    /*
      #ifdef FileSystem_SDfat
        //topic - spltter
        //data pares json
        My_StringSplitter *Splitter = new My_StringSplitter ( topic, '/' ) ;
        String Filename = Splitter -> getItemAtIndex ( -1 ) ;
        Filename += ".csv" ;
// Serial.println ( Filename ) ;


        //char json[] = "{\"first\":\"hello\",\"second\":\"world\"}";
        DynamicJsonDocument JSON_Buffer ( 200 ) ;  // Dynamic / Static ???  Heap / Stack of omgekeerd
        DeserializationError Error = deserializeJson ( JSON_Buffer, data_str ) ;

        if ( Error ) return ;

        JsonObject Root = JSON_Buffer.as<JsonObject>();

        String Header ;
        String Data   ;
        for ( JsonPair KeyValue : Root ) {
          String Key = String ( KeyValue.key().c_str() ) ;
          //JsonVariant Value = KeyValue.value () ;
          String Value = JSON_Buffer [ Key ].as<String>();
          //Serial.print   ( Key   ) ;
          //Serial.print   ( " = " ) ;
          //Serial.println ( Value ) ;
          Header += Key   + '\t' ;
          Data   += Value + '\t' ;
        }
//Serial.println ( "Header = " + Header ) ;
//Serial.println ( "Data   = " + Data   ) ;

        ((_Receiver_SDfat*)_p_Receiver_SDfat) -> Append_2_CSV ( Filename, Header, Data ) ;
      #endif
*/
  }

  // class _myMQTTBroker ******************************************
  // **************************************************************
private:
  bool _New_Items = false;
};

// ***********************************************************************************
// ***********************************************************************************
class _Sensor_MQTTBroker : public _Sensor_BaseClass {
public:
  // Sensor_MQTTBroker ********************************************
  // **************************************************************
  _Sensor_MQTTBroker(bool Make_Device_Active = true) { this->Device_Active = Make_Device_Active; }

  // Sensor_MQTTBroker ********************************************
  // **************************************************************
  void setup() {
    if (!this->Device_Active) return;

    _Broker.init();
    _Broker.subscribe("#");

    Version_Name = "V" + String(Sensor_MQTTBroker_h) + "   Sensor_MQTTBroker.h";
    Serial.println("CREATE    " + Version_Name);

    _JSON_Long_Header = File_System.Read_File("/MBH.txt");
    _JSON_Short_Header = _JSON_Long_Header;

    Help_Text = "_Sensor_MQTTBroker  .... +++";
  }

  // Sensor_MQTTBroker ********************************************
  // **************************************************************
  void loop() {
    //      delay ( 1 ) ;      // needed to spent time in broker loop
  }

  // Sensor_MQTTBroker ********************************************
  // **************************************************************
  void Get_JSON_Data() {
    if (!this->Device_Active) return;

    if (_Broker.New_Items_Found()) {
      this->_JSON_Long_Header = "";
      for (int i = 0; i < _Broker._MQTTBroker_List_Len; i++) {
        this->_JSON_Long_Header += _Broker._MQTTBroker_List[i] + '\t';
      }
      _JSON_Short_Header = _JSON_Long_Header;
      Update_All_Headers();

      File_System.Store_File("/MBH.txt", _JSON_Long_Header);
    }

    for (int i = 0; i < _Broker._MQTTBroker_List_Len; i++) {
      String Value = _Broker._MQTTBroker_Value[i];
      JSON_Data += " \"" + _Broker._MQTTBroker_List[i] + "\":" + Value + ',';
      JSON_Short_Data += Value + '\t';
    }
    return;

    _Broker.publish("broker/counter", String(millis()));

    _Publisch_Counter += 1;
    if (_Publisch_Counter % 10 == 0) {
      _Broker.cleanupClientConnections();
      Serial.println("=====================================> MQTT Connections Cleanup " + String(_Publisch_Counter));
    }
  }

  // Sensor_MQTTBroker ********************************************
  // **************************************************************
private:
  _myMQTTBroker _Broker;
  int _Publisch_Counter = 0;
};
#endif
