#include <Arduino.h>
#include <SPI.h>
#include "RTClib.h"
#include <Wire.h>
#include <TM1637Display.h>
#include "FastLED.h"
#include <EasyButton.h>

#define SPECIAL_MOON 0
#define SPECIAL_SUN 1

#define UPDATE_PERIOD_MINUTES 1
#define LED_PIN  9   
#define CLK_PIN 2
#define DIO_PIN 3
#define BUTTON_HOURS_UP 8
#define BUTTON_MINUTES_UP 7   
#define SCREEN_ADDRESS 0x3C

#define SINGULAR_VERBAL_FORM 0
#define PLURAL_VERBAL_FORM 1

#define BRIGHTNESS_DISPLAY 2
#define BRIGHTNESS_LED 80

CRGB leds[144];

RTC_DS1307 rtc;

TM1637Display display(CLK_PIN, DIO_PIN);

EasyButton buttonHoursUp(BUTTON_HOURS_UP, 35U, false, false);
EasyButton buttonMinutesUp(BUTTON_MINUTES_UP, 35U, false, false);


bool mustUpdateLedMatrix = true;

// DA BASSO VERSO ALTO
const unsigned char specialSymbols[]  = {
  133, //LUNA
  131 //SOLE
};

const unsigned char verbalForms[2][6] = {
  {132,200,200,200,200,200}, //E'
  {108,109,110,111,118,119} //SONO LE-
};

const unsigned char hours[13][12] = {
  {134,135,136,137,138,139,140,141,142,143,200,200},  //MEZZANOTTE
  {113,114,115,116,200,200,200,200,200,200,200,200},  //L'UNA
  {105,106,107,200,200,200,200,200,200,200,200,200},  //DUE
  {102,103,104,200,200,200,200,200,200,200,200,200}, //TRE
  {84,85,86,87,88,89,90,200,200,200,200,200}, //QUATTRO
  {96,97,98,99,100,101,200,200,200,200,200,200},  //CINQUE
  {81,82,83,200,200,200,200,200,200,200,200,200},  //SEI
  {91,92,93,94,95,200,200,200,200,200,200,200},  //SETTE
  {77,78,79,80,200,200,200,200,200,200,200,200}, //OTTO
  {60,61,62,63,200,200,200,200,200,200,200,200}, //NOVE
  {72,73,74,75,76,200,200,200,200,200,200,200},  //DIECI
  {64,65,66,67,68,69,200,200,200,200,200,200}, //UNDICI
  {120,121,122,123,124,125,126,127,128,129,130,200} //MEZZOGIORNO
};

const unsigned char minutes[11][16] = {
  {71,0,1,2,3,4,5,200,200,200,200,200,200,200,200,200}, //E CINQUE
  {71,36,37,38,39,40,200,200,200,200,200,200,200,200,200,200}, //E DIECI
  {71,41,42,6,7,8,9,10,11,200,200,200,200,200,200,200}, //E UN QUARTO
  {71,31,32,33,34,35,200,200,200,200,200,200,200,200,200,200}, //E VENTI
  {71,31,32,33,34,35,0,1,2,3,4,5,200,200,200,200}, //E VENTI CINQUE
  {71,43,44,45,46,47,200,200,200,200,200,200,200,200,200,200}, //E MEZZA
  {71,24,25,26,27,28,29,0,1,2,3,4,5,200,200,200}, //E TRENTA CINQUE
  {71,48,49,50,51,52,53,54,55,200,200,200,200,200,200,200}, //E QUARANTA
  {56,57,58,59,41,42,6,7,8,9,10,11,200,200,200,200}, //MENO UN QUARTO
  {71,13,14,15,16,17,18,19,20,21,200,200,200,200,200,200}, //E CINQUANTA
  {71,13,14,15,16,17,18,19,20,21,0,1,2,3,4,5}  //E CINQUANTA CINQUE
};



