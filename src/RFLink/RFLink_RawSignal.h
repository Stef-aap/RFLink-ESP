// Version 0.2
//    - LoopsPerMilli automatic chaged with the device
//    - added protection for only include once

// Version 0.1

#ifndef RawSignal_h
#define RawSignal_h 0.2

// ***********************************************************************************
// Some time critical parameters are declared global
// ***********************************************************************************
unsigned long FETCH_Pulse_Plus_1; //  here 3320,  inside 3540
// ***********************************************************************************
boolean FetchSignal(int Receive_Pin) {
// ************************************************************
//   LoopsPerMilli      maxloops       timeout [msec)
//      500               3500          0.8 ... 1.7  (zeer wisselend)
//     1500              10500          2.4
//     4000              28000          6.3
// detectie van Notenhout knop A ( QIAchip = EV1527)
//  1500:
//  2370,270,690,270,690,270,690,270,690,270,690,270,690,270,690,270,720,270,690,750,210,240,690,750,210,750,210,750,210,240,720,750,240,750,210,720,240,720,240,720,240,240,720,240,720,240,720,720,210,210,
//              2640;
//  4000:
//  660,270,690,270,690,270,690,240,690,270,690,270,690,270,750,210,690,270,690,720,240,240,720,750,210,750,210,750,210,270,690,750,210,750,210,750,210,750,240,720,240,240,690,240,720,210,720,690,270,240,
//            7050;
// We zien de beginpuls korter worden en de eindpuls langer (dus 1500 was toch wel een hele mooie waarde
// BELANGRIJK: de laatste puls is dus de timeout in usec !!!
// ************************************************************
// for the ESP8266 we've tested the width of the last pulse in Learning_Mode = 5
//     LoopsPerMille     Lastpulswidth
//        800               3900
//       1000               4890
// so 800 is a good value !!
// ************************************************************
// const unsigned long LoopsPerMilli = 345;   // <<< OORSPRONKELIJKE WAARDE VOOR ARDUINO MEGA
// 2500 was optimaal, maar we hebben de loop sneller gemaakt 3540 naar 3320 usec
// dus verhogen we met 3540/3320 =>  2670
#ifdef ESP32
  const unsigned long LoopsPerMilli = 2700;
#else
  const unsigned long LoopsPerMilli = 800;
#endif
  // ************************************************************
  // ************************************************************

  // ******************************************************************************************
  // WEIRD een aantal parameters niet globaal definieeren, maar hier, maakt de routine sneller
  // in de comment staat de breedte van de laatste puls (getriggered door een timeout, dus korter is sneller)
  // ******************************************************************************************
  unsigned long LastPulse;                              // 3320 i.p.v. 3540
  bool Toggle = true;                                   // 3320 i.p.v. 3750
  int RawCodeLength = 0;                                // maakt niet uit
  unsigned long PulseLength;                            // 3320 i.p.v. 3540
  unsigned long numloops;                               // 3320 i.p.v. 4200
  bool Start_Level = LOW;                               // via #define maakt niet uit
  unsigned maxloops = (SIGNAL_TIMEOUT * LoopsPerMilli); // via #define maakt niet uit
  // ******************************************************************************************

  // ************************************************************
  // wacht op lang laag nivo (Start_Level) = start van nieuwe sequence
  // ************************************************************
  bool Started = false;
  unsigned long Start_Time = 500 + millis(); // 100 .. 500 msec, maakt niet zo veel uit
  while (!Started) {
    // ************************************************************
    // als het nivo laag is, wacht tot het einde van dit laag nivo
    // ************************************************************
    //    while ( ( digitalRead ( PIN_RF_RX_DATA ) == Start_Level ) ) ;
    while ((digitalRead(Receive_Pin) == Start_Level) && (millis() < Start_Time))
      ;

    // ************************************************************
    // hier is het nivo hoog, wacht totdat het naar laag springt
    // als we de vorige while lus niet doorlopen zijn,
    //   zijn we halverwege een positieve puls deze routine binnen gekomen
    //   en kunnen we deze positieve puls niet nauwkeurig meten
    //   maar vanwege de herhalende sequences zullen we toch regelmatig de juiste waarde meten
    // ************************************************************
    FETCH_Pulse_Plus_1 = micros();
    //    while ( ( digitalRead ( Receive_Pin ) != Start_Level ) ) ;
    while ((digitalRead(Receive_Pin) != Start_Level) && (millis() < Start_Time))
      ;

    // ************************************************************
    // Wacht tot het einde van de laag periode
    // ************************************************************
    LastPulse = micros();
    //    while ( ( digitalRead ( Receive_Pin ) == Start_Level ) ) ;
    while ((digitalRead(Receive_Pin) == Start_Level) && (millis() < Start_Time))
      ;
    PulseLength = micros() - LastPulse;

    // ************************************************************
    // Als de laag periode voldoende lang is, is het de startpuls
    // Berg dan ook de positieve en negatieve startpuls op
    // ************************************************************
    if (PulseLength > 5000) {
      // Serial.println ( PulseLength ) ;
      RawSignal.Pulses[RawCodeLength++] = LastPulse - FETCH_Pulse_Plus_1;
      RawSignal.Pulses[RawCodeLength++] = PulseLength;
      Started = true;
    }
    if (millis() > Start_Time) return false;
  }

  RawSignal.Min = 10000;
  RawSignal.Max = 0;
  RawSignal.Mean = 0;
  // ************************************************************
  // Na een start, wordt hier de hele serie pulsen gemeten
  // en er wordt gestopt als een (te) lange puls wordt gevonden (of het buffer vol is)
  // ************************************************************
  do {
    // ************************************************************
    // Meet de breedte van het huidige nivo
    //   break als het nivo te lang duurt
    // ************************************************************
    numloops = 0;
    LastPulse = micros();
    while ((digitalRead(Receive_Pin) == Start_Level) ^ Toggle)
      if (numloops++ == maxloops) break;
    PulseLength = micros() - LastPulse;

    // ************************************************************
    // spring uit de loop als we een te korte puls detecteren
    // ************************************************************
    if (PulseLength < MIN_PULSE_LENGTH) break;

    // ************************************************************
    // Inverteer het nivo waar naar we gaan zoeken
    // ************************************************************
    Toggle = !Toggle;

    // ************************************************************
    // Geldige puls gevonden, dus opbergen
    // ************************************************************
    RawSignal.Pulses[RawCodeLength++] = PulseLength;

    // ************************************************************
    // keep track of ststistics
    // ************************************************************
    if (numloops < maxloops) {
      if (PulseLength < RawSignal.Min) RawSignal.Min = PulseLength;
      if (PulseLength > RawSignal.Max) RawSignal.Max = PulseLength;
      RawSignal.Mean += PulseLength;
    }

    // ************************************************************
    // stop als er een lange puls is gevonden of als het buffer vol is
    // ************************************************************
  } while ((RawCodeLength < RAW_BUFFER_SIZE) && (numloops < maxloops));
  // Serial.println ( RawCodeLength ) ;

  // ************************************************************
  // We hebben nu het einde van een signaal bereikt
  //   als we genoeg pulsen hebben,
  //     return true
  // KAKU has a long startperiod of 2500 usec
  // ************************************************************
  if ((RawCodeLength >= MIN_RAW_PULSES) && (RawCodeLength <= MAX_RAW_PULSES) && (RawSignal.Min > 150) &&
      (RawSignal.Max < 3000)) {
    RawSignal.Mean = RawSignal.Mean / (RawCodeLength - 3);
    RawSignal.Number = RawCodeLength - 1;       // Number of received pulse times (pulsen *2)
    RawSignal.Pulses[RawSignal.Number + 1] = 0; // Last element contains the timeout.
    RawSignal.Time = millis();                  // Time the RF packet was received (to keep track of retransmits
                                                // Serial.print ( "D" ) ;
                                                // Serial.print ( RawCodeLength ) ;
    return true;
  }

  // ************************************************************
  // anders opnieuw beginnen
  // ************************************************************
  else {
    RawSignal.Number = 0;
  }

  return false;
}

#endif
