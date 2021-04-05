/*
 * Implementation of the various methods associted with the various hardware interfaces
 */
#include "hardware.h"


//Morse Flasher Class methods
/*
MorseFlasher::MorseFlasher( LED *Led ) {
  myLED = Led;
  
  morseMap['1'] = ".----";
  morseMap['2'] = "..---";
  morseMap['3'] = "...--";
  morseMap['4'] = "....-";
  morseMap['5'] = ".....";
  morseMap['6'] = "-....";
  morseMap['7'] = "--...";
  morseMap['8'] = "---..";
  morseMap['9'] = "----.";
  morseMap['0'] = "-----";
  morseMap['A'] = ".-";
  morseMap['B'] = "-...";
  morseMap['C'] = "-.-.";
  morseMap['D'] = "-..";
  morseMap['E'] = ".";
  morseMap['F'] = "..-.";
  morseMap['G'] = "--.";
  morseMap['H'] = "....";
  morseMap['I'] = "..";
  morseMap['J'] = ".---";
  morseMap['K'] = "-.-";
  morseMap['L'] = ".-..";
  morseMap['M'] = "--";
  morseMap['N'] = "-.";
  morseMap['O'] = "---";
  morseMap['P'] = ".--.";
  morseMap['Q'] = "--.-";
  morseMap['R'] = ".-.";
  morseMap['S'] = "...";
  morseMap['T'] = "-";
  morseMap['U'] = "..-";
  morseMap['V'] = "...-";
  morseMap['W'] = ".--";
  morseMap['X'] = "-..-";
  morseMap['Y'] = "-.--";
  morseMap['Z'] = "--..";
}


void MorseFlasher::flashChar( char myChar ) {
  unsigned chardelay;
  String morseString = morseMap[myChar];
  for (int i=0; i < morseString.length(); i++ )
  { if ( morseString[i] == '.' ) chardelay = 250;
    else chardelay = 500;
    myLED->on();
    delay(chardelay);
    myLED->off();
    delay( 500 );
  }

}

void MorseFlasher::flashString( String myString ) {
   for (int i=0; i < myString.length(); i++) {
       flashChar(myString[i]);
       delay(500);
   }
}
*/
