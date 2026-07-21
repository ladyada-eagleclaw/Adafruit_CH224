#include <Adafruit_CH224.h>

Adafruit_CH224A ch224;
bool ch224Ready = false;

void clearOutputsAndHalt(const char *message) {
  Serial.println(message);

  if (ch224Ready) {
    if (ch224.setVoltage(CH224_VOLTAGE_5V)) {
      Serial.println("Returned CH224A request to 5 V");
    } else {
      Serial.println("Could not return CH224A request to 5 V");
    }
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

  Serial.println("CH224A hardware test 00_i2c_scan");

  if (!ch224.begin()) {
    clearOutputsAndHalt("CH224A begin failed");
  }
  ch224Ready = true;
  Serial.println("CH224A begin succeeded");

  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    clearOutputsAndHalt("Initial 5 V request failed");
  }
  Serial.println("Initial 5 V request succeeded");
  Serial.println();

  uint8_t deviceCount = 0;
  bool foundCH224AAddress = false;
  bool foundCH224QAlias = false;

  Serial.print("SDA level: ");
  if (digitalRead(SDA)) {
    Serial.println("HIGH");
  } else {
    Serial.println("LOW");
  }
  Serial.print("SCL level: ");
  if (digitalRead(SCL)) {
    Serial.println("HIGH");
  } else {
    Serial.println("LOW");
  }

  Serial.println("Scanning I2C addresses...");
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found device at 0x");
      if (address < 0x10) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      deviceCount++;

      if (address == CH224A_I2CADDR_DEFAULT) {
        foundCH224AAddress = true;
      }
      if (address == CH224Q_I2CADDR_DEFAULT) {
        foundCH224QAlias = true;
      }
    }
  }

  if (!foundCH224AAddress) {
    clearOutputsAndHalt("CH224A address 0x23 was not found");
  }
  Serial.println("CH224A address 0x23 found");

  if (foundCH224QAlias) {
    Serial.println("Address 0x22 also acknowledged; this is a known CH224A "
                   "alias, not proof of CH224Q silicon");
  }

  Serial.print("I2C scan succeeded, device count: ");
  Serial.println(deviceCount);

  Serial.println();
  Serial.println("00_i2c_scan passed");
}

void loop() { delay(1000); }
