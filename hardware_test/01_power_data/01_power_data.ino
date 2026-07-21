#include <Adafruit_CH224.h>

Adafruit_CH224 ch224;
bool ch224Ready = false;

void clearOutputsAndHalt(const char *message) {
  Serial.println(message);

  if (ch224Ready) {
    if (ch224.setVoltage(CH224_VOLTAGE_5V)) {
      Serial.println("Returned CH224 request to 5 V");
    } else {
      Serial.println("Could not return CH224 request to 5 V");
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

  Serial.println("CH224 hardware test 01_power_data");

  uint8_t address = CH224A_I2CADDR_DEFAULT;
  if (!ch224.begin(address)) {
    address = CH224Q_I2CADDR_DEFAULT;
    if (!ch224.begin(address)) {
      clearOutputsAndHalt("CH224A/Q begin failed");
    }
  }
  ch224Ready = true;

  Serial.print("Begin succeeded at I2C address 0x");
  Serial.println(address, HEX);
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