void turnOnSpecialSymbols(int currentHours, int currentMinutes) 
{
    int ledIndex = (currentHours == 0 && currentMinutes < 5)?  specialSymbols[SPECIAL_MOON] : ((currentHours == 12 && currentMinutes < 5)? specialSymbols[SPECIAL_SUN] : -1);

    if (ledIndex == -1)
      return;
    
    if (ledIndex  == specialSymbols[SPECIAL_MOON])
      leds[ledIndex] = CRGB::Blue;
    else 
      leds[ledIndex] = CRGB::Yellow;

}

void turnOnLedsForVerbalForm(int currentHours, int currentMinutes){
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
    Serial.println();
}

void turnOnLedsForHours(int currentHours, int currentMinutes){
    
    //Caso speciale per "meno un quarto"
    int arrangedHours = (currentMinutes == 45)? (currentHours+ 1) % 24 : currentHours;

    int indexOfHours = (arrangedHours > 12)? arrangedHours - 12: arrangedHours;

    //Index for verbal form
    for (size_t i = 0; i < 12; i++)
    {
        unsigned int ledToTurnOn = hours[indexOfHours][i];
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
    Serial.println();
}

void turnOnLedsForMinutes(int currentMinutes){
    
    // if minutes are 0 must not turn on led
    if (currentMinutes == 0)
      return;

    int arrangedMinutes = currentMinutes / 5 * 5; //get nearest multiple of 5
    int indexOfMinutes = arrangedMinutes / 5 -1;

    //Index for verbal form
    for (size_t i = 0; i < 16; i++)
    {
        unsigned int ledToTurnOn = minutes[indexOfMinutes][i];
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
    Serial.println();
}


void printTimeOnDisplay(int currentHours, int currentMinutes) {
  // Create time format to display as single decimal value
	int displaytime = (currentHours * 100) + currentMinutes;
  	// Display the current time in 24 hour format with leading zeros and a center colon enabled
	display.showNumberDecEx(displaytime, 0b11100000, true);
}


bool shouldUpdateLedMatrix(int currentHours, int currentMinutes, int updatePeriodInMinutes) 
{
    return ((currentMinutes + currentHours*60) % updatePeriodInMinutes) == 0;
}

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
    FastLED.setBrightness(BRIGHTNESS_LED);

    // Set the display brightness (0-7)
    display.setBrightness(BRIGHTNESS_DISPLAY);
    // Clear the display
    display.clear();


    buttonHoursUp.begin();
    buttonMinutesUp.begin();
}




// the loop function runs over and over again forever
void loop() {
    DateTime now = rtc.now();
    buttonHoursUp.read();
    buttonMinutesUp.read();

    if (buttonHoursUp.wasPressed()) 
    {
      uint8_t hours = (now.hour() + 1) % 24;
      rtc.adjust(DateTime(now.year(),now.month(),now.day(),hours,now.minute(),now.second()));
      now = rtc.now();
      mustUpdateLedMatrix = true;
    }

    if (buttonMinutesUp.wasPressed()) 
    {
      uint8_t minutes = (now.minute() + 1) % 60;
      rtc.adjust(DateTime(now.year(),now.month(),now.day(),now.hour(),minutes,now.second()));
      now = rtc.now();
      mustUpdateLedMatrix = true;
    }

    int currentHours = now.hour();
    int currentMinutes = now.minute();

    Serial.print(currentHours);
    Serial.print(":");
    Serial.println(currentMinutes);
    
    if (mustUpdateLedMatrix || shouldUpdateLedMatrix(currentHours, currentMinutes, 5)) {

      mustUpdateLedMatrix = false;
      //turn off all LEDS
      FastLED.clearData();
      turnOnLedsForVerbalForm(currentHours, currentMinutes);
      turnOnLedsForHours(currentHours, currentMinutes);
      turnOnLedsForMinutes(currentMinutes);
      turnOnSpecialSymbols(currentHours, currentMinutes); 
      //Update leds
      FastLED.show();

    } 

    printTimeOnDisplay(currentHours, currentMinutes);
    delay(50);
}
