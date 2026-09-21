#ifndef HEATER_H
#define HEATER_H

#include<Arduino.h>

/* Has to put the PID output into a PWM value, as well as do the inversion for the NPN transistor
 */

class Heater {
public:
  Heater(uint8_t pin);
  void begin();          // forces heater off — call before anything else in the setup
  void setDuty(float duty);  // 0.0 = off, 1.0 = full power
  void off();
private:
  uint8_t pin_;
};

#endif // HEATER_H
