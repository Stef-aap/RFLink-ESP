
// Version 0.3, 16-03-2020, SM
//    - Function Show extended with Size parameter
// Version 0.2, 20-12-2019, SM
//
// Version 0.1, initial version, 09-12-2019, SM

#ifndef Receiver_SSD1306_h
#define Receiver_SSD1306_h 0.3

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMonoBoldOblique18pt7b.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 OLED_Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ***********************************************************************************
//  https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf
//  https://learn.adafruit.com/adafruit-gfx-graphics-library?view=all#graphics-primitives
// ***********************************************************************************
class _Receiver_SSD1306 : public _Receiver_BaseClass {
public:
  float Bar_1_Max = 64;

  // ***********************************************************************
  // Constructor,
  // ***********************************************************************
  _Receiver_SSD1306() {
    this->Character_Display = true;
    this->Name = "SSD1306 OLED Display 128*64 // kar=10/4";
    Version_Name = "V" + String(Receiver_SSD1306_h) + "   Receiver_SSD1306.h";
    Serial.println("CREATE    " + Version_Name);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  void setup() {

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!OLED_Display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
    }

    // to get rid of the AdaFruit Logo
    OLED_Display.clearDisplay();
    OLED_Display.display();
    OLED_Display.setTextSize(this->_Size);
  }

  // **********************************************************************************************
  // **********************************************************************************************
  bool Check_Modified_Settings() { return false; }

  // **********************************************************************************************
  // **********************************************************************************************
  void Default_Settings(bool Force = false) { Serial.println("WARNING: no Default Settings for : " + Version_Name); }

  // ***********************************************************************
  // ***********************************************************************
  bool Send_Data(String JSON_Message) { return true; }

  // ***********************************************************************
  // ***********************************************************************
  int _Char_Time_Count = 0;
  void loop() {
    return;

    if (millis() - _Sample_Time_Last >= 200) {
      _Sample_Time_Last = millis();

      int Height;

      // *****************************
      // Display the main Bar
      // *****************************
      // Height = Bar_Value ;
      // OLED_Display.fillRect ( 51,      0, 16, Height    , BLACK ) ;
      // OLED_Display.fillRect ( 51, Height, 16, 64-Height , WHITE ) ;

      // *****************************
      // Add the gradient to the Bar
      // *****************************
      if (Gradient >= 0) {
        OLED_Display.fillRect(61, Height - Gradient, 5, Gradient, WHITE);
      } else {
        OLED_Display.fillRect(61, Height, 5, -Gradient, BLACK);
      }

      // *****************************
      // Add noise signal to the Bar
      // *****************************
      int Ruis = 10; // random ( 0, 20 );
      if (Odd_Frame) {
        OLED_Display.fillRect(51, Height, 3, Ruis, BLACK);
      } else {
        OLED_Display.fillRect(51, Height - Ruis, 3, Ruis, WHITE);
      }
      Odd_Frame = !Odd_Frame;

      _Char_Time_Count += 1;

      OLED_Display.display();
    }
  }

  // ***********************************************************************
  // Displayed a Bar in the center of a 128 * 65 display
  // The scaling is related to the maximum Bar_1_Max
  // Noise must be >= 0
  // Gardient gives a pole to the right up or down
  // ***********************************************************************
  void Bar_1(float Value, int Noise, int Gradient) {
    // *****************************
    // Display the main Bar
    // *****************************
    int Bar = 64 - (int)round(64 * Value / Bar_1_Max);
    OLED_Display.fillRect(51, 0, 16, Bar, BLACK);
    OLED_Display.fillRect(51, Bar, 16, 64 - Bar, WHITE);

    // *****************************
    // Add noise signal to the Bar
    // *****************************
    if (Noise > 0) {
      if (Odd_Frame) {
        OLED_Display.fillRect(51, Bar, 3, Noise, BLACK);
      } else {
        OLED_Display.fillRect(51, Bar - Noise, 3, Noise, WHITE);
      }
    }

    // *****************************
    // Add the gradient to the Bar
    // *****************************
    if (Gradient >= 0) {
      OLED_Display.fillRect(61, Bar - Gradient, 5, Gradient, WHITE);
    } else {
      OLED_Display.fillRect(61, Bar, 5, -Gradient, BLACK);
    }

    Odd_Frame = !Odd_Frame;
    OLED_Display.display();
  }

  // ***********************************************************************
  // ***********************************************************************
  void Show(String Line, int x = -1, int y = -1, int Len = -1, int Size = 2) {
    OLED_Display.setTextSize(Size);

    OLED_Display.setTextColor(WHITE, BLACK); // Draw 'inverse' text
    if ((x >= 0) && (y >= 0)) {
      OLED_Display.setCursor(x * 12, y * 16);
    }

    if (Len > 0) {
      String Spaces = "";
      for (int i = 0; i < Len; i++) {
        Spaces += " ";
      }
      OLED_Display.print(Spaces);
      OLED_Display.setCursor(x * 12, y * 16);
    }

    OLED_Display.print(Line);

    OLED_Display.display();
  }

  // ***********************************************************************
private:
  // ***********************************************************************
  unsigned long _Sample_Time_Last = 0;

  float Bar_Value = 50;
  bool Odd_Frame = false;
  float Bar_Delta = 0.5;
  bool Bar_Up = true;

  int Gradient = 7;
  int _Size = 2;
};
#endif
