#include <Adafruit_CH224.h>

Adafruit_CH224 ch224;

const ch224_voltage_t fixedVoltages[] = {
    CH224_VOLTAGE_5V,  CH224_VOLTAGE_9V,  CH224_VOLTAGE_12V,
    CH224_VOLTAGE_15V, CH224_VOLTAGE_20V, CH224_VOLTAGE_28V};
const uint8_t voltageValues[] = {5, 9, 12, 15, 20, 28};

void returnToFiveVoltsAndHalt(const char *message) {
  Serial.println(message);

  if (ch224.setVoltage(CH224_VOLTAGE_5V)) {
    Serial.println("Returned CH224 request to 5 V");
  } else {
    Serial.println("Could not return CH224 request to 5 V");
  }

  while (1) {
    delay(10);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit CH224 try all fixed voltages");
  Serial.println("Disconnect voltage-sensitive loads before running!");

  if (!ch224.begin()) {
    Serial.println("Could not find CH224. Check wiring and I2C address.");
    while (1) {
      delay(10);
    }
  }

  for (uint8_t index = 0; index < 6; index++) {
    float amps = 0.0;
    if (!ch224.getAvailableCurrent(fixedVoltages[index], &amps)) {
      Serial.print(voltageValues[index]);
      Serial.println(" V is unavailable");
      continue;
    }

    Serial.print("Requesting ");
    Serial.print(voltageValues[index]);
    Serial.print(" V at up to ");
    Serial.print(amps, 2);
    Serial.println(" A");

    if (!ch224.setVoltage(fixedVoltages[index])) {
      returnToFiveVoltsAndHalt("Voltage request failed");
    }

    delay(2000);
  }

  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    returnToFiveVoltsAndHalt("Final 5 V request failed");
  }

  Serial.println("Finished and returned CH224 request to 5 V");
}

void loop() { delay(1000); }
