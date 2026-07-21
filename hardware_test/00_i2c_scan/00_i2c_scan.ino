#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Wire.begin();
  Serial.println("Metro Mini I2C scan");
}

void loop() {
  uint8_t deviceCount = 0;

  Serial.print("SDA level: ");
  Serial.println(digitalRead(SDA) ? "HIGH" : "LOW");
  Serial.print("SCL level: ");
  Serial.println(digitalRead(SCL) ? "HIGH" : "LOW");

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
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.print("I2C scan succeeded, device count: ");
    Serial.println(deviceCount);
  }

  Serial.println();
  delay(2000);
}
