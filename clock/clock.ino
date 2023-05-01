#include <Arduino.h>
#include <SPI.h>
#include "RTClib.h"
#include <Wire.h>
#include "FastLED.h"
#include <LiquidCrystal_I2C.h>
#include <EasyButton.h>

#define UPDATE_PERIOD_MINUTES 1
#define LED_PIN  9
#define BUTTON_HOURS_UP 6
#define BUTTON_MINUTES_UP 7   
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     -1 

#define SINGULAR_VERBAL_FORM 0
#define PLURAL_VERBAL_FORM 1

CRGB leds[144];

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

EasyButton buttonHoursUp(BUTTON_HOURS_UP, 35U, true, false);
EasyButton buttonMinutesUp(BUTTON_MINUTES_UP, 35U, true, false);

const unsigned char verbalForms[2][6] = {
   {0,200,200,200,200,200}, //E'
   {24,25,26,27,34,35} //SONO LE
};

 const unsigned char hours[13][12] = {
  {2,3,4,5,6,7,8,9,10,11,200,200},  //"MEZZANOTTE",
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
  {12,13,14,15,16,17,18,19,20,21,22,200} //MEZZOGIORNO
};

const unsigned char minutes[11][16] = {
  {83,132,133,134,135,136,137,200,200,200,200,200,200,200,200,200}, //E CINQUE
  {83,96,97,98,99,100,200,200,200,200,200,200,200,200,200,200}, //E DIECI
  {83,101,102,138,139,140,141,142,143,200,200,200,200,200,200,200}, //E UN QUARTO
  {83,115,116,117,118,119,200,200,200,200,200,200,200,200,200,200}, //E VENTI
  {83,115,116,117,118,119,132,133,134,135,136,137,200,200,200,200}, //E VENTI CINQUE
  {83,103,104,105,106,107,200,200,200,200,200,200,200,200,200,200}, //E MEZZA
  {83,108,109,110,111,112,113,132,133,134,135,136,137,200,200,200}, //E TRENTA CINQUE
  {83,84,85,86,87,88,89,90,91,200,200,200,200,200,200,200}, //E QUARANTA
  {92,93,94,95,101,102,138,139,140,141,142,143,200,200,200,200}, //MENO UN QUARTO
  {83,121,122,123,124,125,126,127,128,129,200,200,200,200,200,200}, //E CINQUANTA
  {83,121,122,123,124,125,126,127,128,129,132,133,134,135,136,137}  //E CINQUANTA CINQUE
};


bool mustUpdateLedMatrix = true;


void turnOnSpecialSymbols(int currentHours, int currentMinutes) 
{
    int ledIndex = (currentHours == 0 && currentMinutes == 0)?  1 : ((currentHours == 12 && currentMinutes == 0)? 23 : -1);

    if (ledIndex == -1)
      return;
    
    if (ledIndex  == 1)
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
    lcd.setCursor(5,0);
    char outputStr[6];
    snprintf(outputStr,6,"%02d:%02d",currentHours, currentMinutes);
    lcd.print(outputStr);
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

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

    FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, 144).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(80);

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
      Serial.println("aggiorno");
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
