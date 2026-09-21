#include "Logger.h"

/* Fairly short code, since it's just to check what's happening 
 * during a run.
 */
 
Logger::Logger(long baud) : baud_(baud) {}
 
void Logger::begin() {
  Serial.begin(baud_);
  Serial.println(F("millis,setpoint,temperature,duty,pwm"));
}

/* The (F("...")) string above is to keep that literal in the flash memory.
 * It saves on the Arduino Uno's RAM.
 */
 
void Logger::logRow(unsigned long timeMs, float setpoint, float temperature,
                     float duty, uint8_t pwm) {
  Serial.print(timeMs);         //Exact instant the control loop sampled the sensor.
  Serial.print(',');
  Serial.print(setpoint, 2);
  Serial.print(',');
  Serial.print(temperature, 2);
  Serial.print(',');
  Serial.print(duty, 3);
  Serial.print(',');
  Serial.println(pwm);
}
