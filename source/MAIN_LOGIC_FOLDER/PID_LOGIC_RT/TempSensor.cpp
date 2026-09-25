#include "TempSensor.h"

// Interval of accepted temperatures. Anything outside these values is not seen as a temperature.
// Can be changed according to preference.
const float TempSensor::kMinPlausibleC = -10.0f;
const float TempSensor::kMaxPlausibleC = 125.0f;

TempSensor::TempSensor(uint8_t pin, uint8_t resolutionBits)
  : oneWire_(pin),
    sensors_(&oneWire_),
    resBits_(resolutionBits),
    lastTemp_(NAN),
    valid_(false),
    present_(false) {
  if (resBits_ < 9)  resBits_ = 9;
  if (resBits_ > 12) resBits_ = 12;
}

bool TempSensor::begin() {
  sensors_.begin();

  // Grab the address of the first device so we can address it directly;
  // this is faster than the "skip ROM" broadcast read.
  present_ = sensors_.getAddress(addr_, 0);
  if (!present_) return false;

  sensors_.setResolution(addr_, resBits_);

  // The key line: requestTemperatures() will no longer block.
  sensors_.setWaitForConversion(false);

  return true;
}

//Reads the temperature 
void TempSensor::startConversion() {
  if (!present_) return;
  sensors_.requestTemperatures();
}


//Returns the last valid temperature reading
float TempSensor::read() {
  if (!present_) {
    valid_ = false;
    return lastTemp_;
  }

  float t = sensors_.getTempC(addr_);

  // DEVICE_DISCONNECTED_C is -127.0; also reject 85.0, the power-on
  // default the DS18B20 returns if it is read before a conversion finishes.
  bool ok = (t != DEVICE_DISCONNECTED_C) &&
            (t >= kMinPlausibleC) && (t <= kMaxPlausibleC) &&
            (t != 85.0f);
  // 85 is rejected becuase that's the DS18B20's power on value. It could affect the control loop.

  if (ok) {
    lastTemp_ = t;
    valid_    = true;
  } else {
    valid_ = false;  // lastTemp_ is left at the previous good value
  }

  return lastTemp_;
}

// The max conversion times of the sensor (incase we want to tweak the resolution)
uint16_t TempSensor::conversionTimeMs() const {
  switch (resBits_) {
    case 9:  return 94;
    case 10: return 188;
    case 11: return 375;
    default: return 750;
  }
}
