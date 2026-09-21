#include "Heater.h"

Heater::Heater(uint8_t pin) : pin_(pin) {}

void Heater::begin() {
  pinMode(pin_, OUTPUT);
  off();                     // Safety Requirement
}

void Heater::off() {
  analogWrite(pin_, 255);    // HIGH (255) = off, per the NPN inversion
}

void Heater::setDuty(float duty) {
  if (duty < 0.0f) duty = 0.0f;
  if (duty > 1.0f) duty = 1.0f;

  uint8_t pwm = (uint8_t)(duty * 255.0f + 0.5f);  // round, don't truncate
  analogWrite(pin_, 255 - pwm);                   // invert: LOW = on
}
