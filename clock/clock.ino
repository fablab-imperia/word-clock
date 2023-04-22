#include <Arduino.h>

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

const char* hours[] = {
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

const char* minutes[] = {
  "",
  "E CINQUE",
  "E DIECI",
  "E UN QUARTO",
  "E VENTI",
  "E VENTICINQUE",
  "E MEZZA",
  "E TRENTA CINQUE",
  "MENO VENTI",
  "MENO UN QUARTO",
  "MENO DIECI",
  "MENO CINQUE"
};

const char* templates[] = {
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


bool ledArray[144];

void setLedArray() 
{
  
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}