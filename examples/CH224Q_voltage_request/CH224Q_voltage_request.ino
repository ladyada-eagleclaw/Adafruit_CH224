#include <Adafruit_CH224.h>

Adafruit_CH224Q ch224;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit CH224Q programmable voltage example");

  if (!ch224.begin()) {
    Serial.println("Could not find a CH224Q. Check the variant and wiring.");
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

  // Programmable requests return false unless the source advertises the
  // matching PPS or AVS capability.
  // ch224.setPPSVoltage(5.0);
  // ch224.setAVSVoltage(15.0);

  Serial.println("Requested 5 V");
}

void loop() { delay(1000); }
