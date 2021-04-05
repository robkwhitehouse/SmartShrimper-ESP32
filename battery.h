/*
A simple class to monitor boat battery voltage.
A boat (leisure) battery is a lead-acid battery with a nominal
voltage of 12v. In use, the battery terminal voltage can
actually vary between about 16v (when both fully charged and still being 
charged) and  11v when the battery is totally flat
So the battery +ve terminal is connected to an Arduino Analog input in
(supplied in the constructor) via an external voltage divider to bring
the voltage range within spec for the Arduino max voltage.
For an ESP-32 this is 3.3v. An voltage divider of 5:1 would thus
bring the maximum voltage (16) down to 3.2
For an ATMEL device this is 5v. The voltage divider would need to be 4:1
giving a maximum voltage of 4v
*/

#include "ESPADCfix.h"

 class Battery {
  private:
  int _pin;
  #ifdef ESP32 //3.3v device with 5:1 divider
  const float voltageConversionConstant = (3.2/3.3) * 4095 / 16; //12 bit ADC
  #else //5v device with 4:1 divider
  const float voltageConversionConstant = (4/5) * 1023 / 16; //10 bit ADC
  #endif
  public:
  //Constructor
  Battery(int pin) { _pin = pin; }
  
  void begin() {
    pinMode(_pin,INPUT);
  }
  
  float voltage() {
    return( ADC_LUT[analogRead(_pin)] / voltageConversionConstant );
  }
  
 };
