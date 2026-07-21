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

  Serial.println("CH224A hardware test 00_basic");

  if (!ch224.begin()) {
    clearOutputsAndHalt("CH224A begin failed");
  }
  ch224Ready = true;
  Serial.println("CH224A begin succeeded");

  if (ch224.getVariant() != CH224_VARIANT_A) {
    clearOutputsAndHalt("Driver did not identify CH224A silicon");
  }
  Serial.println("Driver identified CH224A silicon");

  if (ch224.getI2CAddress() != CH224A_I2CADDR_DEFAULT) {
    clearOutputsAndHalt("CH224A did not initialize at address 0x23");
  }
  Serial.println("CH224A initialized at I2C address 0x23");
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
