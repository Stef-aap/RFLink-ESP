
// ****************************************************************************************
// LEARNING_MODE
// ****************************************************************************************
String Learning_Modes_Text = "\
-----  Learning_Modes  -----\n\
0 : Production Mode\n\
1 : Real Learning Mode\n\
2 : one detection  + statistics\n\
3 : all detections + statistics\n\
4 : display pulstime\n\
5 : display pulstime rounded at 30 usec\n\
6 : Statistics, Binairy and Hex Results\n\
7 : Statistics followed, by normal detection\n\
8 : Transmit a predefined sequence\n\
9 : same as 8, but indefinitly" ;

String Commands_Text = "\n\
10;LIST;          // list all commands\n\
10;PING;          // return PONG\n\
10;REBOOT;        // reboot RFLink\n\
10;VERSION;       // displays version information\n\
10;RF_Command;    // Send an RF package\n\
10;DEBUG=x;       // Enter Learning/Debug Mode\n\
12;Name;ID;       // In Learning_Mode=1 add this device\n\
X                 // In Learning_Mode=1 add the last seen Device\n\
19;DIR;           // Directory of the file-system\n\
19;DUMP;Filename; // Print the content of the file\n\
19:DEL;Filename;  // Delete the file\n\
19:CLEAR;         // Delete all registered Devices\n\
11;<LINE>         // Complete <LINE> is echoed" ;


// ***********************************************************************************
// ***********************************************************************************
//bool On_Off_Command () {
//  byte kar = InputBuffer_Serial [ SerialInByteCounter - 3 ] ;
//  return kar == 'N' || kar == 'n' ;
//}


