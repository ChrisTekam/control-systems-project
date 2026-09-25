#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H
 
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/* Non-blocking DS18B20 wrapper.
 * Requirement: Request & read temperature at 200ms/5Hz sample rate.
 * At 9-bit resolution a conversion takes ~94 ms, so it is always finished
 * by the time the next tick arrives.
 */
 
class TempSensor {
public:
  // resolutionBits: 9 - 12. Use 9 (0.5 C, ~94 ms) or 10 (0.25 C, ~188 ms).
  // If we use 11 or 12, the concersiontime will exceed the 200ms threshold.
  TempSensor(uint8_t pin, uint8_t resolutionBits = 9); // <- Change the resolution here 
 
  // Starts the bus, finds the first sensor, applies the resolution and
  // switches to async mode. Returns false if no sensor is present.
  bool begin();
 
  // Tells the sensor to take a measurement. Returns immediately.
  void startConversion();
 
  // Returns the last completed measurement in degrees C.
  // On a bad read it returns the previous good value and clears isValid().
  float read();
 
  // False if the last read was disconnected or outside the plausible range.
  bool isValid() const { return valid_; }
 
  // True if a sensor was found by begin().
  bool isPresent() const { return present_; }
 
  // Worst-case conversion time in ms for the configured resolution.
  uint16_t conversionTimeMs() const;
 
private:
  static const float kMinPlausibleC;
  static const float kMaxPlausibleC;
 
  OneWire            oneWire_;
  DallasTemperature  sensors_;
  DeviceAddress      addr_;
  uint8_t            resBits_;
  float              lastTemp_;
  bool               valid_;
  bool               present_;
};

#endif  // TEMP_SENSOR_H
