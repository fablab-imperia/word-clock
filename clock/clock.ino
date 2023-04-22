#include <Arduino.h>
 #include <SPI.h>
//For RTC support
#include "RTClib.h"
// Generic I2C support
#include <Wire.h>

#define UPDATE_PERIOD_MINUTES 2

RTC_DS1307 rtc;

//Global array whose elements are ids of leds to light up to create the words we need.
//List of leds is terminated by -1 (invalid value)
char ledsToLightUp[144];

char charMatrix[12][12] = 
{
  {'!','[','M','E','Z','Z','A','N','O','T','T','E'},
  {']','M','E','Z','Z','O','G','I','O','R','N','O'},
  {'S','O','N','O','E','^','U','N','A','V','L','E'},
  {'D','U','E','T','R','E','C','I','N','Q','U','E'},
  {'Q','U','A','T','T','R','O','S','E','T','T','E'},
  {'S','E','I','O','T','T','O','D','I','E','C','I'},
  {'N','O','V','E','U','N','D','I','C','I','U','E'},
  {'M','E','N','O','Q','U','A','R','A','N','T','A'},
  {'D','I','E','C','I','U','N','M','E','Z','Z','A'},
  {'V','E','N','T','I','C','T','R','E','N','T','A'},
  {'M','C','I','N','Q','U','A','N','T','A','G','S'},
  {'Q','U','A','R','T','O','C','I','N','Q','U','E'}
};

const char* hoursInWords[] = {
  "MEZZANOTTE",
  "^UNA",
  "LE DUE",
  "LE TRE",
  "LE QUATTRO",
  "LE CINQUE",
  "LE SEI",
  "LE SETTE",
  "LE OTTO",
  "LE NOVE",
  "LE DIECI",
  "LE UNDICI",
  "MEZZOGIORNO",
  "^UNA",
  "LE DUE",
  "LE TRE",
  "LE QUATTRO",
  "LE CINQUE",
  "LE SEI",
  "LE SETTE",
  "LE OTTO",
  "LE NOVE",
  "LE DIECI",
  "LE UNDICI"  
};

const char* minutesInWords[] = {
  "",
  "E UNO",
  "E DUE",
  "E TRE",
  "E QUATTRO",
  "E CINQUE",
  "E SEI",
  "E SETTE",
  "E OTTO",
  "E NOVE",
  "E DIECI",
  "E UNDICI",
  "E DODICI",
  "E TREDICI",
  "E QUATTORDICI",
  "E UN QUARTO",
  "E SEDICI",
  "E DICIASSETTE",
  "E DICIOTTO",
  "E DICIANNOVE",
  "E VENTI",
  "E VENTUNO",
  "E VENTI DUE",
  "E VENTI TRE",
  "E VENTI QUATTRO",
  "E VENTI CINQUE",
  "E VENTI SEI",
  "E VENTI SETTE",
  "E VENTOTTO",
  "E VENTI NOVE",
  "E MEZZA",
  "E TRENTUNO",
  "E TRENTA DUE",
  "E TRENTA TRE",
  "E TRENTA QUATTRO",
  "E TRENTA CINQUE",
  "E TRENTA SEI",
  "E TRENTA SETTE",
  "E TRENTOTTO",
  "E TRENTA NOVE",
  "MENO VENTI",
  "MENO DICIANNOVE",
  "MENO DICIOTTO",
  "MENO DICIASSETTE",
  "MENO SEDICI",
  "MENO UN QUARTO",
  "MENO QUATTORDICI",
  "MENO TREDICI",
  "MENO DODICI",
  "MENO UNDICI",
  "MENO DIECI",
  "MENO NOVE",
  "MENO OTTO",
  "MENO SETTE",
  "MENO SEI",
  "MENO CINQUE"
  "MENO QUATTRO",
  "MENO TRE",
  "MENO DUE",
  "MENO UNO"
};

const char* hourTemplates[] = {
  "! $ORE $MIN",
  "! $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "! $ORE $MIN",
  "! $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN",
  "SONO $ORE $MIN"
};


const char ledArray[144];

unsigned int lastTimeInMinutes = 0;

String currentTimeAsWords(const int hours, const int minutes) 
{
  String tmpl = String(hourTemplates[hours]);
  const char* minutesComponentInWords = minutesInWords[minutes];  
  
  const char* hoursComponentInWords = NULL;
  // If minutes are more than 35 we must increase hours
  int fixedHours = (minutes > 35)? hours + 1 : hours; 
  hoursComponentInWords = hoursInWords[fixedHours];

  tmpl.replace("$ORE", hoursComponentInWords);
  tmpl.replace("$MIN",minutesComponentInWords);

  return tmpl;
}

void fillLedsArrayNeededToWriteTime(const char* timeInWords, const char* ledArray, char matrix[12][12])
{
  
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
}

// the loop function runs over and over again forever
void loop() {
    DateTime now = rtc.now();
    int currentHours = now.hour();
    int currentMinutes = now.minute();
    int currentTimeInMinutes = currentHours*60 + currentMinutes;
    if ((lastTimeInMinutes == 0) || currentTimeInMinutes == lastTimeInMinutes + UPDATE_PERIOD_MINUTES) 
    {
      String currentTime = currentTimeAsWords(currentHours, currentMinutes);
      lastTimeInMinutes = currentTimeInMinutes;
      Serial.println(currentTime.c_str());
      Serial.print(currentHours);
      Serial.print(":");
      Serial.println(currentMinutes);
    }

    delay(10000);
}
