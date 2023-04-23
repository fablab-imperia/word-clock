#include <Arduino.h>
 #include <SPI.h>
//For RTC support
#include "RTClib.h"
// Generic I2C support
#include <Wire.h>

#define UPDATE_PERIOD_MINUTES 1

RTC_DS1307 rtc;

//Global array whose elements are ids of leds to light up to create the words we need.
//List of leds is terminated by -1 (invalid value)
char ledsToLightUp[144];

char ouputBuffer[144];


char charMatrix[144] = 
{
  '!','[','M','E','Z','Z','A','N','O','T','T','E', 
  ']','M','E','Z','Z','O','G','I','O','R','N','O',
  'S','O','N','O','E','^','U','N','A','V','L','E',
  'D','U','E','T','R','E','C','I','N','Q','U','E',
  'Q','U','A','T','T','R','O','S','E','T','T','E',
  'S','E','I','O','T','T','O','D','I','E','C','I',
  'N','O','V','E','U','N','D','I','C','I','U','E',
  'M','E','N','O','Q','U','A','R','A','N','T','A',
  'D','I','E','C','I','U','N','M','E','Z','Z','A',
  'V','E','N','T','I','C','T','R','E','N','T','A',
  'M','C','I','N','Q','U','A','N','T','A','G','S',
  'Q','U','A','R','T','O','C','I','N','Q','U','E'
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
  "E CINQUE",
  "E DIECI",
  "E UN QUARTO",
  "E VENTI",
  "E VENTI CINQUE",
  "E MEZZA",
  "E TRENTA CINQUE",
  "MENO VENTI",
  "MENO UN QUARTO",
  "MENO DIECI",
  "MENO CINQUE"
};

 const char* hourTemplates[] = {
  "! $H $M",
  "! $H $M",
  "SONO $HH $M",
  "SONO $ $M",
  "SONO $H $M",
  "SONO $H $M",
  "SONO $H $M",
  "SONO $H $M",
  "SONO $H $M",
  "SONO $H $M",
  "SONO $H $M",
  "SONO $H $M"
};


char ledArray[144];

unsigned int lastTimeInMinutes = 0;



void currentTimeAsWords(char* outputBuffer, int hours, const int minutes) 
{
  //clear buffer
  memset(outputBuffer,'\0',144);

  //convert minutes to nearest multiple of 5
  int indexForMinutes = (minutes / 5);
  const char* minutesComponentInWords = minutesInWords[indexForMinutes];   

  const char* hoursComponentInWords = NULL;

  // If minutes are more than 35 we must increase hours
  int fixedHours = (minutes > 35)? (hours + 1) % 24 : hours; 
  hoursComponentInWords = hoursInWords[fixedHours];

  int indexForTmpl = fixedHours % 12;
  const char* tmpl = hourTemplates[indexForTmpl];

  const char* tmplPointer = tmpl;
  char* outputBufferPointer = outputBuffer;

  while ( (*tmplPointer) != '\0') 
  {
     //if char in template string is not special char
     if ((*tmplPointer) != '$') 
     {
        //copy char into output buffer
        (*outputBufferPointer) = (*tmplPointer);
        //move on both pointers
        outputBufferPointer++;
        tmplPointer++;
     } 
     else 
     {
      //When a special char is found, we must check if its next is M or H to expand it
      tmplPointer++;
      const char* stringToCopyPointer = NULL;
      //point to the right string to copy
      if ((*tmplPointer) == 'M')
      {
         stringToCopyPointer = minutesComponentInWords;      
      } 
      else 
      {
        stringToCopyPointer = hoursComponentInWords;
      }
      //In any case copy each char
      while((*stringToCopyPointer) != '\0')
      {
        (*outputBufferPointer) = (*stringToCopyPointer);
        outputBufferPointer++;
        stringToCopyPointer++;
      }
      //move tmplPointer to next char
      tmplPointer++;
     }
  }
}

void fillLedsArrayNeededToWriteTime(char* timeInWords, char* ledArray, char* charMatrix)
{
  int ledArrayIndex = 0;
  //delimiter between words
  const char delimiter = ' ';
  //reset the global array
  memset(ledArray,-1,144);
  
  char* 
  
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
    unsigned int currentTimeInMinutes = currentHours*60 + currentMinutes;
    if ((lastTimeInMinutes == 0) || currentTimeInMinutes == lastTimeInMinutes + UPDATE_PERIOD_MINUTES) 
    {
      currentTimeAsWords(ouputBuffer, currentHours, currentMinutes);
      lastTimeInMinutes = currentTimeInMinutes;
    
      Serial.print("In words: ");
      Serial.println(ouputBuffer);

      Serial.print(currentHours);
      Serial.print(":");
      Serial.println(currentMinutes);

      fillLedsArrayNeededToWriteTime("SONO LE SETTE E VENTI CINQUE",ledArray,charMatrix);
      size_t arrayIndex=0;
      while (ledArray[arrayIndex] != -1)
      {
        Serial.println((int)ledArray[arrayIndex]);
        arrayIndex++;
      }
      
      
    }

    delay(10000);
}
