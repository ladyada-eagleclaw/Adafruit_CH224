#include <Adafruit_CH224.h>

Adafruit_CH224 ch224;

void printActivity(bool active) {
  if (active) {
    Serial.println("active");
  } else {
    Serial.println("inactive");
  }
}

void printAvailability(bool available) {
  if (available) {
    Serial.println("available");
  } else {
    Serial.println("unavailable");
  }
}

void printVariant(ch224_variant_t variant) {
  if (variant == CH224_VARIANT_A) {
    Serial.println("CH224A");
  } else if (variant == CH224_VARIANT_Q) {
    Serial.println("CH224Q");
  } else {
    Serial.println("unknown");
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit CH224 test");

  if (!ch224.begin()) {
    Serial.println("Could not find CH224. Check wiring and I2C address.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("CH224 found");
  Serial.print("Variant: ");
  printVariant(ch224.getVariant());
  Serial.print("I2C address: 0x");
  Serial.println(ch224.getI2CAddress(), HEX);
  Serial.print("PPS control: ");
  printAvailability(ch224.supportsPPS());
  Serial.print("AVS control: ");
  printAvailability(ch224.supportsAVS());
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

  Serial.println("Protocol activity:");
  Serial.print("  BC: ");
  printActivity(status & CH224_PROTOCOL_BC);
  Serial.print("  QC 2.0: ");
  printActivity(status & CH224_PROTOCOL_QC2);
  Serial.print("  QC 3.0: ");
  printActivity(status & CH224_PROTOCOL_QC3);
  Serial.print("  USB PD: ");
  printActivity(status & CH224_PROTOCOL_PD);
  Serial.print("  EPR contract: ");
  printActivity(status & CH224_PROTOCOL_EPR);

  Serial.println("Power mode availability:");
  Serial.print("  Fixed PD PDOs: ");
  printAvailability(ch224.isVoltageAvailable(CH224_VOLTAGE_5V));
  Serial.print("  PPS APDO reported: ");
  printAvailability(ch224.isPPSAvailable());
  Serial.print("  EPR source: ");
  printAvailability(status & CH224_PROTOCOL_EPR_AVAILABLE);
  Serial.print("  AVS source reported: ");
  printAvailability(ch224.isAVSAvailable());

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
