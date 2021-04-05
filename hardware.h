/**
 * A set of classes to provide interfaces to the various hardware devices 
 */

 
//#include <arduino.h>
/* You might need to tweak the constants defined here depending 
 *  upon your own battery and cooler element etc.
 */

// battery internal resistance (ohms)
#define BATT_RESISTANCE 0.0625 //typical value for a hybrid/marine battery
//#define BATT_RESISTANCE 0.02 //typical value for a car starter battery

#define BATTOK 12.7 //any open circuit voltage above 12.7 is totally fine
#define PELTIER_CURRENT 4.0 //Amps - typical for a 50W TEC




 
 /*****
  * Will flash a character or String in morse code on the LED (supplied as a constructor parameter)
 

#ifndef MORSE_FLASHER_H
#define MORSE_FLASHER_H
#include <LED.h>
#include <ArduinoMap.h>

class MorseFlasher {
  public:
  MorseFlasher( LED * );
  void flashChar(char);
  void flashString(String);
  private:
  LED *myLED;
  CreateMap(morseMap, char, String, 36);
};

*/
