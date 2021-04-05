
/*
Smart Shrimper - monitors various boat sensors and reports via Bluetooth to Android app

Uses a simple real-time scheduler to schedule various tasks

 V1.0 - just sends engine RPM data from a hall effect sensor (KY03) 
*/

#define VERSION 1.0

#ifndef ESP32
  #error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif


#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include <ESP32TimerInterrupt.h>
#include <ESP32_ISR_Timer.h>
#include "scheduler.h"
#include "BluetoothSerial.h"
#include <EEPROM.h>
#include "battery.h"
#include "FridgeRelay.h"
#include "hardware.h"



#define MICROS_PER_MINUTE 60000000
#define OUTPUT_PERIOD 200 //milliseconds between OUTPUT events
#define TICK_PERIOD 1 //millis
//#define STAB_TIMER_PERIOD 1000
#define ON 1
#define OFF 0

volatile unsigned long ticks = 0;
volatile int RPM = 0;
volatile boolean engineRunning = false;

//Hall sensor (RPM counter) interrupt related globals
const byte KY_03_PIN = 34;
volatile long revTime;//logs time of last pulse from KY-003

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

//Create our global static scheduler object
Scheduler scheduler;

//Create an object to model the battery
#define BATTV_PIN A0
Battery battery(BATTV_PIN);


//External on/off switch for the fridge
const byte FRIDGE_SWITCH=39;
//Create a FridgeRelay object
const byte RELAY_PIN=35;
FridgeRelay fridgeRelay(RELAY_PIN);

//Create a Bluetooth Serial stream
BluetoothSerial SerialBT;

//Tacho calibration globals
String calString("");
float calibFactor = 1.0;
unsigned TachoCalEEPROMindex = 0;
String ident = String("Tacho Cal:");

// Init ESP32 timer 1
ESP32Timer ITimer1(TICK_PERIOD);

//Timer interrupt handler
void IRAM_ATTR tickHandler() {
  portENTER_CRITICAL_ISR(&mux);
  ticks++;
  portEXIT_CRITICAL_ISR(&mux);
}

