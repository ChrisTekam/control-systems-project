#ifndef LOGGER_H
#define LOGGER_H
#include <Arduino.h>

/* Prints one CSV row per control tick over Serial:
 * millis,setpoint,temperature,duty,pwm
 *
 * Paste the Serial Monitor/Plotter output straight into MATLAB/Excel -
 * this is also what you'll use to validate the physical system against
 * your Simulink model.
 */

class Logger {

  public:
  // This can be changed to 115200 if needed (make sure it matches the Serial Monitor)
  Logger (long baud = 9600);
  
  //Starts Serial and prints the header row.
  void begin();

  /* Duty is the 0.0 - 1.0 PID output; pwm is the actual analogErite() value
   * This is sent to the heater pin.
   */
  void logRow(unsigned long timeMs, float setpoint, float temperature,
       float duty, uint8_t pwm);

  private:
    long baud_;
};


#endif // LOGGER_H