// ***********************************************************************************
// ***********************************************************************************
void Handle_Serial () {
    // *********************************************
  // If a complete line is received
  // *********************************************
  if ( Serial_Command ) {

    // *********************************************
    // *********************************************
    if ( ( InputBuffer_Serial[0] == 'X' ) && ( Learning_Mode == 1 ) ) {
      if ( Unknown_Device_ID.length() > 0 ) {
        RFLink_File.Add_Device ( Unknown_Device_ID ) ;
        RFLink_File.Print_Devices () ;
      }
    }

    // *********************************************
    // *********************************************
    else if ( ( InputBuffer_Serial[0] == 'Y' ) && ( Learning_Mode == 1 ) ) {
      if ( Unknown_Device_ID.length() > 0 ) {
        RFLink_File.Add_Device ( "-" + Unknown_Device_ID ) ;
        RFLink_File.Print_Devices () ;
      } 
//      Unknown_Device_ID = Randomize_Device_ID ( Unknown_Device_ID ) ;
//      if ( Unknown_Device_ID.length() > 0 ) {
//        Serial.println (">>>");
//        Serial.println ( Unknown_Device_ID ) ;
//        Serial.println ("<<<");
//        //RFLink_File.Add_Device ( Unknown_Device_ID ) ;
//        //RFLink_File.Print_Devices () ;
//      }
    }

    
    // *********************************************
    // 10;   // COMMAND
    // *********************************************
    else if ( strncmp ( InputBuffer_Serial, "10;", 3 ) == 0 ) {  

      // *********************************************
      // LIST all Commands
      // *********************************************
      if ( strcasecmp ( InputBuffer_Serial+3, "LIST;" ) == 0 ) {
        Serial.printf  ( InputBuffer_Serial, "20;%02X;LIST;\r\n", PKSequenceNumber++ ) ;
        Serial.println ( Commands_Text ) ;
        Serial.println ( Learning_Modes_Text ) ;
      }

      // *********************************************
      //   PING
      // Very Important, because this is used by Domoticz
      //    to see if the RFLink is working properly
      // *********************************************
      else if ( strcasecmp ( InputBuffer_Serial+3, "PING;" ) == 0 ) {
        sprintf ( InputBuffer_Serial, "20;%02X;PONG;", PKSequenceNumber++ ) ;
        Serial.println ( InputBuffer_Serial ) ; 
      }

      // *********************************************
      // REBOOT
      // *********************************************
      else if ( strcasecmp ( InputBuffer_Serial+3, "REBOOT;" ) == 0 ) {
        ESP.restart () ;
      }

      // *********************************************
      // VERSION
      // *********************************************
      else if ( strcasecmp ( InputBuffer_Serial+3, "VERSION;" ) == 0 ) {
        Serial.printf ( "20;%02X;VER=%s;REV=%02x;BUILD=%02x;\r\n", PKSequenceNumber++, Version, Revision, Build ) ; 
      }

      // *********************************************
      // DEBUG = ...
      // *********************************************
      else if ( strncasecmp ( InputBuffer_Serial+3, "DEBUG=", 6 ) == 0 ) {
        byte kar = InputBuffer_Serial [9] ;
        Learning_Mode = kar - 0x30 ;
        sprintf ( InputBuffer_Serial, "20;%02X;DEBUG=%i;", PKSequenceNumber++, Learning_Mode ) ;
        Serial.println ( InputBuffer_Serial ) ; 
 
        // *********************************************
        // *********************************************
        if ( Learning_Mode > 0 ) {
          Serial.println ( Learning_Modes_Text ) ;
        }
        
        // *********************************************
        // *********************************************
        if ( Learning_Mode == 1 ) {
          RFLink_File.Print_Devices () ;
        }
        
        // *********************************************
        // *********************************************
        else if ( Learning_Mode == 8 ) {

          //20;7C;EV1527;ID=005DF;SWITCH=01;CMD=ON;
          //20;7D;EV1527;ID=005DF;SWITCH=01;CMD=ON;
          RFL_Protocols.Home_Command ( "10;EV1527;005DF;01;ON;" ) ;
          delay ( 3000 ) ;
          RFL_Protocols.Home_Command ( "10;EV1527;005DF;02;ON;" ) ;


          //20;01;KAKU;ID=2508A7C;SWITCH=A;CMD=OFF;
          //20;02;KAKU;ID=2508A7C;SWITCH=A;CMD=ON;
          //20;04;KAKU;ID=2508A7C;SWITCH=B;CMD=OFF;
          //20;05;KAKU;ID=2508A7C;SWITCH=B;CMD=ON;
//          RFL_Protocols.Home_Command ( "10;KAKU;02508A7C;0A;ON;" ) ;
//          delay ( 3000 ) ;
//          RFL_Protocols.Home_Command ( "10;KAKU;02508A7C;0A;OFF;" ) ;

          //20;4D;PT2262;ID=54555;SWITCH=01;CMD=ON;
          //20;4E;PT2262;ID=54155;SWITCH=01;CMD=OFF;
        }

        // *********************************************
        // *********************************************
        else if ( Learning_Mode == 9 ) {
          while ( true ) {
            RFL_Protocols.Home_Command ( "10;EV1527;005DF;01;ON;" ) ;
            delay ( 3000 ) ;
            RFL_Protocols.Home_Command ( "10;EV1527;005DF;02;ON;" ) ;
            delay ( 3000 ) ;          
          }
        }
      }


      // *********************************************
      // Unhandled Commands
      // 10;TRISTATEINVERT; => Toggle Tristate ON/OFF inversion
      // 10;RTSCLEAN; => Clean Rolling code table stored in internal EEPROM
      // 10;RTSRECCLEAN=9 => Clean Rolling code record number (value from 0 - 15)
      // 10;RTSSHOW; => Show Rolling code table stored in internal EEPROM (includes RTS settings)
      // 10;RTSINVERT; => Toggle RTS ON/OFF inversion
      // 10;RTSLONGTX; => Toggle RTS long transmit ON/OFF 
      //
      // 11;20;0B;NewKaku;ID=000005;SWITCH=2;CMD=ON; => 11; 
      //    is the required node info it can be followed by any custom data which will be echoed 
      // *********************************************


      
      // *********************************************
      // Handle Generic Commands / Translate protocol data into Nodo text commands 
      // *********************************************
      else {
        //10;EV1527;0005df;2;ON
        if ( RFL_Protocols.Home_Command ( InputBuffer_Serial ) ){
          Serial.printf ( "20;%02X;OK;\r\n", PKSequenceNumber++ ) ;  
        }
      }
    
    }

    // *********************************************
    // 12; if Learning_Mode 1, Add New Device
    // *********************************************
    else if ( ( Learning_Mode == 1 ) && 
              ( strncmp ( InputBuffer_Serial, "12;", 3 ) == 0 )) {  
      String Command = String ( InputBuffer_Serial ) ;
      int x1 = Command.indexOf ( ";", 3 ) ;
      x1     = Command.indexOf ( ";", x1+1 ) ;
      String New = Command.substring ( 3, x1+1 ) ;

      RFLink_File.Add_Device ( New ) ;
      RFLink_File.Print_Devices () ;
    }

    // *********************************************
    // 19; System Commands
    // *********************************************
    else if ( strncmp ( InputBuffer_Serial, "19;", 3 ) == 0 ) {  
      String Command = String ( InputBuffer_Serial ) ;
      int    x1      = Command.indexOf   ( ";", 3 ) ;
      String CMD     = Command.substring ( 3, x1 ) ;
      int    x2      = Command.indexOf   ( ";", x1+1 ) ;
      String ARG     = Command.substring ( x1+1, x2) ;
      String Rest    = Command.substring ( x1+1 ) ;

      //Serial.println ( "cmd19" );
      //Serial.println ( CMD ) ;
      //Serial.println ( ARG ) ;

      if ( CMD.equalsIgnoreCase ( "DIR" ) ) {
        RFLink_File.DirList () ;
      } 
      else if ( CMD.equalsIgnoreCase ( "DUMP" ) ) {
        RFLink_File.Dump ( ARG ) ;
      }
      else if ( CMD.equalsIgnoreCase ( "DEL" ) ) {
        RFLink_File.Delete ( ARG ) ;
      }
      else if ( CMD.equalsIgnoreCase ( "CLEAR" ) ) {
        RFLink_File.Clear_Devices () ;
      }
      else if ( CMD.equalsIgnoreCase ( "REMOVE" ) ) {
        RFLink_File.Remove_Device ( Rest ) ;
        RFLink_File.Print_Devices () ;
      }
    }
    
    // *********************************************
    // 11; Echo the complete Line (without the preceeding "11;"
    // *********************************************
    else if ( strncmp ( InputBuffer_Serial, "11;", 3 ) == 0 ) {  
      String Command = String ( InputBuffer_Serial ) ;
      Serial.print ( Command.substring ( 3, -1 ) + "\r\n" ) ;      
    }

    
    // *********************************************
    // *********************************************
    Serial_Command      = false ;  
    SerialInByteCounter = 0 ;  
  } 

  // *********************************************
  // Collect serial bytes until "\n" is detected
  // *********************************************
  //else if ( Serial.available () ) {
  else while ( Serial.available () && !Serial_Command ) {
    SerialInByte = Serial.read ();                

    // *********************************************
    // If "\n", line is complete
    // *********************************************
    if ( SerialInByte == '\n' ) {                                              // new line character
      InputBuffer_Serial [ SerialInByteCounter++ ] = 0 ;
      Serial_Command = true ;   
    }
    // *********************************************
    // otherwise collect as long as there's space in the buffer
    // *********************************************
    else if ( isprint ( SerialInByte ) ) {
      if ( SerialInByteCounter < ( INPUT_COMMAND_SIZE - 1 ) ) {
        InputBuffer_Serial [ SerialInByteCounter++ ] = SerialInByte ;
      }
      // *********************************************
      // otherwise clear the buffer and start all over again
      // *********************************************
      else {
        SerialInByteCounter = 0 ;  
      }
    }
  }

  
}

