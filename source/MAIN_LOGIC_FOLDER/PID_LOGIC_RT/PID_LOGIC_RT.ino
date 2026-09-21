/*
 * Notes in case i need them!
 * You assign pins when you create the objects in this main file.
 */

// ----- Include the header files -----
#include "TempSensor.h"
#include "Heater.h"
#include "PIDController.h"
#include "Logger.h"

// ----- Pin Assignments ----- (Update to match the wiring!)
const uint8_t ONE_WIRE_PIN = 2;   // DS18B20 data line
const uint8_t HEATER_PIN   = 9;   // to 1k base resistor of the 2N3904

// ----- Timing -----
const unsigned long TICK_MS = 200;   // 5 Hz control loop

// ----- PID Gains ----- ( Currently placeholders; change after simulating)
const float KP = 1.0f;
const float KI = 0.0f;
const float KD = 0.0f;

// ----- Setpoint & Safety -----
float setpoint = 30.0f;              // deg C - change as needed
const float SAFETY_MARGIN_C = 15.0f; // cutoff at setpoint + this

// ----- Objects -----
TempSensor    sensor(ONE_WIRE_PIN, /*resolutionBits=*/9);
Heater        heater(HEATER_PIN);
PIDController pid(KP, KI, KD, TICK_MS / 1000.0f);
Logger        logger(9600);

unsigned long lastTick = 0;


void setup() {
  // Run a known safe state before everything else runs (Item #6)
  heater.begin();
  logger.begin();

  if (!sensor.begin()){
    Serial.println (F("FATAL: DS18B20 not found. Check wiring."));
    while (true) {heater.off();
    // The heater stays off.
    // Now the controller won't chase a temperature it cannot measure.
    }
  }

  // Start the PID logic.
  pid.begin();
  sensor.startConversion(); // Start the first reading/measurement
  lastTick = millis();
  
} // End of Setup function.

void loop() {
  unsigned long now = millis();
  if (now - lastTick < TICK_MS) {
    return;   // not time for the next sample yet - loop() just spins
  }
  lastTick += TICK_MS;

  // 1) Sensor reading: collect the conversion requested last tick.
  float temperature = sensor.read();

  // 2-4) Safety cutoff overrides everything else.
  bool safe = sensor.isValid() && (temperature <= setpoint + SAFETY_MARGIN_C);

  float duty = 0.0f;
  uint8_t pwm = 255;   // 255 = off, matches Heater::off()'s inverted logic

  if (!safe) {
    heater.off();
    pid.reset();        // don't let the integral wind up while cut off
  } else {
    duty = pid.compute(setpoint, temperature);
    heater.setDuty(duty);
    pwm = (uint8_t)(duty * 255.0f + 0.5f);
    pwm = 255 - pwm;    // mirror the inversion for logging purposes only
  }

  // 5) Data logging
  logger.logRow(now, setpoint, temperature, duty, pwm);

  // Start the next conversion so it's ready by the next tick.
  sensor.startConversion();

} // End of the Loop function.
