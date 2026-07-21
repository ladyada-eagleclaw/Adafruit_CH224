#include <Adafruit_CH224.h>

Adafruit_CH224A ch224;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit CH224 voltage request example");

  if (!ch224.begin()) {
    Serial.println("Could not find CH224. Check wiring and I2C address.");
    while (1) {
      delay(10);
    }
  }

  // Start with a safe 5 V fixed request. Change this only when the connected
  // load can safely accept the higher requested voltage.
  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    Serial.println("Fixed voltage request failed");
    while (1) {
      delay(10);
    }
  }

  // Other fixed-voltage example:
  // ch224.setVoltage(CH224_VOLTAGE_9V);
  // PPS and AVS requests use the Adafruit_CH224Q subclass instead.

  Serial.println("Requested 5 V");
}

void loop() {
  float amps = 0.0;
  if (ch224.getMaxCurrent(&amps)) {
    Serial.print("Maximum negotiated current: ");
    Serial.print(amps, 2);
    Serial.println(" A");
  }

  delay(1000);
}
