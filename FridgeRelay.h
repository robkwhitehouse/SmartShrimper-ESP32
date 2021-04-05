/*
 * Simple class to model an external relay that controls power
 * to a cool box. The essential behaviour is to provide a bit
 * of hysteresis because the battery voltage takes a few minutes
 * to settle after an on/off transition
 */

 class StabilizationTimer {
   private:
      long counter;
      const long threeMins = 180000;//milliseconds
   public:
      const int STAB_TIMER_PERIOD = 1000;//millis
      void reset() {
        counter = threeMins;
      }
      void tickDown() { 
        if (counter > 0 ) counter -= STAB_TIMER_PERIOD;
        else counter = 0;
      }
      boolean isExpired() {
        return(counter>0?false:true);
      }
 } stabilizationTimer;

 class FridgeRelay {
  private:

    byte _pin;
  public:
    FridgeRelay(byte pin)  { _pin = pin; }
    boolean enabled = false;
    void begin() 
      { pinMode(_pin,OUTPUT);
        digitalWrite(_pin, LOW);
      }
    void turnOn() { 
      digitalWrite(_pin, HIGH);
      stabilizationTimer.reset();
    }
    void turnOff() { 
      digitalWrite(_pin, LOW);
      stabilizationTimer.reset();
    }
    byte status() { return(digitalRead(_pin)); } 
 };
