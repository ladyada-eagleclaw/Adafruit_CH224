#include <Adafruit_CH224.h>

Adafruit_CH224A ch224;
Adafruit_CH224 detector;
Adafruit_CH224Q ch224q;
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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("CH224A hardware test 03_variant_guard");

  if (!ch224.begin()) {
    clearOutputsAndHalt("CH224A begin failed");
  }
  ch224Ready = true;
  pinMode(voutSensePin, INPUT);
  Serial.println("CH224A subclass begin succeeded");

  if (ch224.getVariant() != CH224_VARIANT_A) {
    clearOutputsAndHalt("CH224A subclass reported the wrong variant");
  }
  Serial.println("CH224A subclass reported CH224A silicon");

  if (ch224.getI2CAddress() != CH224A_I2CADDR_DEFAULT) {
    clearOutputsAndHalt("CH224A subclass did not use address 0x23");
  }
  Serial.println("CH224A subclass used I2C address 0x23");
  Serial.println();

  if (!ch224.setVoltage(CH224_VOLTAGE_5V)) {
    clearOutputsAndHalt("Initial 5 V request failed");
  }
  Serial.println("Initial 5 V request succeeded");
  delay(1000);

  if (ch224.supportsPPS() || ch224.supportsAVS()) {
    clearOutputsAndHalt("CH224A incorrectly reports Q-only support");
  }
  Serial.println("CH224A reports no PPS or AVS control support");

  if (!detector.begin()) {
    clearOutputsAndHalt("Base-class detection failed");
  }
  Serial.println("Base-class detection succeeded");

  if (detector.getVariant() != CH224_VARIANT_A) {
    clearOutputsAndHalt("Base-class detection did not report CH224A");
  }
  Serial.println("Base-class detection reported CH224A");

  if (ch224q.begin()) {
    clearOutputsAndHalt("CH224Q subclass accepted CH224A hardware");
  }
  Serial.println("CH224Q subclass rejected CH224A hardware");

  if (ch224q.setAVSVoltage(15.0)) {
    clearOutputsAndHalt("CH224A accepted an AVS request");
  }
  Serial.println("CH224A AVS request was safely rejected");

  float measuredVolts = readVoutVoltage();
  Serial.print("Measured VOUT after rejected request: ");
  Serial.print(measuredVolts, 2);
  Serial.println(" V");
  if (abs(measuredVolts - 5.0) > voltageTolerance) {
    clearOutputsAndHalt("Rejected AVS request changed VOUT");
  }
  Serial.println("VOUT remained at 5 V");

  Serial.println("03_variant_guard passed");
}

void loop() { delay(1000); }
