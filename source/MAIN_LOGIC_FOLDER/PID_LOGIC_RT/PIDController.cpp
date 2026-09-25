#include "PIDController.h"

PIDController::PIDController(float kp, float ki, float kd, float Ts)
  : kp_(kp), ki_(ki), kd_(kd), Ts_(Ts),
    integral_(0.0f), prevMeasured_(0.0f), havePrevMeasured_(false) {}

void PIDController::begin() {
  reset();
}

void PIDController::reset() {
  integral_ = 0.0f;
  havePrevMeasured_ = false;  // next compute() won't attempt a derivative
}

void PIDController::setGains(float kp, float ki, float kd) {
  kp_ = kp;
  ki_ = ki;
  kd_ = kd;
}

float PIDController::compute(float setpoint, float measured) {
  float error = setpoint - measured;

  // --- Proportional ---
  float P = kp_ * error;

  // --- Integral (with clamping anti-windup) ---
  integral_ += ki_ * error * Ts_; // clamp I to [0,1] - anti windup
  if (integral_ > 1.0f) integral_ = 1.0f;
  if (integral_ < 0.0f) integral_ = 0.0f;
  float I = integral_;

  // --- Derivative on measurement ---
  float D = 0.0f;
  if (havePrevMeasured_) {
    D = -kd_ * (measured - prevMeasured_) / Ts_;
  }
  prevMeasured_ = measured;
  havePrevMeasured_ = true;

  // --- Combine and saturate output ---
  float u = P + I + D;
  if (u > 1.0f) u = 1.0f;
  if (u < 0.0f) u = 0.0f;

  return u;
}
