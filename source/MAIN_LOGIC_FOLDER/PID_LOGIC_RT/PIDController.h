#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <Arduino.h>

// The parameters can change depending on our preferences.
class PIDController {
  public:
  // Ts = sample time in seconds (0.2 for the 200 ms / 5 Hz loop).
  PIDController(float kp, float ki, float kd, float Ts);
  
  // Call this once in setup(), in case the gains are known before begin().
  void begin (); 

  // For one PID step.
  float compute(float setpoint,float measured); 

  //Reset
  void reset();

  // Incase we want to change the gains during runtime
  void setGains (float kp, float ki, float kd);

  private:
  float kp_, ki_, kd_;
  float Ts_;
  float integral_;
  float prevMeasured_;
  bool havePrevMeasured_;

};

#endif
