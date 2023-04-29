#include <Arduino.h>
 #include <SPI.h>
//For RTC support
#include "RTClib.h"
// Generic I2C support
#include <Wire.h>

#include "FastLED.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define UPDATE_PERIOD_MINUTES 1
#define LED_PIN  5   

#define SINGULAR_VERBAL_FORM 0
#define PLURAL_VERBAL_FORM 1

CRGB leds[144];

RTC_DS1307 rtc;


char ouputBuffer[80];
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);



const unsigned int verbalForms[2][6] = {
   {0,200,200,200,200,200}, //E'
   {24,25,26,27,34,35} //SONO LE
};

 const unsigned int hours[13][12] = {
  {1,2,3,4,5,6,7,8,9,10,11,200},  //"MEZZANOTTE",
  {29,30,31,32,200,200,200,200,200,200,200,200},  //L'UNA,
  {45,46,47,200,200,200,200,200,200,200,200,200},  //DUE
  {42,43,44,200,200,200,200,200,200,200,200,200}, //TRE
  {48,49,50,51,52,53,54,200,200,200,200,200}, //QUATTRO
  {36,37,38,39,40,41,200,200,200,200,200,200},  //CINQUE
  {69,70,71,200,200,200,200,200,200,200,200,200},  //SEI
  {55,56,57,58,59,200,200,200,200,200,200,200},  //SETTE
  {65,66,67,68,200,200,200,200,200,200,200,200}, //OTTO
  {72,73,74,75,200,200,200,200,200,200,200,200}, //NOVE
  {60,61,62,63,64,200,200,200,200,200,200,200},  //DIECI
  {76,77,78,79,80,81,200,200,200,200,200,200}, //UNDICI
  {12,13,14,15,16,17,18,19,20,21,22,23} //MEZZOGIORNO
};

const unsigned int minutes[11][16] = {
  {83,132,133,134,135,136,137,200,200,200,200,200,200,200,200,200}, //E CINQUE
  {83,96,97,98,99,100,200,200,200,200,200,200,200,200,200,200}, //E DIECI
  {83,101,102,138,139,140,141,142,143,200,200,200,200,200,200,200}, //E UN QUARTO
  {83,115,116,117,118,119,200,200,200,200,200,200,200,200,200,200}, //E VENTI
  {83,115,116,117,118,119,132,133,134,135,136,137,200,200,200,200}, //E VENTI CINQUE
  {83,103,104,105,106,107,200,200,200,200,200,200,200,200,200,200}, //E MEZZA
  {83,108,109,110,111,112,113,132,133,134,135,136,137,200,200,200}, //E TRENTA CINQUE
  {83,84,85,86,87,88,89,90,91,200,200,200,200,200,200,200}, //E QUARANTA
  {92,93,94,95,101,102,138,139,140,141,142,143,200,200,200,200}, //MENO UN QUARTO
  {83,121,122,123,124,125,126,127,128,200,200,200,200,200,200,200}, //E CINQUANTA
  {83,121,122,123,124,125,126,127,128,132,133,134,135,136,137,200}  //E CINQUANTA CINQUE
};


unsigned int lastTimeInMinutes = 0;



// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

    FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, 144).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(80);
}

// the loop function runs over and over again forever
void loop() {
    DateTime now = rtc.now();
    int currentHours = now.hour();
    int currentMinutes = now.minute();

    Serial.print(currentHours);
    Serial.print(":");
    Serial.println(currentMinutes);
    
    int verbalFormIndex = (currentHours == 12 || currentHours == 13 || currentHours == 0)? 0 : 1;

    //Index for verbal form
    for (size_t i = 0; i < 6; i++)
    {
        unsigned int ledToTurnOn = verbalForms[verbalFormIndex][i];
        if (ledToTurnOn ==  200)
        {
            break;
        } 
        else 
        {
            leds[ledToTurnOn] = CRGB::White;
            Serial.print(ledToTurnOn);
            Serial.print(" ");
        }
    }
    FastLED.show();
    Serial.println();
    

    delay(10000);
}
