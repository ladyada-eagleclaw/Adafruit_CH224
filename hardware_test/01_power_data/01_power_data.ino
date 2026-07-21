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

  Serial.println("CH224A hardware test 01_power_data");

  if (!ch224.begin()) {
    clearOutputsAndHalt("CH224A begin failed");
  }
  ch224Ready = true;
  Serial.println("CH224A begin succeeded");

  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    clearOutputsAndHalt("Initial 5 V request failed");
  }
  Serial.println("Initial 5 V request succeeded");

  if (ch224.getVariant() != CH224_VARIANT_A) {
    clearOutputsAndHalt("Driver did not identify CH224A silicon");
  }
  Serial.println("Driver identified CH224A silicon");

  if (ch224.getI2CAddress() != CH224A_I2CADDR_DEFAULT) {
    clearOutputsAndHalt("CH224A did not initialize at address 0x23");
  }
  Serial.println("CH224A initialized at I2C address 0x23");
  Serial.println();

  uint8_t powerData[CH224_POWER_DATA_LENGTH];
  if (!ch224.readPowerData(powerData)) {
    clearOutputsAndHalt("Power data read failed");
  }
  Serial.println("Power data read succeeded");

  for (uint8_t index = 0; index < CH224_POWER_DATA_LENGTH; index++) {
    if ((index % 4) == 0) {
      Serial.print("0x");
      Serial.print(CH224_POWER_DATA_REGISTER + index, HEX);
      Serial.print(": ");
    }

    if (powerData[index] < 0x10) {
      Serial.print("0");
    }
    Serial.print(powerData[index], HEX);

    if ((index % 4) == 3) {
      Serial.println();
    } else {
      Serial.print(" ");
    }
  }

  Serial.println();
  Serial.println("01_power_data passed");
}

void loop() { delay(1000); }
