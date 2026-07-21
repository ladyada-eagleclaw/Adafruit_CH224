#include <Adafruit_CH224.h>

Adafruit_CH224 ch224;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit CH224 test");

  // CH224A uses the default 0x23 address. For CH224Q, use:
  // ch224.begin(CH224Q_I2CADDR_DEFAULT)
  if (!ch224.begin()) {
    Serial.println("Could not find CH224. Check wiring and I2C address.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("CH224 found");
}

void loop() {
  uint8_t status = 0;
  if (!ch224.getStatus(&status)) {
    Serial.println("Could not read CH224 status");
    delay(1000);
    return;
  }

  Serial.print("Status: 0x");
  Serial.println(status, HEX);

  Serial.print("USB PD active: ");
  Serial.println((status & CH224_PROTOCOL_PD) ? "yes" : "no");

  Serial.print("EPR available: ");
  Serial.println((status & CH224_PROTOCOL_EPR_AVAILABLE) ? "yes" : "no");

  Serial.print("AVS available: ");
  Serial.println((status & CH224_PROTOCOL_AVS_AVAILABLE) ? "yes" : "no");

  float amps = 0.0;
  if (ch224.getMaxCurrent(&amps)) {
    Serial.print("Maximum negotiated current: ");
    Serial.print(amps, 2);
    Serial.println(" A");
  } else {
    Serial.println("Maximum negotiated current is unavailable");
  }

  const ch224_voltage_t fixedVoltages[] = {
      CH224_VOLTAGE_5V,  CH224_VOLTAGE_9V,  CH224_VOLTAGE_12V,
      CH224_VOLTAGE_15V, CH224_VOLTAGE_20V, CH224_VOLTAGE_28V};
  const uint8_t voltageValues[] = {5, 9, 12, 15, 20, 28};

  Serial.println("Available fixed PD voltages:");
  for (uint8_t index = 0; index < 6; index++) {
    Serial.print(voltageValues[index]);
    Serial.print(" V: ");

    float availableAmps = 0.0;
    if (ch224.getAvailableCurrent(fixedVoltages[index], &availableAmps)) {
      Serial.print("available at ");
      Serial.print(availableAmps, 2);
      Serial.println(" A");
    } else {
      Serial.println("unavailable");
    }
  }

  Serial.println();
  delay(1000);
}