//Hall  sensor KY03 (rev counter) interrupt handler
void IRAM_ATTR handleKY03Interrupt() {
  portENTER_CRITICAL_ISR(&mux);
  long now = micros();
  long micros_per_rev = now - revTime;
  engineRunning = true;
  
  //Calc current RPM and apply a bit of smoothing
  
  RPM = ((2 * RPM) + (MICROS_PER_MINUTE / micros_per_rev )) / 3;
  
  revTime = now;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {

  Serial.begin(115200);
  SerialBT.begin("Smart Shrimper");
  delay(2000);
  Serial.print("Smart Shrimper Version ");
  Serial.println(VERSION);
  if (!EEPROM.begin(100)) {
    Serial.println("Error: could not initialise EEPROM segment");
    delay(5000);
  }

  battery.begin();
  fridgeRelay.begin();

  pinMode(KY_03_PIN, INPUT_PULLUP);

    //Check to see if we have previously saved calibration data in EEPROM
  String existing_id;
  int n = EPreadString(TachoCalEEPROMindex, existing_id);
  Serial.print("Reading existing ID -- ");
  if (n) Serial.println(existing_id);
  else Serial.println("None found");
  if ( existing_id.substring(0,ident.length()).compareTo(ident) == 0 ) { 
    //retrieve calibration data from EEPROM
    TachoCalEEPROMindex += ident.length();
    String cf;
    n = EPreadString(TachoCalEEPROMindex,cf);
    if (n) calibFactor = cf.toFloat(); 
    Serial.print("Stored calibration factor: ");
    Serial.println(calibFactor);
  }
  else {
    Serial.print("Storing default calibration factor: ");
    Serial.println(calibFactor);
    String temp = ident + "1.0";
    EPwriteString(TachoCalEEPROMindex,temp);
  }

  // Add all our tasks to the scheduler

  scheduler.add(&output, OUTPUT_PERIOD); //Report via Bluetooth every 200ms
  scheduler.add(&checkRPMsensor, 200);//Check engine revs every 200ms
  scheduler.add(&checkBattery,10000);//Check battery every 10 secs
  scheduler.add(&checkSwitch,10000);//Check fridge switch every 10 secs
  scheduler.add(&voltageTimerTick, stabilizationTimer.STAB_TIMER_PERIOD);

  // Attatch the Interrupt service routines
   
  //set up KY_03 interrupt
  attachInterrupt(digitalPinToInterrupt(KY_03_PIN), handleKY03Interrupt, RISING);
  
  //set up (and start) timer interrupt
  ITimer1.attachInterruptInterval(TICK_PERIOD * 1000, tickHandler);
 
}


unsigned long lastTick = 0;
void loop() {
  int i;

  if ( ticks > lastTick ) {
    //Run scheduled tasks
    scheduler.run(ticks * TICK_PERIOD);
    lastTick = ticks;
  }
  //Run anything else
    //Calibration - read a factor from Bluetooth terminal - if available. 

  if (SerialBT.available() > 0) {
    char c=SerialBT.read();
    if(c=='c') {
        calString = "";
        while ( SerialBT.available() > 0 ) {
          c = SerialBT.read();
          if (c == '\n' || c == '\r' ) break;
          calString += c;
        }
        if ( calString.toFloat() != 0 ) calibFactor = calString.toFloat();
        Serial.println(calibFactor);
        SerialBT.println(calibFactor);

        //Save the calibration factor to EEPROM (actually an area of flash)
        String temp = ident + calString;
        EPwriteString(TachoCalEEPROMindex,temp);
        Serial.print(temp.length());
        Serial.println(" bytes written to EEPROM");
        
    }
  }
}

/*
   Real time task definitons
*/

void checkRPMsensor() {
  if ( engineRunning ) {
    if ((micros() - revTime) > 500000) { // no pulse for > 500ms engine has probably stopped - decrement RPM
      RPM = (RPM * 0.8)- 200;
      if (RPM < 0) {
        RPM = 0;
        engineRunning = false;
      }
    }
  } else RPM = 0;
}

void checkBattery() {
  if (stabilizationTimer.isExpired()) {
    if (battery.voltage() < BATTOK) {
      fridgeRelay.enabled = false;
    } else {
      fridgeRelay.enabled = true;
    }
  }
  
}

void checkSwitch() {
  int switchState = digitalRead(FRIDGE_SWITCH);
  if (switchState == ON && fridgeRelay.enabled) fridgeRelay.turnOn();
  else fridgeRelay.turnOff();
}

void voltageTimerTick() {
  stabilizationTimer.tickDown();
}
void output() {
  
  float bv = battery.voltage();
  int rpm = RPM * calibFactor;
  
  //output to standard Serial port
    Serial.print("RPM = ");
    Serial.print(rpm);
    Serial.print(", Voltage = ");
    Serial.println(bv);
    
  //output to Bluetooth in JSON format  
    SerialBT.print("{ \"RPM\" : ");
    SerialBT.print(rpm);
    SerialBT.print(",  \"Voltage\" : ");
    SerialBT.print(bv);
    SerialBT.print(", \"Coolbox Status\" : ");
    SerialBT.print(fridgeRelay.status()?"true":"false");
    SerialBT.println(" }");
}



int EPwriteString(char add,String data)
{
  int _size;
  int i;

  _size=data.length();
  
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
  delay(200);
  return(_size);
}

int EPreadString(unsigned add, String &data)
{
  int i;
  int len=0;
  unsigned char k;

  len=0;
  k=EEPROM.read(add++);
  while(k != '\0' && len<500)   //Read until null character
  {    
    data += char(k);
    k=EEPROM.read(add+len);    
    len++;
  }
  data += '\0';
  return(len);
}
