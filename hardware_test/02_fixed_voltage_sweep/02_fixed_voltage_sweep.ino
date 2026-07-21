#include <Adafruit_CH224.h>

Adafruit_CH224A ch224;
bool ch224Ready = false;

const uint16_t voutSensePin = A0;
const float dividerTopOhms = 10000.0;
const float dividerBottomOhms = 560.0;
const float voltageTolerance = 1.0;

float readVoutVoltage() {
  uint32_t adcTotal = 0;
  const uint8_t sampleCount = 16;

  for (uint8_t sample = 0; sample < sampleCount; sample++) {
    adcTotal += analogRead(voutSensePin);
    delay(5);
  }

  uint16_t adcValue = adcTotal / sampleCount;
  float senseVolts = map(adcValue, 0, 1023, 0, 5000) / 1000.0;
  float dividerRatio =
      (dividerTopOhms + dividerBottomOhms) / dividerBottomOhms;

  Serial.print("A0 averaged ADC reading: ");
  Serial.println(adcValue);
  return senseVolts * dividerRatio;
}

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

void requestAndCheck(ch224_voltage_t setting, float volts) {
  float expectedAmps = 0.0;
  if (!ch224.getAvailableCurrent(setting, &expectedAmps)) {
    clearOutputsAndHalt("Requested voltage is not advertised by the source");
  }

  Serial.print("Requesting ");
  Serial.print(volts, 0);
  Serial.print(" V at up to ");
  Serial.print(expectedAmps, 2);
  Serial.println(" A");

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

  float measuredVolts = readVoutVoltage();
  Serial.print("Measured VOUT: ");
  Serial.print(measuredVolts, 2);
  Serial.println(" V");

  if (abs(measuredVolts - volts) > voltageTolerance) {
    clearOutputsAndHalt("Measured VOUT does not match requested voltage");
  }
  Serial.println("Measured VOUT matches requested voltage");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("CH224A hardware test 02_fixed_voltage_sweep");

  if (!ch224.begin()) {
    clearOutputsAndHalt("CH224A begin failed");
  }
  ch224Ready = true;
  Serial.println("CH224A begin succeeded");

  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    clearOutputsAndHalt("Initial 5 V request failed");
  }
  Serial.println("Initial 5 V request succeeded");

  pinMode(voutSensePin, INPUT);

  if (ch224.getVariant() != CH224_VARIANT_A) {
    clearOutputsAndHalt("Driver did not identify CH224A silicon");
  }
  Serial.println("Driver identified CH224A silicon");

  if (ch224.getI2CAddress() != CH224A_I2CADDR_DEFAULT) {
    clearOutputsAndHalt("CH224A did not initialize at address 0x23");
  }
  Serial.println("CH224A initialized at I2C address 0x23");
  Serial.println();

  requestAndCheck(CH224_VOLTAGE_5V, 5.0);
  requestAndCheck(CH224_VOLTAGE_9V, 9.0);
  requestAndCheck(CH224_VOLTAGE_15V, 15.0);
  requestAndCheck(CH224_VOLTAGE_20V, 20.0);
  requestAndCheck(CH224_VOLTAGE_5V, 5.0);

  Serial.println("02_fixed_voltage_sweep passed");
}

void loop() { delay(1000); }
