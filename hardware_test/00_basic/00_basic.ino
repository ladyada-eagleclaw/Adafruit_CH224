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

  Serial.println("CH224 hardware test 00_basic");

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

  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    clearOutputsAndHalt("5 V request failed");
  }
  Serial.println("5 V request succeeded");
  delay(500);

  uint8_t status = 0;
  if (!ch224.getStatus(&status)) {
    clearOutputsAndHalt("Status read failed");
  }
  Serial.print("Status read succeeded: 0x");
  Serial.println(status, HEX);

  float amps = 0.0;
  if (!ch224.getMaxCurrent(&amps)) {
    clearOutputsAndHalt("Current read failed");
  }
  Serial.print("Current read succeeded: ");
  Serial.print(amps, 2);
  Serial.println(" A");
  Serial.println();

  Serial.println("00_basic passed");
}

void loop() {
  delay(1000);
}
