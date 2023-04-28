#include <Arduino.h>
 #include <SPI.h>
//For RTC support
#include "RTClib.h"
// Generic I2C support
#include <Wire.h>

#include "FastLED.h"

#define UPDATE_PERIOD_MINUTES 1
#define LED_PIN  5   

#define SINGULAR_VERBAL_FORM 0
#define PLURAL_VERBAL_FORM 1

CRGB leds[144];

RTC_DS1307 rtc;


char ouputBuffer[80];


char charMatrix[145] = 
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
  'Q','U','A','R','T','O','C','I','N','Q','U','E','\0'
};

const char* verbalForms[] = {
  "! $H $M",
  "SONO $H $M"
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

 const int hourTemplates[] = {
  SINGULAR_VERBAL_FORM,
  SINGULAR_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM,
  PLURAL_VERBAL_FORM
};


//int ledArray[144];

unsigned int lastTimeInMinutes = 0;



void currentTimeAsWords(char* outputBuffer, int hours, const int minutes) 
{
  //clear buffer
  memset(outputBuffer,'\0',80);

  //convert minutes to nearest multiple of 5
  int indexForMinutes = (minutes / 5);
  const char* minutesComponentInWords = minutesInWords[indexForMinutes];   

  const char* hoursComponentInWords = NULL;

  // If minutes are more than 35 we must increase hours
  int fixedHours = (minutes > 35)? (hours + 1) % 24 : hours; 
  hoursComponentInWords = hoursInWords[fixedHours];

  int indexForTmpl = fixedHours % 12;
  int tmplIndex = hourTemplates[indexForTmpl];

  const char* tmplPointer = verbalForms[tmplIndex];
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

void fillLedsArrayNeededToWriteTime(const char* timeInWords, char* charMatrix)
{
  //delimiter between words
  const char delimiter = ' ';
  //reset the global array
  const char* tokenStringPointer = timeInWords;
  const char* tokenStart = tokenStringPointer; 
  const char* matrixScanner = charMatrix; 
  int ledArrayIndex= 0;

  char currentToken[50];

  while (true) 
  {
    if ((*tokenStringPointer) !=  delimiter && (*tokenStringPointer) != '\0') 
    {
      tokenStringPointer++;
    } 
    else 
    {
       size_t numberOfChars = tokenStringPointer - tokenStart;
       for (size_t i = 0; i < numberOfChars; i++)
       {
         currentToken[i] = (*tokenStart);
         tokenStart++;
       }
       currentToken[numberOfChars]= '\0';

       matrixScanner = strstr(matrixScanner,currentToken);
       if (matrixScanner != NULL)
       {
          int indexOfFirstChar = matrixScanner - charMatrix;

          for (size_t i = 0; i < numberOfChars; i++)
          {
            leds[i]=  CRGB::White;
          }
       }

       if ((*tokenStringPointer) == '\0')
       {
          FastLED.show();
          break;
       }  
       else 
       {
          tokenStringPointer++;
          tokenStart = tokenStringPointer;
       }
    }
  }
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
    FastLED.setBrightness(80);
}

// the loop function runs over and over again forever
void loop() {
    DateTime now = rtc.now();
    int currentHours = now.hour();
    int currentMinutes = now.minute();
    unsigned int currentTimeInMinutes = currentHours*60 + currentMinutes;
    if ((lastTimeInMinutes == 0) || currentTimeInMinutes == (lastTimeInMinutes + UPDATE_PERIOD_MINUTES) % 1440 ) 
    {
      currentTimeAsWords(ouputBuffer, currentHours, currentMinutes);
      lastTimeInMinutes = currentTimeInMinutes;
    
      Serial.print("In words: ");
      Serial.println(ouputBuffer);

      Serial.print(currentHours);
      Serial.print(":");
      Serial.println(currentMinutes);

      FastLED.clear(true);
      fillLedsArrayNeededToWriteTime(ouputBuffer,charMatrix);

    }

    delay(10000);
}
