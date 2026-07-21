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

void requestAndCheck(ch224_voltage_t setting, float volts,
                     float expectedAmps) {
  Serial.print("Requesting ");
  Serial.print(volts, 0);
  Serial.println(" V");

  if (!ch224.setVoltage(setting)) {
    clearOutputsAndHalt("Voltage request failed");
  }
  Serial.println("Voltage request write succeeded");
  delay(1500);

  uint8_t status = 0;
  if (!ch224.getStatus(&status)) {
    clearOutputsAndHalt("Status read failed");
  }
  Serial.print("Status read succeeded: 0x");
  Serial.println(status, HEX);

  if ((status & CH224_PROTOCOL_PD) == 0) {
    clearOutputsAndHalt("USB PD contract is not active");
  }
  Serial.println("USB PD contract is active");

  float amps = 0.0;
  if (!ch224.getMaxCurrent(&amps)) {
    clearOutputsAndHalt("Current read failed");
  }
  Serial.print("Current read succeeded: ");
  Serial.print(amps, 2);
  Serial.println(" A");

  if (abs(amps - expectedAmps) > 0.05) {
    clearOutputsAndHalt("Negotiated current does not match source capability");
  }
  Serial.println("Negotiated current matches source capability");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("CH224 hardware test 02_fixed_voltage_sweep");

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

  requestAndCheck(CH224_VOLTAGE_5V, 5.0, 3.0);
  requestAndCheck(CH224_VOLTAGE_9V, 9.0, 2.5);
  requestAndCheck(CH224_VOLTAGE_15V, 15.0, 1.5);
  requestAndCheck(CH224_VOLTAGE_20V, 20.0, 1.25);
  requestAndCheck(CH224_VOLTAGE_5V, 5.0, 3.0);

  Serial.println("02_fixed_voltage_sweep passed");
}

void loop() { delay(1000); }
